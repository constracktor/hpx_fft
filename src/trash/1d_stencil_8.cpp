//  Copyright (c) 2014-2022 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the eighth in a series of examples demonstrating the development
// of a fully distributed solver for a simple 1D heat distribution problem.
//
// This example builds upon and extends example seven.
#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/modules/collectives.hpp>
#include <hpx/modules/type_support.hpp>
#include <hpx/serialization.hpp>
#include <hpx/modules/collectives.hpp>
#include <hpx/iostream.hpp>
//HPX 1.9.0 apply -> post #include <hpx/async_distributed/detail/post.hpp>

#include <boost/shared_array.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <stack>
#include <string>
#include <utility>
#include <vector>
///////////////////////////////////////////////////////////////////////////////
// Command-line variables
bool header = true;    // print csv heading
bool print_results = false;
double k = 0.5;    // heat transfer coefficient
double dt = 1.;    // time step
double dx = 1.;    // grid spacing

char const* stepper_basename = "/1d_stencil_8/stepper/";
char const* gather_basename = "/1d_stencil_8/gather/";

/////////////////////////////////////////////////////////////////////////////
// Use a special allocator for the partition data to remove a major contention
// point - the constant allocation and deallocation of the data arrays.
template <typename T>
struct partition_allocator
{
private:
    typedef hpx::spinlock mutex_type;

public:
    explicit partition_allocator(std::size_t max_size = std::size_t(-1))
      : max_size_(max_size)
    {
    }

    ~partition_allocator()
    {
        std::lock_guard<mutex_type> l(mtx_);
        while (!heap_.empty())
        {
            T* p = heap_.top();
            heap_.pop();
            delete[] p;
        }
    }

    T* allocate(std::size_t n)
    {
        std::lock_guard<mutex_type> l(mtx_);
        if (heap_.empty())
        {
            return new T[n];
        }

        T* next = heap_.top();
        heap_.pop();
        return next;
    }

    void deallocate(T* p)
    {
        std::lock_guard<mutex_type> l(mtx_);
        if (max_size_ == static_cast<std::size_t>(-1) ||
            heap_.size() < max_size_)
            heap_.push(p);
        else
            delete[] p;
    }

private:
    mutex_type mtx_;
    std::size_t max_size_;
    std::stack<T*> heap_;
};

///////////////////////////////////////////////////////////////////////////////
struct partition_data
{
private:
    typedef hpx::serialization::serialize_buffer<double> buffer_type;

    struct hold_reference
    {
        hold_reference(buffer_type const& data)
          : data_(data)
        {
        }

        void operator()(double*) {}    // no deletion necessary

        buffer_type data_;
    };

    static void deallocate(double* p) noexcept
    {
        alloc_.deallocate(p);
    }

    static partition_allocator<double> alloc_;

public:
    partition_data()
      : size_(0)
      , min_index_(0)
    {
    }

    // Create a new (uninitialized) partition of the given size.
    explicit partition_data(std::size_t size)
      : data_(alloc_.allocate(size), size, buffer_type::take,
            &partition_data::deallocate)
      , size_(size)
      , min_index_(0)
    {
    }

    // Create a new (initialized) partition of the given size.
    partition_data(std::size_t size, double initial_value)
      : data_(alloc_.allocate(size), size, buffer_type::take,
            &partition_data::deallocate)
      , size_(size)
      , min_index_(0)
    {
        double base_value = double(initial_value * size);
        for (std::size_t i = 0; i != size; ++i)
            data_[i] = base_value + double(i);
    }

    // Create a partition which acts as a proxy to a part of the embedded array.
    // The proxy is assumed to refer to either the left or the right boundary
    // element.
    partition_data(partition_data const& base, std::size_t min_index)
      : data_(base.data_.data() + min_index, 1, buffer_type::reference,
            hold_reference(base.data_))
      ,    // keep referenced partition alive
      size_(base.size())
      , min_index_(min_index)
    {
        HPX_ASSERT(min_index < base.size());
    }

    double& operator[](std::size_t idx)
    {
        return data_[index(idx)];
    }
    double operator[](std::size_t idx) const
    {
        return data_[index(idx)];
    }

    std::size_t size() const
    {
        return size_;
    }

private:
    std::size_t index(std::size_t idx) const
    {
        HPX_ASSERT(idx >= min_index_ && idx < size_);
        return idx - min_index_;
    }

private:
    // Serialization support: even if all of the code below runs on one
    // locality only, we need to provide an (empty) implementation for the
    // serialization as all arguments passed to actions have to support this.
    friend class hpx::serialization::access;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        // clang-format off
        ar & data_ & size_ & min_index_;
        // clang-format on
    }

private:
    buffer_type data_;
    std::size_t size_;
    std::size_t min_index_;
};

partition_allocator<double> partition_data::alloc_;

std::ostream& operator<<(std::ostream& os, partition_data const& c)
{
    os << "{";
    for (std::size_t i = 0; i != c.size(); ++i)
    {
        if (i != 0)
            os << ", ";
        os << c[i];
    }
    os << "}";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
inline std::size_t idx(std::size_t i, int dir, std::size_t size)
{
    if (i == 0 && dir == -1)
        return size - 1;
    if (i == size - 1 && dir == +1)
        return 0;

    HPX_ASSERT((i + dir) < size);

    return i + dir;
}

///////////////////////////////////////////////////////////////////////////////
// This is the server side representation of the data. We expose this as a HPX
// component which allows for it to be created and accessed remotely through
// a global address (hpx::id_type).
struct partition_server : hpx::components::component_base<partition_server>
{
    // construct new instances
    partition_server() = default;

    explicit partition_server(partition_data const& data)
      : data_(data)
    {
    }

    partition_server(std::size_t size, double initial_value)
      : data_(size, initial_value)
    {
    }

    // Access data. The parameter specifies what part of the data should be
    // accessed. As long as the result is used locally, no data is copied,
    // however as soon as the result is requested from another locality only
    // the minimally required amount of data will go over the wire.
    partition_data get_data() const
    {
        return data_;
    }

    // Every member function which has to be invoked remotely needs to be
    // wrapped into a component action. The macro below defines a new type
    // 'get_data_action' which represents the (possibly remote) member function
    // partition::get_data().
    HPX_DEFINE_COMPONENT_DIRECT_ACTION(
        partition_server, get_data, get_data_action)

private:
    partition_data data_;
};

// The macros below are necessary to generate the code required for exposing
// our partition type remotely.
//
// HPX_REGISTER_COMPONENT() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::component<partition_server> partition_server_type;
HPX_REGISTER_COMPONENT(partition_server_type, partition_server)

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef partition_server::get_data_action get_data_action;
HPX_REGISTER_ACTION(get_data_action)

///////////////////////////////////////////////////////////////////////////////
// This is a client side helper class allowing to hide some of the tedious
// boilerplate while referencing a remote partition.
struct partition : hpx::components::client_base<partition, partition_server>
{
    typedef hpx::components::client_base<partition, partition_server> base_type;

    partition() = default;

    // Create new component on locality 'where' and initialize the held data
    partition(hpx::id_type where, std::size_t size, double initial_value)
      : base_type(hpx::new_<partition_server>(where, size, initial_value))
    {
    }

    // Create a new component on the locality co-located to the id 'where'. The
    // new instance will be initialized from the given partition_data.
    partition(hpx::id_type where, partition_data const& data)
      : base_type(hpx::new_<partition_server>(hpx::colocated(where), data))
    {
    }

    // Attach a future representing a (possibly remote) partition.
    partition(hpx::future<hpx::id_type>&& id) noexcept
      : base_type(std::move(id))
    {
    }

    // Unwrap a future<partition> (a partition already is a future to the
    // id of the referenced object, thus unwrapping accesses this inner future).
    partition(hpx::future<partition>&& c) noexcept
      : base_type(std::move(c))
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    // Invoke the (remote) member function which gives us access to the data.
    // This is a pure helper function hiding the async.
    hpx::future<partition_data> get_data() const
    {
        partition_server::get_data_action act;
        return hpx::async(act, get_id());
    }
};

///////////////////////////////////////////////////////////////////////////////
// Data for one time step on one locality
struct stepper_server : hpx::components::component_base<stepper_server>
{
    // Our data for one locality
    typedef std::vector<partition> space;
    // Buffer
    typedef hpx::lcos::local::receive_buffer<partition> communication_buffer;

    stepper_server() = default;

    explicit stepper_server(std::size_t nl)
    {
        // locality_ids_.resize(nl);
        // for(std::size_t i = 0; i!=nl; ++i)
        // {
        //     locality_ids_[i] = hpx::find_from_basename(
        //     stepper_basename, i);
        // }
        //communication_buffer.resize(nl);
    }

    // Do all the work on 'np' partitions, 'nx' data points each, for 'nt'
    // time steps
    space do_work(std::size_t local_np, std::size_t nx, std::size_t nt);

    HPX_DEFINE_COMPONENT_ACTION(stepper_server, do_work, do_work_action)

    void from_get(std::size_t t, partition p)
    {
        receive_buffer_.store_received(t, std::move(p));
    }

    HPX_DEFINE_COMPONENT_ACTION(stepper_server, from_get, from_get_action)

    // release dependencies
    void release_dependencies()
    {
        // left_ = hpx::shared_future<hpx::id_type>();
        // right_ = hpx::shared_future<hpx::id_type>();
    }

    HPX_DEFINE_COMPONENT_ACTION(
        stepper_server, release_dependencies, release_dependencies_action)

protected:
    // The partitioned operator, it invokes the heat operator above on all
    // elements of a partition.
    static partition heat_part(
        partition const& part);

    partition receive_from(std::size_t t)
    {
        return receive_buffer_.receive(t);
    }

    // Helper functions to send our left and right boundary elements to
    // the neighbors.
    inline void send_to(hpx::shared_future<hpx::id_type> loc_id, std::size_t t, partition p) const;

private:
    
    space local_partitions;
    communication_buffer receive_buffer_;
    std::vector<hpx::shared_future<hpx::id_type>> locality_ids_;
    hpx::components::client<partition_server> left_;
};

// The macros below are necessary to generate the code required for exposing
// our partition type remotely.
//
// HPX_REGISTER_COMPONENT() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::component<stepper_server> stepper_server_type;
HPX_REGISTER_COMPONENT(stepper_server_type, stepper_server)

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef stepper_server::from_get_action from_get_action;
HPX_REGISTER_ACTION(from_get_action)

typedef stepper_server::do_work_action do_work_action;
HPX_REGISTER_ACTION(do_work_action)

typedef stepper_server::release_dependencies_action release_dependencies_action;
HPX_REGISTER_ACTION(release_dependencies_action)
//hpx::shared_future<hpx::id_type>
void stepper_server::send_to(hpx::shared_future<hpx::id_type> loc_id, std::size_t t, partition p) const
{
    hpx::apply(from_get_action(), loc_id.get(), t, std::move(p));
}

///////////////////////////////////////////////////////////////////////////////
// This is a client side member function can now be implemented as the
// stepper_server has been defined.
struct stepper : hpx::components::client_base<stepper, stepper_server>
{
    typedef hpx::components::client_base<stepper, stepper_server> base_type;

    // construct new instances/wrap existing steppers from other localities
    explicit stepper(std::size_t num_localities)
      : base_type(hpx::new_<stepper_server>(hpx::find_here(), num_localities))
    {
        hpx::register_with_basename(
            stepper_basename, get_id(), hpx::get_locality_id());
    }

    stepper(hpx::future<hpx::id_type>&& id) noexcept
      : base_type(std::move(id))
    {
    }

    ~stepper()
    {
        // break cyclic dependencies
        hpx::future<void> f1 =
            hpx::async(release_dependencies_action(), get_id());

        // release the reference held by AGAS
        hpx::future<void> f2 = hpx::unregister_with_basename(
            stepper_basename, hpx::get_locality_id());

        hpx::wait_all(f1, f2);    // ignore exceptions
    }

    hpx::future<stepper_server::space> do_work(
        std::size_t local_np, std::size_t nx, std::size_t nt)
    {
        return hpx::async(do_work_action(), get_id(), local_np, nx, nt);
    }
};

///////////////////////////////////////////////////////////////////////////////
// The partitioned operator, it invokes the heat operator above on all elements
// of a partition.
partition stepper_server::heat_part(partition const& part)
{
    hpx::shared_future<partition_data> part_data =
        part.get_data();

    hpx::future<partition_data> next_part = part_data.then(
        hpx::unwrapping([part](partition_data const& p) -> partition_data {
            HPX_UNUSED(part);

            std::size_t size = p.size();
            partition_data next(size);
            for (std::size_t i = 1; i != size - 1; ++i)
                next[i] = p[i] + 0.5;
            return next;
        }));
    
    return hpx::dataflow(hpx::launch::async,
        hpx::unwrapping(
            [part](partition_data const& next) -> partition {

                return partition(part.get_id(), next);
            }),
        std::move(next_part));
}

///////////////////////////////////////////////////////////////////////////////
// This is the implementation of the time step loop
//
// Do all the work on 'np' partitions, 'nx' data points each, for 'nt'
// time steps, limit depth of dependency tree to 'nd'.
stepper_server::space stepper_server::do_work(
    std::size_t local_np, std::size_t nx, std::size_t nt)
{
    // resize space local_partitions -> vector of partitions
    local_partitions.resize(local_np);


    // Initial condition
    hpx::id_type this_locality = hpx::find_here();
    std::size_t this_id = hpx::get_locality_id();

    for (std::size_t i = 0; i != local_np; ++i)
    {
        local_partitions[i] = partition(this_locality, nx, double(i + nx* this_id));
        //std::cout << local_partitions[i].get_data().get() << std::endl;
    }
    
    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();    // Number of localities


    for (std::size_t i = 0; i != local_np; ++i)
    {
        // auto current_partition = local_partitions[i].get_data().get();
        // //fft r2c 2D
        // char const* msg = "From locality {1}: {2}  -  {3}\n";
        // hpx::util::format_to(hpx::cout, msg, hpx::get_locality_id(), current_partition[0], current_partition[1])
        //     << std::flush;
        partition& current_partition = local_partitions[i];
        std::cout << "hello for " << this_id << std::endl;
        current_partition =
                hpx::dataflow(hpx::launch::async, &stepper_server::heat_part, current_partition); 

        auto a = current_partition.get_data().get();
        char const* msg = "This locality {1}: {2}  -  {3}\n";
        hpx::util::format_to(hpx::cout, msg, hpx::get_locality_id(), a[1], a[2])
            << std::flush;

        
        //rearrange

        //communicate

                
                
        // local 
        // for (std::size_t other_locality = 0; other_locality != nl; ++other_locality)
        // {
        //     if( true)//this_locality != localities[other_locality])
        //     {
        //         send_to(localities[other_locality], current_partition);
        //         //send_to(other_locality, current_partition);
        //     }
        // }
        //size_t id = this_id == 0 ? 1 : 0;
        if(this_id == 0){
            //hpx::components::client<stepper_server> other_locality = hpx::find_from_basename(stepper_basename, 0);  
            hpx::shared_future<hpx::id_type> other_locality = hpx::find_from_basename(stepper_basename, 1);
            hpx::shared_future<hpx::id_type>  t_locality = hpx::make_ready_future(localities[0]);//hpx::find_from_basename(stepper_basename, 0);

                        char const* msg1 = "Send from locality {1} to locality {2}. All: {3} -- {4}\n";
            hpx::util::format_to(hpx::cout, msg1, t_locality.get(), other_locality.get(), localities[0], localities[1])
                << std::flush;  


            send_to(other_locality, 1, current_partition); 
        }
        // else
        // {
        //     hpx::components::client<stepper_server> other_locality = hpx::find_from_basename(stepper_basename, idx(hpx::get_locality_id(), -1, nl));   
        //     send_to(other_locality, current_partition); 
        // }
        // hpx::components::client<stepper_server> other_locality = hpx::find_from_basename(stepper_basename, id);

        // send_to(other_locality, current_partition);
        //std::cout << "hello send" << std::endl;
        if(this_id == 1){
            partition b = receive_from(1);

            auto c= b.get_data().get();

            char const* msg1 = "Received from locality {1} on locality {2}: {3}  -  {4}\n";
            hpx::util::format_to(hpx::cout, msg1, this_id,this_id, c[1], c[2])
                << std::flush;  

        }
        // partition b = receive_from();
    
        //         auto c= b.get_data().get();

        // std::cout << "hello in" << std::endl;
        // char const* msg = "Received from locality {1} on locality {2}: {3}  -  {4}\n";
        // hpx::util::format_to(hpx::cout, msg, this_id,this_id, c[1], c[2])
        //     << std::flush;
        // global
        // for (std::size_t other_locality = 0; other_locality != nl; ++other_locality)
        // {
        //     std::cout << "hello out" << std::endl;
        //     if( true)//this_locality != localities[other_locality])
        //     {
        //         partition b = receive_from();
        //         auto c= b.get_data().get();
        //         std::cout << "hello in" << std::endl;
        //         char const* msg = "Received from locality {1} on locality {2}: {3}  -  {4}\n";
        //         hpx::util::format_to(hpx::cout, msg, other_locality,this_id, c[1], c[2])
        //             << std::flush;
        //         // char const* msg = "From locality {1} access locality {2}\n";

        //         // hpx::util::format_to(hpx::cout, msg, hpx::get_locality_id(), other_locality)
        //         //     << std::flush;
        //     }
        // }

        //fft c2c 1D

        // TODO backwards fft3d
    }
    return local_partitions;
        // send_left(0, U_[0][0]);
        // send_right(0, U_[0][local_np - 1]);

        // next[0] =
        //     hpx::dataflow(hpx::launch::async, &stepper_server::heat_part,
        //         receive_left(t), current[0], current[1]);

        // next[i] = hpx::dataflow(hpx::launch::async,
        //     &stepper_server::heat_part, current[i - 1], current[i],
        //     current[i + 1]);

        // next[local_np - 1] = hpx::dataflow(hpx::launch::async,
        //     &stepper_server::heat_part, current[local_np - 2],
        //     current[local_np - 1], receive_right(t));
}

///////////////////////////////////////////////////////////////////////////////
void do_all_work(
    std::uint64_t nt, std::uint64_t nx, std::uint64_t np)
{
    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();    // Number of localities

    if (np < nl)
    {
        std::cout << "The number of partitions should not be smaller than "
                     "the number of localities"
                  << std::endl;
        return;
    }

    // Create the local stepper instance, register it
    stepper step(nl);

    // Measure execution time.
    std::uint64_t t = hpx::chrono::high_resolution_clock::now();


    ////////////////////////////////////////////////
    // Perform all work and wait for it to finish
    hpx::future<stepper_server::space> result =
        step.do_work(np / nl, nx, nt);

    ////////////////////////////////////////////////
    

    std::uint64_t elapsed = hpx::chrono::high_resolution_clock::now() - t;
    // The HPX-thread has been run on the desired OS-thread.
    char const* msg = "Locality {1}: {2} micros\n";

    hpx::util::format_to(hpx::cout, msg, hpx::get_locality_id(), elapsed/1000)
        << std::flush;
    
    // // Gather results from all localities
    // if (0 == hpx::get_locality_id())
    // {
    //     // hpx::future<std::vector<stepper_server::space>> overall_result =
    //     //     result.then([&](hpx::future<stepper_server::space>&& result) {
    //     //         return hpx::collectives::gather_here(gather_basename,
    //     //             result.get(), hpx::collectives::num_sites_arg(nl));
    //     //     });

    //     // std::vector<stepper_server::space> solution = overall_result.get();
    //     // for (std::size_t i = 0; i != nl; ++i)
    //     // {
    //     //     stepper_server::space const& s = solution[i];
    //     //     for (std::size_t i = 0; i != s.size(); ++i)
    //     //     {
    //     //         s[i].get_data(partition_server::middle_partition).get();
    //     //     }
    //     // }

    //     std::uint64_t elapsed = hpx::chrono::high_resolution_clock::now() - t;
    //             // The HPX-thread has been run on the desired OS-thread.
    //     char const* msg = "Locality {0}: {1}s\n";

    //     hpx::util::format_to(hpx::cout, msg, hpx::get_locality_id(), elapsed)
    //         << std::flush;
    // }
    // else // other localities
    // {
    //     result
    //         .then([&](hpx::future<stepper_server::space>&& result) {
    //             hpx::collectives::gather_there(gather_basename, result.get());
    //         })
    //         .get();
    // }
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main(hpx::program_options::variables_map& vm)
{
    std::uint64_t nx =
        vm["nx"].as<std::uint64_t>();    // Number of grid points.
    std::uint64_t np = vm["np"].as<std::uint64_t>();    // Number of partitions.

    if (vm.count("no-header"))
        header = false;
    if (vm.count("results"))
        print_results = true;

    do_all_work(1, nx, np);

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()(
        "results", "print generated results (default: false)")("nx",
        value<std::uint64_t>()->default_value(10),
        "Local x dimension (of each partition)")("np",
        value<std::uint64_t>()->default_value(10),
        "Number of partitions")("k", value<double>(&k)->default_value(0.5),
        "Heat transfer coefficient (default: 0.5)")("dt",
        value<double>(&dt)->default_value(1.0),
        "Timestep unit (default: 1.0[s])")(
        "dx", value<double>(&dx)->default_value(1.0), "Local x dimension")(
        "no-header", "do not print out the csv header row");

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {"hpx.run_hpx_main!=1"};

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    return hpx::init(argc, argv, init_args);
}
#endif
