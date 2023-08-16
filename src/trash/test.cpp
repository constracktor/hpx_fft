//  Copyright (c) 2016-2022 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// The purpose of this example is to demonstrate how the partitioned_vector
// data structure can be used in an SPMD style way, i.e. how to execute code
// on each of the localities the partinioned_vector is located. Each locality
// touches only the elements located on this locality. The access to those
// elements is done directly, with best possible performance, working directly
// on the std::vector's the data is stored in.

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/algorithm.hpp>
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/partitioned_vector.hpp>
#include <hpx/iostream.hpp>


#include <hpx/modules/program_options.hpp>

#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <string>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
// Define the vector types to be used.
using plan = std::vector<double>;

HPX_REGISTER_PARTITIONED_VECTOR(double);

///////////////////////////////////////////////////////////////////////////////
//
// Define a view for a partitioned vector which exposes the part of the vector
// which is located on the current locality.
//
// This view does not own the data and relies on the partitioned_vector to be
// available during the full lifetime of the view.
//
template <typename T>
struct partitioned_vector_view
{
private:
    typedef typename hpx::partitioned_vector<T>::iterator global_iterator;
    typedef typename hpx::partitioned_vector<T>::const_iterator
        const_global_iterator;

    typedef hpx::traits::segmented_iterator_traits<global_iterator> traits;
    typedef hpx::traits::segmented_iterator_traits<const_global_iterator>
        const_traits;

    typedef typename traits::local_segment_iterator local_segment_iterator;

public:
    typedef typename traits::local_raw_iterator iterator;
    typedef typename const_traits::local_raw_iterator const_iterator;
    typedef T value_type;

public:
    explicit partitioned_vector_view(hpx::partitioned_vector<T>& data)
      : segment_iterator_(data.segment_begin(hpx::get_locality_id()))
    {
#if defined(HPX_DEBUG)
        // this view assumes that there is exactly one segment per locality
        typedef typename traits::local_segment_iterator local_segment_iterator;
        local_segment_iterator sit = segment_iterator_;
        HPX_ASSERT(
            ++sit == data.segment_end(hpx::get_locality_id()));    // NOLINT
#endif
    }

    iterator begin()
    {
        return traits::begin(segment_iterator_);
    }
    iterator end()
    {
        return traits::end(segment_iterator_);
    }

    const_iterator begin() const
    {
        return const_traits::begin(segment_iterator_);
    }
    const_iterator end() const
    {
        return const_traits::end(segment_iterator_);
    }
    const_iterator cbegin() const
    {
        return begin();
    }
    const_iterator cend() const
    {
        return end();
    }

    value_type& operator[](std::size_t index)
    {
        return (*segment_iterator_)[index];
    }
    value_type const& operator[](std::size_t index) const
    {
        return (*segment_iterator_)[index];
    }

    std::size_t size() const
    {
        return (*segment_iterator_).size();
    }

private:
    local_segment_iterator segment_iterator_;
};

///////////////////////////////////////////////////////////////////////////////
int hpx_main(hpx::program_options::variables_map& vm)
{
    unsigned int seed = std::random_device{}();
    if (vm.count("seed"))
        seed = vm["seed"].as<unsigned int>();

    std::cout << "using seed: " << seed << std::endl;

    unsigned int size = 2;
    if (vm.count("size"))
        size = vm["size"].as<unsigned int>();

    char const* const example_vector_name =
        "partitioned_vector_spmd_foreach_example";
    char const* const example_latch_name = "latch_spmd_foreach_example";

    {
        // create vector on one locality, connect to it from all others
        hpx::partitioned_vector<plan> v;
        hpx::distributed::latch l;


        std::uint32_t this_loc = hpx::get_locality_id();
        if (0 == this_loc)
        {
            std::vector<hpx::id_type> localities = hpx::find_all_localities();

            v = hpx::partitioned_vector<plan>(
                size, hpx::container_layout(localities));
            v.register_as(example_vector_name);

            l = hpx::distributed::latch(localities.size());
            l.register_as(example_latch_name);
        }
        else
        {
            hpx::future<void> f1 = v.connect_to(example_vector_name);
            l.connect_to(example_latch_name);
            f1.get();
        }

        std::vector<double> local_v(10);
        for( double value : local_v)
        {
            value = 1.0;
        }

        partitioned_vector_view<plan> view(v); 
        // fill vector
        view[0][1] =1.0;
        //std::move(local_v.begin(), local_v.end(), view[0].begin());

        l.arrive_and_wait();        

       //std::vector<double> value1 = std::move(v[this_loc]);

        // print local
        // for( double values : value1)
        // {
        //    std::cout << values << std::endl;
        // }
        // char const* msg_l = "Locality {1}: Get element of own partitioned vector: {2}\n";

        // hpx::util::format_to(hpx::cout, msg_l, this_loc, v[this_loc])
        //     << std::flush;
        
        // // print distributed
        // std::vector<std::uint32_t> locality_ids = hpx::agas::get_all_locality_ids();
        // for (std::uint32_t loc_id : locality_ids)
        // {
        //     if (loc_id != this_loc)
        //     {
        //         char const* msg = "Locality {1}: From other locality {2} get element of partitioned vector: {3}\n";

        //         hpx::util::format_to(hpx::cout, msg, this_loc, loc_id, v[loc_id][0])
        //         << std::flush;
        //     }

        // }


        // std::mt19937 gen(seed);
        // std::uniform_int_distribution<> dist(0, 1000);

        // // fill the vector with random numbers
        // partitioned_vector_view<int> view(v);
        // hpx::generate(hpx::execution::par, view.begin(), view.end(),
        //     [&]() { return hpx::get_locality_id();});//dist(gen); });

        // // square all numbers in the array
        // hpx::ranges::for_each(
        //     hpx::execution::par, view, [](int& val) { val *= val; });

        // // do the same using a plain loop
        // std::size_t maxnum = view.size();
        // for (std::size_t i = 0; i != maxnum; ++i){
        //     //view[i] *= 2;
        //     char const* msg = "From locality {1} get element of partitioned vector: {2}\n";

        //     hpx::util::format_to(hpx::cout, msg, hpx::get_locality_id(), v[hpx::get_locality_id()])
        //     << std::flush;
        // }
        // Wait for all localities to reach this point.
        l.arrive_and_wait();
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // add command line option which controls the random number generator seed
    using namespace hpx::program_options;
    options_description desc_commandline(
        "Usage: " HPX_APPLICATION_STRING " [options]");

    // clang-format off
    desc_commandline.add_options()
        ("maxelems,m", value<unsigned int>(),
            "the data array size to use (default: 10000)")
        ("seed,s", value<unsigned int>(),
            "the random number generator seed to use for this run")
        ;
    // clang-format on

    // run hpx_main on all localities
    std::vector<std::string> const cfg = {"hpx.run_hpx_main!=1"};

    // Initialize and run HPX
    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    return hpx::init(argc, argv, init_args);
}
#endif