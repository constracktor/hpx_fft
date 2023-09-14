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

using vector = std::vector<double, std::allocator<double>>;
using split_vector = std::vector<vector>;

// #define N_X 8
// #define N_Y 14 

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

    // fftw_plan fftw_create_plan(int n, fftw_direction dir,
    //                        int flags);
    ////////////////////////////////////////////////////////////////
    // initialize values
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        std::iota(values_vec[i].begin(), values_vec[i].end() - 2, 0.0);
    });
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////7
    // COMPUTATION
    auto start_total = t.now();
    /////////////////////////////////////////////////////////////////
    // FFTW 1D in y-direction 
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        //fftw_execute(plans_1D_r2c[i]);
        fftw_execute_dft_r2c(plan_1D_r2c, values_vec[i].data(), reinterpret_cast<fftw_complex*>(values_vec[i].data()));



        hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto j)
        {
            std::move(values_vec[i].begin() + j * dim_c_y_part, values_vec[i].begin() + (j+1) * dim_c_y_part, values_prep[j].begin() + i * dim_c_y_part);
        });
    });
    /////////////////////////////////////////////////////////////////
    // Communication in x-direction 
    auto start_first_comm = t.now();
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        // communicate
        if(this_locality != i)
        {
             // receive from other locality
            hpx::shared_future<vector> result = hpx::collectives::scatter_from<vector>(scatter_communicators[i], hpx::collectives::generation_arg(generation_counter));
            ///////
            // store future
            communication_tmp_futures[i] = std::move(result);
        }
        else
        {
            // send from this locality
            hpx::shared_future<vector> result2 = hpx::collectives::scatter_to(scatter_communicators[this_locality], std::move(values_prep), hpx::collectives::generation_arg(generation_counter));
            ///////
            // store future
            communication_tmp_futures[this_locality] = std::move(result2);
            // do something that num localities is 1:
        }
        // transpose
        communication_vec[i] = communication_tmp_futures[i].get();

        hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto j)
        {
            hpx::experimental::for_loop(hpx::execution::seq, 0, n_y_local, [&](auto k)
            {
                trans_values_vec[k][j* num_localities*2 + 2*i] = communication_vec[i][2*k+ j * dim_c_y_part];
                trans_values_vec[k][j* num_localities*2 + 2*i+1] = communication_vec[i][2*k+1 + j * dim_c_y_part];
            });
        });   
    });
    ++generation_counter;
    // ////////////////////////////////////////////////////////////////
    // FFTW 1D x-direction
    auto start_second_fft = t.now();
    // forward step two
    hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto i)
    {
        fftw_execute_dft(plan_1D_c2c, reinterpret_cast<fftw_complex*>(trans_values_vec[i].data()), reinterpret_cast<fftw_complex*>(trans_values_vec[i].data()));

        hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto j)
        {
            // remove from loop
            //trans_values_prep[j].resize(n_y_local * dim_c_x_part);
            std::move(trans_values_vec[i].begin() + j * dim_c_x_part, trans_values_vec[i].begin() + (j+1) * dim_c_x_part, trans_values_prep[j].begin() + i * dim_c_x_part);
        });
    });
    /////////////////////////////////////////////////////////////////
    // Communication in x-direction 
    auto start_second_comm = t.now();
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        // communicate
        if(this_locality != i)
        {
             // receive from other locality
            hpx::shared_future<vector> result = hpx::collectives::scatter_from<vector>(scatter_communicators[i], hpx::collectives::generation_arg(generation_counter));
            ///////
            // store future
            communication_tmp_futures[i] = std::move(result);
        }
        else
        {
            // send from this locality
            hpx::shared_future<vector> result2 = hpx::collectives::scatter_to(scatter_communicators[this_locality], std::move(trans_values_prep), hpx::collectives::generation_arg(generation_counter));
            ///////
            // store future
            communication_tmp_futures[this_locality] = std::move(result2);
            // do something that num localities is 1:
        }

        // transpose
        communication_vec[i] = communication_tmp_futures[i].get();

        hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto j)
        {
            hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto k)
            {
                values_vec[k][j* num_localities*2 + 2*i] = communication_vec[i][2*k + j * dim_c_x_part];
                values_vec[k][j* num_localities*2 + 2*i+1] = communication_vec[i][2*k+1 + j * dim_c_x_part];
            });
        });    
    });
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
    // // divide value vector
    // hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto i)
    // {
    //     hpx::experimental::for_loop(hpx::execution::seq, 0, num_localities, [&](auto j)
    //     {
    //         vector tmp(std::make_move_iterator(values_vec[i].begin()+j*dim_c_y_part),
    //                             std::make_move_iterator(values_vec[i].begin()+(j+1)*dim_c_y_part)); // move;
    //         values_div_vec[i][j] = std::move(tmp);
    //     });
    // });

    // // divide transposed value vector
    // hpx::experimental::for_loop(hpx::execution::seq, 0, n_y_local, [&](auto i)
    // {
    //     hpx::experimental::for_loop(hpx::execution::seq, 0, num_localities, [&](auto j)
    //     {
    //         vector tmp(std::make_move_iterator(trans_values_vec[i].begin()+j*dim_c_x_part),
    //                             std::make_move_iterator(trans_values_vec[i].begin()+(j+1)*dim_c_x_part)); // move;
    //         trans_values_div_vec[i][j] = std::move(tmp);

    //     });
    // });













    // ////////////////////////////////////////////////////////////////
    // // communication
    // hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto i)
    // {
    //     ///////
    //     // receive from other localities
    //     hpx::experimental::for_loop(hpx::execution::seq, 0, num_localities, [&](auto other_locality)
    //     {
    //         if(this_locality != other_locality)
    //         {
    //             hpx::shared_future<vector> result = hpx::collectives::scatter_from<vector>(scatter_communicators[other_locality], hpx::collectives::generation_arg(generation_counter));
    //             ///////
    //             // store future
    //             communication_tmp_futures[other_locality] = std::move(result);
    //         }
    //     });
    //     ///////
    //     // send from this locality
    //     hpx::shared_future<vector> result = hpx::collectives::scatter_to(scatter_communicators[this_locality], std::move(values_div_vec[i]), hpx::collectives::generation_arg(generation_counter));
    //     ///////
    //     // store future
    //     communication_tmp_futures[this_locality] = std::move(result);
    //     // get futures
    //     hpx::experimental::for_loop(hpx::execution::seq, 0, num_localities, [&](auto other_locality)
    //     {
    //         communication_vec[other_locality].resize(n_x_local);
    //         communication_vec[other_locality][i] = communication_tmp_futures[other_locality].get();
    //     });
    //     ++generation_counter;
    // });
    // auto start_first_trans = t.now();






































































    ////////////////////////////////////////////////////////////////
    // print result
    // sleep(this_locality);
    // if (1)
    // {
    //     std::string msg = "\nLocality {1}:";
    //     hpx::util::format_to(hpx::cout, msg, this_locality) << std::flush;
    //     for (auto r5 : trans_values_vec)
    //     {
    //         std::string msg = "\n";
    //         hpx::util::format_to(hpx::cout, msg) << std::flush;
    //         std::uint32_t counter = 0;
    //         for (auto v : r5)
    //         {
    //             if(counter%2 == 0)
    //             {
    //                 std::string msg = "({1} ";
    //                 hpx::util::format_to(hpx::cout, msg, v) << std::flush;
    //             }
    //             else
    //             {
    //                 std::string msg = "{1}) ";
    //                 hpx::util::format_to(hpx::cout, msg, v) << std::flush;
    //             }
    //             ++counter;
    //         }
    //     }
    //     std::string msg2 = "\n";
    //     hpx::util::format_to(hpx::cout, msg2) << std::flush;
    // }

    // sleep(this_locality);
    // if (1)
    // {
    //     std::string msg = "\nLocality {1}:";
    //     hpx::util::format_to(hpx::cout, msg, this_locality) << std::flush;
    //     for (auto r4 :communication_vec)
    //     {
    //         std::string msg = "\n";
    //         hpx::util::format_to(hpx::cout, msg) << std::flush;
    //         for (auto r5 : r4)
    //         {
    //             std::string msg = "\n";
    //             hpx::util::format_to(hpx::cout, msg) << std::flush;
    //             std::uint32_t counter = 0;
    //             for (auto v : r5)
    //             {
    //                 if(counter%2 == 0)
    //                 {
    //                     std::string msg = "({1} ";
    //                     hpx::util::format_to(hpx::cout, msg, v) << std::flush;
    //                 }
    //                 else
    //                 {
    //                     std::string msg = "{1}) ";
    //                     hpx::util::format_to(hpx::cout, msg, v) << std::flush;
    //                 }
    //                 ++counter;
    //             }
    //         }
    //     }
    //     std::string msg2 = "\n";
    //     hpx::util::format_to(hpx::cout, msg2) << std::flush;
    // }

    // sleep(this_locality);
    // if (1)
    // {
    //     std::string msg = "\nLocality {1}:";
    //     hpx::util::format_to(hpx::cout, msg, this_locality) << std::flush;
    //     for (auto r4 :trans_values_div_vec)
    //     {
    //         std::string msg = "\n";
    //         hpx::util::format_to(hpx::cout, msg) << std::flush;
    //         for (auto r5 : r4)
    //         {
    //             std::string msg = "\n";
    //             hpx::util::format_to(hpx::cout, msg) << std::flush;
    //             std::uint32_t counter = 0;
    //             for (auto v : r5)
    //             {
    //                 if(counter%2 == 0)
    //                 {
    //                     std::string msg = "({1} ";
    //                     hpx::util::format_to(hpx::cout, msg, v) << std::flush;
    //                 }
    //                 else
    //                 {
    //                     std::string msg = "{1}) ";
    //                     hpx::util::format_to(hpx::cout, msg, v) << std::flush;
    //                 }
    //                 ++counter;
    //             }
    //         }
    //     }
    //     std::string msg2 = "\n";
    //     hpx::util::format_to(hpx::cout, msg2) << std::flush;
    // }
