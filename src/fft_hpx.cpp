//  Copyright (c) 2020-2022 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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

void split(const vector& input, 
           split_vector& output, 
           const std::uint32_t num_localities, 
           const std::uint32_t offset)
{
    const std::uint32_t part_size = input.size() / num_localities;
    for (std::uint32_t j = 0; j < num_localities; ++j) 
    {
        std::move(input.begin() + j * part_size, input.begin() + (j+1) * part_size, output[j].begin() + offset * part_size);
    }
}

void split_vector_comm(hpx::shared_future<void> ready, 
                       const vector& input, 
                       split_vector& output, 
                       const std::uint32_t num_localities, 
                       const std::uint32_t offset)
{
    ready.get();
    split(std::cref(input), std::ref(output), num_localities, offset);
}


std::vector<hpx::shared_future<vector>> communicate_scatter(
                         std::vector<hpx::shared_future<void>>& ready,
                         std::vector<hpx::collectives::communicator> communicators,
                         split_vector& input, 
                         const std::uint32_t num_localities,
                         std::uint32_t& generation_counter)
{
    const std::uint32_t this_locality = hpx::get_locality_id();
    std::vector<hpx::shared_future<vector>> communication_futures(num_localities);
    hpx::wait_all(ready);

    for (std::uint32_t i = 0; i < num_localities; ++i) 
    {  
        if(this_locality != i)
        {
            // receive from other locality
            communication_futures[i] = hpx::collectives::scatter_from<vector>(communicators[i], hpx::collectives::generation_arg(generation_counter));
        }
        else
        {
            // send from this locality
            communication_futures[this_locality] = hpx::collectives::scatter_to(communicators[this_locality], std::move(input), hpx::collectives::generation_arg(generation_counter));
        }
    }
    ++generation_counter;
    
    return std::move(communication_futures);
}

hpx::shared_future<split_vector> communicate_all_to_all(
                         std::vector<hpx::shared_future<void>>& ready,
                         hpx::collectives::communicator communicator,
                         split_vector& input,
                         std::uint32_t& generation_counter)
{
    const std::uint32_t this_locality = hpx::get_locality_id();
    hpx::shared_future<split_vector> communication_future;
    hpx::wait_all(ready);

    communication_future = hpx::collectives::all_to_all(communicator, std::move(input), hpx::collectives::generation_arg(generation_counter));
    ++generation_counter;
    
    return std::move(communication_future);
}


void local_transpose(const vector& input, 
                      std::vector<vector>& output, 
                      const std::uint32_t part_size, 
                      const std::uint32_t offset, 
                      const std::uint32_t num_loc)
{
    std::uint32_t index;
    std::uint32_t index_trans;
    const std::uint32_t dim_output = output.size();
    const std::uint32_t dim_input = input.size() / part_size;

    
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_input, [&](auto j)
    {
        hpx::experimental::for_loop(hpx::execution::seq, 0, dim_output, [&](auto k)
        {
            index = 2 * k + j * part_size;
            index_trans = 2 * (j * num_loc + offset);

            output[k][index_trans] = input[index];
            output[k][index_trans + 1] = input[index + 1];
        });
    });     
}

void transpose(const vector& input, 
               vector& output, 
               const std::uint32_t factor_in, 
               const std::uint32_t factor_out, 
               const std::uint32_t offset_in, 
               const std::uint32_t offset_out)
{
    std::uint32_t index_in;
    std::uint32_t index_out;
    const std::uint32_t dim_input = input.size() / factor_in;
    for(std::uint32_t j = 0; j < dim_input; ++j)
    {
        // compute indices once use twice
        index_in = factor_in * j + offset_in;
        index_out = factor_out * j + offset_out;
        // transpose
        output[index_out]     = input[index_in];
        output[index_out + 1] = input[index_in + 1];
    }
}




void test_multiple_use_with_generation(hpx::program_options::variables_map& vm)
{
    ////////////////////////////////////////////////////////////////
    // Parameters and Data structures
    // hpx parameters
    const std::uint32_t this_locality = hpx::get_locality_id();
    const std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::uint32_t generation_counter = 1;
    // fft dimension parameters
    const std::uint32_t dim_c_x = vm["nx"].as<std::uint32_t>();//N_X; 
    const std::uint32_t dim_r_y = vm["ny"].as<std::uint32_t>();//N_Y;
    const std::uint32_t dim_c_y = dim_r_y / 2 + 1;
    const std::uint32_t dim_c_y_part = 2 * dim_c_y / num_localities;
    const std::uint32_t dim_c_x_part = 2 * dim_c_x / num_localities;
    // division parameters
    const std::uint32_t n_x_local = dim_c_x / num_localities;
    const std::uint32_t n_y_local = dim_c_y / num_localities;
    // communicators
    std::vector<const char*> scatter_basenames(num_localities);
    std::vector<hpx::collectives::communicator> scatter_communicators(num_localities);
    // value vectors
    std::vector<vector> values_vec(n_x_local);
    std::vector<vector> trans_values_vec(n_y_local);
    split_vector communication_vec(num_localities);
    split_vector values_prep(num_localities);
    split_vector trans_values_prep(num_localities);
    // holder for communication futures
    std::vector<hpx::shared_future<vector>> communication_tmp_futures(num_localities);
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
    // setup communicators
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        scatter_basenames[i] = std::move(std::to_string(i).c_str());
        scatter_communicators[i] = std::move(hpx::collectives::create_communicator(scatter_basenames[i],
            hpx::collectives::num_sites_arg(num_localities), hpx::collectives::this_site_arg(this_locality)));
    });
    ////////////////////////////////////////////////////////////////
    //
    hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto i)
    {
        values_vec[i].resize(2*dim_c_y);
    });
    // forward step two: c2c in x-direction
    hpx::experimental::for_loop(hpx::execution::seq, 0, n_y_local, [&](auto i)
    {
        trans_values_vec[i].resize(2*dim_c_x);
    });        
    hpx::experimental::for_loop(hpx::execution::seq, 0, num_localities, [&](auto j)
    {
        values_prep[j].resize(n_x_local * dim_c_y_part);
        trans_values_prep[j].resize(n_y_local * dim_c_x_part);
        if (n_y_local * dim_c_x_part > n_x_local * dim_c_y_part)
        {
            communication_vec[j].resize(n_y_local * dim_c_x_part);
        }
        else
        {
            communication_vec[j].resize(n_x_local * dim_c_y_part);
        }
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
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        std::iota(values_vec[i].begin(), values_vec[i].end() - 2, 0.0);
    });
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////7
    // COMPUTATION
    std::vector<hpx::shared_future<void>> r2c_futures(n_x_local);
    std::vector<hpx::shared_future<void>> split_x_futures(n_x_local);
    std::vector<hpx::shared_future<void>> c2c_futures(n_x_local);
    std::vector<hpx::shared_future<void>> split_y_futures(n_y_local);
    std::vector<hpx::shared_future<void>> trans_futures(num_localities);
    
    std::vector<std::vector<hpx::shared_future<void>>> futures(n_y_local);
    std::vector<std::vector<hpx::shared_future<void>>> futures2(n_x_local);
    hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto k)
    {
        futures[k].resize(num_localities);
    });
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto k)
    {
        futures2[k].resize(num_localities);
    });

    auto start_total = t.now();
    /////////////////////////////////////////////////////////////////
    // FFTW 1D in y-direction 
    for(std::uint32_t i = 0; i < n_x_local; ++i)
    {
        r2c_futures[i] = hpx::async(&fft_1d_r2c_inplace, plan_1D_r2c, std::ref(values_vec[i]));
        split_x_futures[i] = hpx::async(&split_vector_comm, r2c_futures[i], std::cref(values_vec[i]), std::ref(values_prep), num_localities, i);
    };
    hpx::wait_all(split_x_futures);
    /////////////////////////////////////////////////////////////////
    // Communication in x-direction 
    auto start_first_comm = t.now();                                       

    ////////////////////////////////
    // all to all
    // auto futures = communicate_all_to_all(
    //                     std::ref(split_x_futures),
    //                     scatter_communicators[0],
    //                     std::ref(values_prep), 
    //                     generation_counter);
    // communication_vec = futures.get();
    ////////////////////////////////

 
    communication_tmp_futures = communicate_scatter(std::ref(split_x_futures),
                                                    scatter_communicators,
                                                    std::ref(values_prep), 
                                                    num_localities,
                                                    generation_counter);

    //////////////////////////////////////////////////////////////////
    // Local tranpose in x-direction
    std::uint32_t factor_in = dim_c_y_part;
    std::uint32_t factor_out = 2 * num_localities;
    std::uint32_t offset_in, offset_out;
    for(std::uint32_t k = 0; k < n_y_local; ++k)
    {
        vector& output = trans_values_vec[k];
        offset_in = 2 * k;

        for(std::uint32_t i = 0; i < num_localities; ++i)
        {
            const vector& input = communication_tmp_futures[i].get();
            offset_out = 2 * i;

            futures[k][i] = hpx::async(&transpose,std::cref(input), std::ref(output), factor_in, factor_out, offset_in, offset_out);
        };
    };
    
    // // ////////////////////////////////////////////////////////////////
    // // FFTW 1D x-direction
    auto start_second_fft = t.now();
    // forward step two
    for(std::uint32_t i = 0; i < n_y_local; ++i)
    {
        hpx:wait_all(futures[i]);
        c2c_futures[i] = hpx::async(&fft_1d_c2c_inplace, plan_1D_c2c, std::ref(trans_values_vec[i]));
        split_y_futures[i] = hpx::async(&split_vector_comm, c2c_futures[i], std::cref(trans_values_vec[i]), std::ref(trans_values_prep), num_localities, i);
    };
    hpx::wait_all(split_y_futures);
    /////////////////////////////////////////////////////////////////
    // Communication in y-direction 
    auto start_second_comm = t.now();


    ////////////////////////////////
    // all to all
    // auto futures1 = communicate_all_to_all(
    //                     std::ref(split_y_futures),
    //                     scatter_communicators[0],
    //                     std::ref(trans_values_prep), 
    //                     generation_counter);
    // communication_vec = futures1.get();
    ////////////////////////////////

    communication_tmp_futures = communicate_scatter(std::ref(split_y_futures),
                         scatter_communicators,
                         std::ref(trans_values_prep), 
                         num_localities,
                         generation_counter);

    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        // transpose
        communication_vec[i] = communication_tmp_futures[i].get();
    });

    //////////////////////////////////////////////////////////////////
    // Local tranpose in x-direction
    factor_in = dim_c_x_part;
    //factor_out = 2 * num_localities;
    
    for(std::uint32_t k = 0; k < n_x_local; ++k)
    {
        vector& output = values_vec[k];
        offset_in = 2 * k;

        for(std::uint32_t i = 0; i < num_localities; ++i)
        {
            const vector& input = communication_tmp_futures[i].get();
            offset_out = 2 * i;

            futures2[k][i] = hpx::async(&transpose,std::cref(input), std::ref(output), factor_in, factor_out, offset_in, offset_out);
        };
    };

    // hpx::experimental::for_loop(hpx::execution::seq, 0, num_localities, [&](auto i)
    // {
    //     trans_futures[i] = hpx::dataflow(hpx::unwrapping(&local_transpose), communication_tmp_futures[i], std::ref(values_vec), dim_c_x_part, i , num_localities);
    // });
    // hpx::wait_all(trans_futures);

    // hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    // {
    //     // transpose
    //     //communication_vec[i] = communication_tmp_futures[i].get();
    //     hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto j)
    //     {
    //         hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto k)
    //         {
    //             values_vec[k][j* num_localities*2 + 2*i] = communication_vec[i][2*k + j * dim_c_x_part];
    //             values_vec[k][j* num_localities*2 + 2*i+1] = communication_vec[i][2*k+1 + j * dim_c_x_part];
    //         });
    //     });    
    // });
    // wait till finished
    for(std::uint32_t i = 0; i < n_x_local; ++i)
    {
        hpx:wait_all(futures2[i]);
    };






    ////////////////////////////////////////////////////////////////
    auto stop_total = t.now();
    auto total = stop_total - start_total;
    auto first_fftw = start_first_comm - start_total;
    auto first_comm = start_second_fft - start_first_comm;

    auto second_fftw = start_second_comm - start_second_fft;
    auto second_comm = stop_total - start_second_comm;
    // print result    
    if (1)//this_locality==0)
    {
        std::string msg = "\nLocality {1}:\nTotal runtime: {2}\nFFTW r2c     : {3}\nFirst comm   : {4}\nFFTW c2c     : {5}\nSecond comm  : {6}\n";
        hpx::util::format_to(hpx::cout, msg, 
                            this_locality, 
                            total,
                            first_fftw,
                            first_comm,
                            second_fftw,
                            second_comm) << std::flush;
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

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {"hpx.run_hpx_main!=1"};

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    return hpx::init(argc, argv, init_args);
}

#endif

    // auto start_second_comm = t.now();
    // hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    // {
    //     // transpose
    //     //communication_vec[i] = communication_tmp_futures[i].get();
    //     hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto j)
    //     {
    //         hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto k)
    //         {
    //             values_vec[k][j* num_localities*2 + 2*i] = communication_vec[i][2*k + j * dim_c_x_part];
    //             values_vec[k][j* num_localities*2 + 2*i+1] = communication_vec[i][2*k+1 + j * dim_c_x_part];
    //         });
    //     });    
    // });