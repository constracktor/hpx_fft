
#include <hpx/config.hpp>

#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/modules/collectives.hpp>
#include <hpx/iostream.hpp>

#include <hpx/timing/high_resolution_timer.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
// FFTW
#include <fftw3.h>

using real = double;
using vector = std::vector<real, std::allocator<double>>;
using split_vector = std::vector<vector>;
using fft_backend_plan = fftw_plan;

// #define N_X 8
// #define N_Y 14 

void fft_1d_r2c_inplace(const fft_backend_plan plan, 
                        vector& input)
{
    fftw_execute_dft_r2c(plan, input.data(), reinterpret_cast<fftw_complex*>(input.data()));
}

void fft_1d_c2c_inplace(const fft_backend_plan plan, 
                        vector& input)
{
    fftw_execute_dft(plan, reinterpret_cast<fftw_complex*>(input.data()), reinterpret_cast<fftw_complex*>(input.data()));
}


void transpose(const vector& input, 
                      std::vector<vector>& output, 
                      const std::uint32_t index_trans)
{
    const std::uint32_t dim_input = input.size()/2;
    
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_input, [&](auto index)
    {
        output[index][2*index_trans] = input[2*index];
        output[index][2*index_trans + 1] = input[2*index + 1];
    });     
}

void local_transpose(hpx::shared_future<void> ready, 
                       const vector& input, 
                       std::vector<vector>& output, 
                       const std::uint32_t index_trans)
{
    ready.get();
    transpose(std::cref(input), std::ref(output), index_trans);
}



void test_multiple_use_with_generation(hpx::program_options::variables_map& vm)
{
    ////////////////////////////////////////////////////////////////
    // Parameters and Data structures
    // hpx parameters
    const std::uint32_t this_locality = hpx::get_locality_id();
    //const std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    // fft dimension parameters
    const std::uint32_t dim_c_x = vm["nx"].as<std::uint32_t>();//N_X; 
    const std::uint32_t dim_r_y = vm["ny"].as<std::uint32_t>();//N_Y;
    const std::uint32_t dim_c_y = dim_r_y / 2 + 1;
    // value vectors
    std::vector<vector> values_vec(dim_c_x);
    std::vector<vector> trans_values_vec(dim_c_y);
    // FFTW plans
    std::string plan_flag = vm["plan"].as<std::string>();
    unsigned FFTW_PLAN_FLAG = FFTW_ESTIMATE;
    if( plan_flag == "measure" )
    {
        FFTW_PLAN_FLAG = FFTW_MEASURE;
    }
    else if ( plan_flag == "patient")
    {
        FFTW_PLAN_FLAG = FFTW_PATIENT;
    }
    else if ( plan_flag == "exhaustive")
    {
        FFTW_PLAN_FLAG = FFTW_EXHAUSTIVE;
    }
    fftw_plan plan_1D_r2c;
    fftw_plan plan_1D_c2c;
    // time measurement
    auto t = hpx::chrono::high_resolution_timer();
    const std::uint32_t loop = vm["loop"].as<std::uint32_t>();//1;    // Number of loops to average
    bool print_result = vm["result"].as<bool>();

    ////////////////////////////////////////////////////////////////
    //
    hpx::experimental::for_loop(hpx::execution::seq, 0, dim_c_x, [&](auto i)
    {
        values_vec[i].resize(2*dim_c_y);
    });
    // forward step two: c2c in x-direction
    hpx::experimental::for_loop(hpx::execution::seq, 0, dim_c_y, [&](auto i)
    {
        trans_values_vec[i].resize(2*dim_c_x);
    });        

    ////////////////////////////////////////////////////////////////
    //FFTW plans
    // forward step one: r2c in y-direction
    plan_1D_r2c = fftw_plan_dft_r2c_1d(dim_r_y,
                            values_vec[0].data(),
                            reinterpret_cast<fftw_complex*>(values_vec[0].data()),
                            FFTW_PLAN_FLAG);
    // forward step two: c2c in x-direction
    plan_1D_c2c = fftw_plan_dft_1d(dim_c_x, 
                            reinterpret_cast<fftw_complex*>(trans_values_vec[0].data()), 
                            reinterpret_cast<fftw_complex*>(trans_values_vec[0].data()), 
                            FFTW_FORWARD, FFTW_PLAN_FLAG);

    ////////////////////////////////////////////////////////////////
    // initialize values
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_x, [&](auto i)
    {
        std::iota(values_vec[i].begin(), values_vec[i].end() - 2, 0.0);
    });
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////7
    // COMPUTATION
    std::vector<hpx::shared_future<void>> r2c_futures(dim_c_x);
    std::vector<hpx::shared_future<void>> trans_x_futures(dim_c_x);
    std::vector<hpx::shared_future<void>> c2c_futures(dim_c_y);
    std::vector<hpx::shared_future<void>> trans_y_futures(dim_c_y);
    ////////////////////////////////////////////////////////////////
    auto start_total = t.now();
    /////////////////////////////////////////////////////////////////
    hpx::experimental::for_loop(hpx::execution::seq, 0, dim_c_x, [&](auto i)
    {
        // FFTW 1D in y-direction 
        r2c_futures[i] = hpx::async(&fft_1d_r2c_inplace, plan_1D_r2c, std::ref(values_vec[i]));
        // Local tranpose in x-direction
        trans_x_futures[i] = hpx::async(&local_transpose, r2c_futures[i], std::cref(values_vec[i]), std::ref(trans_values_vec), i);
    });
    hpx::wait_all(trans_x_futures);
    auto start_second_fft = t.now();
    // forward step two
    hpx::experimental::for_loop(hpx::execution::seq, 0, dim_c_y, [&](auto i)
    {
        // FFTW 1D x-direction
        c2c_futures[i] = hpx::async(&fft_1d_c2c_inplace, plan_1D_c2c, std::ref(trans_values_vec[i]));
        // Local tranpose in y-direction
        trans_y_futures[i] = hpx::async(&local_transpose, c2c_futures[i], std::cref(trans_values_vec[i]), std::ref(values_vec),i);
    });
    hpx::wait_all(trans_y_futures);
    ////////////////////////////////////////////////////////////////
    auto stop_total = t.now();
    auto total = stop_total - start_total;
    auto first_fftw = start_second_fft - start_total;
    auto second_fftw = stop_total - start_second_fft;

    // print result    
    if (1)//this_locality==0)
    {
        std::string msg = "\nLocality {1}:\nTotal runtime: {2}\nFFTW r2c     : {3}\nFFTW c2c     : {4}\n";
        hpx::util::format_to(hpx::cout, msg, 
                            this_locality, 
                            total,
                            first_fftw,
                            second_fftw) << std::flush;
    }
    if (print_result)
    {
        sleep(this_locality);
        std::string msg = "\nLocality {1}\n";
        hpx::util::format_to(hpx::cout, msg, 
                        this_locality) << std::flush;
        for (auto r5 : values_vec)
        {
            std::string msg = "\n";
            hpx::util::format_to(hpx::cout, msg) << std::flush;
            std::uint32_t counter = 0;
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






    ////////////////////////////////////////////////////////////////
    // TODO Backwards transform
    
    // sleep(this_locality+1);
    // auto t2 = t.now();
    // auto t3 = t2 - t1;
    //std::string msg2 = "\n{1} - {2} = {3}\n";
    // std::string msg2 = "\n{1} - {2} = {3}\n";
    // hpx::util::format_to(hpx::cout, msg2,t2,t1,t3) << std::flush;

    ////////////////////////////////////////////////
    // Cleanup
    // FFTW cleanup
    fftw_destroy_plan(plan_1D_r2c);
    // forward step two: c2c in x-direction
    fftw_destroy_plan(plan_1D_c2c);
    fftw_cleanup();
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    test_multiple_use_with_generation(vm);

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
    ("result", value<bool>()->default_value(0), "print generated results (default: false)")
    ("nx", value<std::uint32_t>()->default_value(8), "Total x dimension")
    ("ny", value<std::uint32_t>()->default_value(14), "Total y dimension")
    ("plan", value<std::string>()->default_value("estimate"), "FFTW plan (default: estimate)")
    ("loop",value<std::uint32_t>()->default_value(1), "Total x dimension");

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return hpx::init(argc, argv, init_args);
}

#endif