#include <hpx/config.hpp>

#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/iostream.hpp>
#include <hpx/timing/high_resolution_timer.hpp>


#include <iostream>
#include <string>
#include <vector>
#include <fftw3.h>

// use typedef later
using real = double;
using vector_1d = std::vector<real, std::allocator<real>>;
using vector_2d = std::vector<vector_1d>;

struct fft_server : hpx::components::component_base<fft_server>
{
    using fft_backend_plan = fftw_plan;

    public:

        fft_server() = default;

        void initialize(vector_2d values_vec);
        HPX_DEFINE_COMPONENT_ACTION(fft_server, initialize, initialize_action)

        
        vector_2d fft_2d_r2c();
        HPX_DEFINE_COMPONENT_ACTION(fft_server, fft_2d_r2c, fft_2d_r2c_action)

        virtual ~fft_server()
        {
            fftw_destroy_plan(plan_1d_r2c_);
            fftw_destroy_plan(plan_1d_c2c_);
            fftw_cleanup();
        }

    private:
        void fft_1d_r2c_inplace(const std::size_t i)
        {
            fftw_execute_dft_r2c(plan_1d_r2c_, values_vec_[i].data(), reinterpret_cast<fftw_complex*>(values_vec_[i].data()));
        }
        HPX_DEFINE_COMPONENT_ACTION(fft_server, fft_1d_r2c_inplace, fft_1d_r2c_inplace_action)

        void fft_1d_c2c_inplace(const std::size_t i)
        {
            fftw_execute_dft(plan_1d_c2c_, reinterpret_cast<fftw_complex*>(trans_values_vec_[i].data()), reinterpret_cast<fftw_complex*>(trans_values_vec_[i].data()));
        }
        HPX_DEFINE_COMPONENT_ACTION(fft_server, fft_1d_c2c_inplace, fft_1d_c2c_inplace_action)

        void transpose_shared_r2c(const std::size_t index_trans)
        {
            const std::size_t dim_input = dim_c_y_;
            for( std::size_t index = 0; index < dim_input; ++index)
            {
                trans_values_vec_[index][2 * index_trans] = values_vec_[index_trans][2 * index];
                trans_values_vec_[index][2 * index_trans + 1] = values_vec_[index_trans][2 * index + 1];
            }     
        }
        HPX_DEFINE_COMPONENT_ACTION(fft_server, transpose_shared_r2c, transpose_shared_r2c_action)

        void transpose_shared_c2c(const std::size_t index_trans)
        {
            const std::size_t dim_input = dim_c_x_;
            for( std::size_t index = 0; index < dim_input; ++index)
            {
                values_vec_[index][2 * index_trans] = trans_values_vec_[index_trans][2 * index];
                values_vec_[index][2 * index_trans + 1] = trans_values_vec_[index_trans][2 * index + 1];
            }     
        }      
        HPX_DEFINE_COMPONENT_ACTION(fft_server, transpose_shared_c2c, transpose_shared_c2c_action)
   
    private:
        unsigned PLAN_FLAG = FFTW_ESTIMATE;
        // constants
        std::size_t n_x_local_, n_y_local_;
        std::size_t dim_r_y_, dim_c_y_, dim_c_x_;
        // FFTW plans
        fft_backend_plan plan_1d_r2c_;
        fft_backend_plan plan_1d_c2c_;
        // value vectors
        vector_2d values_vec_;
        vector_2d trans_values_vec_;
        // future vector
        std::vector<hpx::future<void>> r2c_futures_;
        std::vector<hpx::future<void>> trans_r2c_futures_; 
        std::vector<hpx::future<void>> c2c_futures_;
        std::vector<hpx::future<void>> trans_c2c_futures_; 
};

//HPX_REGISTER_COMPONENT_MODULE()
// HPX_REGISTER_COMPONENT() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::component<fft_server> fft_server_type;
HPX_REGISTER_COMPONENT(fft_server_type, fft_server)

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.

typedef fft_server::fft_2d_r2c_action fft_2d_r2c_action;
HPX_REGISTER_ACTION(fft_2d_r2c_action)

typedef fft_server::initialize_action initialize_action;
HPX_REGISTER_ACTION(initialize_action)


///////////////////////////////////////////////////////////////////////////////
// This is a client side member function can now be implemented as the
// fft_server has been defined.
struct fft : hpx::components::client_base<fft, fft_server>
{
    typedef hpx::components::client_base<fft, fft_server> base_type;

    explicit fft()
      : base_type(hpx::new_<fft_server>(hpx::find_here()))
    {
    }

    hpx::future<vector_2d> fft_2d_r2c()
    {
        return hpx::async(fft_2d_r2c_action(), get_id());
    }

    hpx::future<void> initialize(vector_2d values_vec)
    {
        return hpx::async(initialize_action(), get_id(), std::move(values_vec));
    }

    ~fft() = default;
};

vector_2d fft_server::fft_2d_r2c()
{
    ////////////////////////////////
    // FFTW 1d r2c in first dimension
    for(std::size_t i = 0; i < n_x_local_; ++i)
    {
        r2c_futures_[i] = hpx::async(fft_1d_r2c_inplace_action(),get_id(), i);
        
        // transpose
        trans_r2c_futures_[i] = r2c_futures_[i].then(
            [=](hpx::future<void> r)
            {
                r.get();
                return hpx::async(transpose_shared_r2c_action(), get_id(), i);
            }); 
    }
    // synchronization step
    hpx::shared_future<std::vector<hpx::future<void>>> all_trans_r2c_futures = hpx::when_all(trans_r2c_futures_);
    all_trans_r2c_futures.get();
    /////////////////////////////////
    // FFTW 1d c2c in second dimension
    for(std::size_t i = 0; i < n_y_local_; ++i)
    {
        c2c_futures_[i] = hpx::async(fft_1d_c2c_inplace_action(), get_id(), i);
        //all_trans_r2c_futures.then(
            // [=](hpx::future<void> r)
            // {
            //     r.get();
            //     return hpx::async(fft_1d_c2c_inplace_action(), get_id(), i);
            // });     
        // transpose
        trans_c2c_futures_[i] = c2c_futures_[i].then(
            [=](hpx::future<void> r)
            {
                r.get();
                return hpx::async(transpose_shared_c2c_action(), get_id(), i);
            }); 
    }
    // synchronization step
    hpx::shared_future<std::vector<hpx::future<void>>> all_trans_c2c_futures = hpx::when_all(trans_c2c_futures_);
    all_trans_c2c_futures.get();
    return std::move(values_vec_);
}

void fft_server::initialize(vector_2d values_vec)
{
    // move data into own structure
    values_vec_ = std::move(values_vec);
    // parameters
    n_x_local_ = values_vec_.size();
    dim_c_x_ = n_x_local_;
    dim_c_y_ = values_vec_[0].size() / 2;
    dim_r_y_ = 2 * dim_c_y_ - 2;
    n_y_local_ = dim_c_y_;
    // resize other data structures
    trans_values_vec_.resize(n_y_local_);
    for(std::size_t i = 0; i < n_y_local_; ++i)
    {
        trans_values_vec_[i].resize(2 * dim_c_x_);
    }
    //create fftw plans
    // forward step one: r2c in y-direction
    plan_1d_r2c_ = fftw_plan_dft_r2c_1d(dim_r_y_,
                                       values_vec_[0].data(),
                                       reinterpret_cast<fftw_complex*>(values_vec_[0].data()),
                                       PLAN_FLAG);
    // forward step two: c2c in x-direction
    plan_1d_c2c_ = fftw_plan_dft_1d(dim_c_x_, 
                                   reinterpret_cast<fftw_complex*>(trans_values_vec_[0].data()), 
                                   reinterpret_cast<fftw_complex*>(trans_values_vec_[0].data()), 
                                   FFTW_FORWARD,
                                   PLAN_FLAG);
    // resize futures
    r2c_futures_.resize(n_x_local_);
    trans_r2c_futures_.resize(n_x_local_);
    c2c_futures_.resize(n_y_local_);
    trans_c2c_futures_.resize(n_y_local_);
}


int hpx_main(hpx::program_options::variables_map& vm)
{
     ////////////////////////////////////////////////////////////////
    // Parameters and Data structures
    // hpx parameters
    const std::size_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    // test if one locality
    HPX_TEST_LTE(std::size_t(1), num_localities);
    //std::size_t generation_counter = 1;
    // fft dimension parameters
    const std::size_t dim_c_x = vm["nx"].as<std::size_t>();//N_X; 
    const std::size_t dim_r_y = vm["ny"].as<std::size_t>();//N_Y;
    const std::size_t dim_c_y = dim_r_y / 2 + 1;
    // division parameters
    const std::size_t n_x_local = dim_c_x / num_localities;
    // value vector
    vector_2d values_vec(n_x_local);
    // FFTW plans
    std::string plan_flag = vm["plan"].as<std::string>();
    unsigned FFT_BACKEND_PLAN_FLAG = FFTW_ESTIMATE;
    if( plan_flag == "measure" )
    {
        FFT_BACKEND_PLAN_FLAG = FFTW_MEASURE;
    }
    else if ( plan_flag == "patient")
    {
        FFT_BACKEND_PLAN_FLAG = FFTW_PATIENT;
    }
    else if ( plan_flag == "exhaustive")
    {
        FFT_BACKEND_PLAN_FLAG = FFTW_EXHAUSTIVE;
    }
    //
    std::string run_flag = vm["run"].as<std::string>();
    bool print_result = vm["result"].as<bool>();

    ////////////////////////////////////////////////////////////////
    // initialize values
    for(std::size_t i = 0; i < n_x_local; ++i)
    {
        values_vec[i].resize(2*dim_c_y);
        std::iota(values_vec[i].begin(), values_vec[i].end() - 2, 0.0);
    }
    ////////////////////////////////////////////////////////////////
    // computation   
    // time measurement
    auto t = hpx::chrono::high_resolution_timer();
    // create object: deleted when out of scope
    fft fft_computer;
    hpx::future<void> future_initialize = fft_computer.initialize(std::move(values_vec));
    future_initialize.get();
    auto start_total = t.now();
    hpx::future<vector_2d> future_result = fft_computer.fft_2d_r2c();
    values_vec = future_result.get();
    auto stop_total = t.now();

    auto total = stop_total - start_total;
   
    if (1)
    {
        std::string msg = "\nLocality 0\nTotal runtime: {1}\n";
        hpx::util::format_to(hpx::cout, msg,  
                            total) << std::flush;
    }

    ////////////////////////////////////////////////////////////////
    // print results
    if (print_result)
    {
        for (auto r5 : values_vec)
        {
            std::string msg = "\n";
            hpx::util::format_to(hpx::cout, msg) << std::flush;
            std::size_t counter = 0;
            for (auto v : r5)
            {
                if(counter%2 == 0)
                {
                    std::string msg = "({1} ";
                    hpx::util::format_to(hpx::cout, msg, v) << std::flush;
                }
                else
                {
                    std::string msg = "{1}) ";
                    hpx::util::format_to(hpx::cout, msg, v) << std::flush;
                }
                ++counter;
            }
        }
        std::string msg2 = "\n";
        hpx::util::format_to(hpx::cout, msg2) << std::flush;
    }
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
    ("result", value<bool>()->default_value(0), "print generated results (default: false)")
    ("nx", value<std::size_t>()->default_value(8), "Total x dimension")
    ("ny", value<std::size_t>()->default_value(14), "Total y dimension")
    ("plan", value<std::string>()->default_value("estimate"), "FFTW plan (default: estimate)");

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return hpx::init(argc, argv, init_args);
}

#endif