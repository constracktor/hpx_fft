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
#include <hpx/modules/testing.hpp>

#include <hpx/timing/high_resolution_timer.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
// FFTW
#include <fftw3.h>

using real = double;
using vector_1d = std::vector<real, std::allocator<double>>;
using vector_2d = std::vector<vector_1d>;
using fft_backend_plan = fftw_plan;

// #define N_X 8
// #define N_Y 14 
///////////////////////////////////////////////////////////////////////////////////////////////////
// FFT functions
void fft_1d_r2c_inplace(const fft_backend_plan plan, 
                        vector_1d& input)
{
    fftw_execute_dft_r2c(plan, input.data(), reinterpret_cast<fftw_complex*>(input.data()));
}

void fft_1d_c2c_inplace(const fft_backend_plan plan, 
                        vector_1d& input)
{
    fftw_execute_dft(plan, reinterpret_cast<fftw_complex*>(input.data()), reinterpret_cast<fftw_complex*>(input.data()));
}

void split_vector(const vector_1d& input, 
                 vector_2d& output, 
                  const std::uint32_t num_localities, 
                  const std::uint32_t offset)
{
    const std::uint32_t part_size = input.size() / num_localities;
    for (std::uint32_t j = 0; j < num_localities; ++j) 
    {
        std::move(input.begin() + j * part_size, input.begin() + (j+1) * part_size, output[j].begin() + offset * part_size);
    }

}

void transpose(const vector_1d& input, 
               vector_1d& output, 
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

void transpose_shared(const vector_1d& input, 
                      vector_2d& output, 
                      const std::uint32_t index_trans)
{
    const std::uint32_t dim_input = input.size() / 2;
    for( std::uint32_t index = 0; index < dim_input; ++index)
    {
        output[index][2 * index_trans] = input[2 * index];
        output[index][2 * index_trans + 1] = input[2 * index + 1];
    }     
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// HPX communication functions
std::vector<hpx::shared_future<vector_1d>> communicate_scatter(
                         std::vector<hpx::collectives::communicator>& communicators,
                         vector_2d& input, 
                         const std::uint32_t num_localities,
                         std::uint32_t& generation_counter)
{
    const std::uint32_t this_locality = hpx::get_locality_id();
    std::vector<hpx::shared_future<vector_1d>> communication_futures(num_localities);
    // scatter communication
    for (std::uint32_t i = 0; i < num_localities; ++i) 
    {  
        if(this_locality != i)
        {
            // receive from other locality
            communication_futures[i] = hpx::collectives::scatter_from<vector_1d>(communicators[i], hpx::collectives::generation_arg(generation_counter));
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

hpx::shared_future<vector_2d> communicate_all_to_all(
                         std::vector<hpx::shared_future<void>>& ready,
                         hpx::collectives::communicator communicator,
                         vector_2d& input,
                         std::uint32_t& generation_counter)
{  
    hpx::shared_future<vector_2d> communication_future;
    hpx::wait_all(ready);

    communication_future = hpx::collectives::all_to_all(communicator, std::move(input), hpx::collectives::generation_arg(generation_counter));
    ++generation_counter;
    
    return std::move(communication_future);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 2d FFT functions
// synchronized
void fft_2d_all_to_all(vector_2d& values_vec, const unsigned PLAN_FLAG)
{
    ////////////////////////////////////////////////////////////////
    // Parameters and Data structures
    // hpx parameters
    const std::uint32_t this_locality = hpx::get_locality_id();
    const std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::uint32_t generation_counter = 1;
    // fft dimension parameters
    const std::uint32_t n_x_local = values_vec.size();
    const std::uint32_t dim_c_x = n_x_local * num_localities;
    const std::uint32_t dim_c_y = values_vec[0].size() / 2;
    const std::uint32_t dim_r_y = 2 * dim_c_y - 2;
    const std::uint32_t n_y_local = dim_c_y / num_localities;
    const std::uint32_t dim_c_y_part = 2 * dim_c_y / num_localities;
    const std::uint32_t dim_c_x_part = 2 * dim_c_x / num_localities;
    // transpose parameters
    std::uint32_t factor_out = 2 * num_localities;
    std::uint32_t factor_in;
    // communicator
    const char* all_to_all_basename = "0";
    hpx::collectives::communicator all_to_all_communicator;
    // value vectors
    vector_2d trans_values_vec(n_y_local);
    vector_2d values_prep(num_localities);
    vector_2d trans_values_prep(num_localities);
    vector_2d communication_vec(num_localities);
    // holder for communication futures
    hpx::shared_future<vector_2d> communication_future;
    // FFTW plans
    fft_backend_plan plan_1d_r2c;
    fft_backend_plan plan_1d_c2c;
    // time measurement
    auto t = hpx::chrono::high_resolution_timer();
    ////////////////////////////////////////////////////////////////
    // setup communicator
        all_to_all_communicator = hpx::collectives::create_communicator(all_to_all_basename,
            hpx::collectives::num_sites_arg(num_localities), hpx::collectives::this_site_arg(this_locality));
    ////////////////////////////////////////////////////////////////
    // allocate data
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
    plan_1d_r2c = fftw_plan_dft_r2c_1d(dim_r_y,
                                       values_vec[0].data(),
                                       reinterpret_cast<fftw_complex*>(values_vec[0].data()),
                                       PLAN_FLAG);
    // forward step two: c2c in x-direction
    plan_1d_c2c = fftw_plan_dft_1d(dim_c_x, 
                                   reinterpret_cast<fftw_complex*>(trans_values_vec[0].data()), 
                                   reinterpret_cast<fftw_complex*>(trans_values_vec[0].data()), 
                                   FFTW_FORWARD,
                                   PLAN_FLAG);
    /////////////////////////////////////////////////////////////////
    // COMPUTATION
    auto start_total = t.now();
    ////////////////////////////////
    // FFTW 1d in y-direction 
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        fft_1d_r2c_inplace(plan_1d_r2c, std::ref(values_vec[i]));
        split_vector(std::cref(values_vec[i]),std::ref(values_prep), num_localities, i);
    });
    ////////////////////////////////
    // Communication in x-direction                                        
    auto start_first_comm = t.now();
    // all to all communication
    communication_future = hpx::collectives::all_to_all(all_to_all_communicator, std::move(values_prep), hpx::collectives::generation_arg(generation_counter));
    ++generation_counter;
    communication_vec = communication_future.get();
    /////////////////////////////////
    // Local tranpose in x-direction
    auto start_first_trans = t.now();
    factor_in = dim_c_y_part;
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto j)
        {
            hpx::experimental::for_loop(hpx::execution::seq, 0, n_y_local, [&](auto k)
            {
                trans_values_vec[k][factor_out * j + 2 * i] = communication_vec[i][factor_in * j + 2 * k];
                trans_values_vec[k][factor_out * j + 2 * i + 1] = communication_vec[i][factor_in * j + 2 * k + 1];
            });
        });    
    });
    /////////////////////////////////
    // FFTW 1d x-direction
     auto start_second_fft = t.now();
    hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto i)
    {
        fft_1d_c2c_inplace(plan_1d_c2c, std::ref(trans_values_vec[i]));
        split_vector(std::cref(trans_values_vec[i]),std::ref(trans_values_prep), num_localities, i);
    });
    //////////////////////////////////
    // Communication in y-direction 
    auto start_second_comm = t.now();
    // all to all communication
    communication_future = hpx::collectives::all_to_all(all_to_all_communicator, std::move(trans_values_prep), hpx::collectives::generation_arg(generation_counter));
    ++generation_counter;
    communication_vec = communication_future.get();
    //////////////////////////////////////////////////////////////////
    // Local tranpose in x-direction
    auto start_second_trans = t.now();
    factor_in = dim_c_x_part;
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto j)
        {
            hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto k)
            {
                values_vec[k][factor_out * j + 2 * i] = communication_vec[i][factor_in * j + 2 * k];
                values_vec[k][factor_out * j + 2 * i + 1] = communication_vec[i][factor_in * j + 2 * k + 1];
            });
        });    
    });
    ////////////////////////////////////////////////////////////////
    auto stop_total = t.now();
    auto total = stop_total - start_total;
    auto first_fftw = start_first_comm - start_total;
    auto first_comm = start_first_trans - start_first_comm;
    auto first_trans= start_second_fft - start_first_trans;
    auto second_fftw = start_second_comm - start_second_fft;
    auto second_comm = start_second_trans - start_second_comm;
    auto second_trans= stop_total - start_second_trans;
    // print result    
    if (this_locality==0)
    {
        std::string msg = "\nLocality {1}:\nTotal runtime: {2}\nFFTW r2c     : {3}\nFirst comm   : {4}\nFirst trans  : {5}\nFFTW c2c     : {6}\nSecond comm  : {7}\nSecond trans : {8}\n";
        hpx::util::format_to(hpx::cout, msg, 
                            this_locality, 
                            total,
                            first_fftw,
                            first_comm,
                            first_trans,
                            second_fftw,
                            second_comm,
                            second_trans) << std::flush;
    }
    ////////////////////////////////////////////////
    // Cleanup
    // FFTW cleanup
    fftw_destroy_plan(plan_1d_r2c);
    fftw_destroy_plan(plan_1d_c2c);
    fftw_cleanup();
}

void fft_2d_scatter(vector_2d& values_vec, const unsigned PLAN_FLAG)
{
    ////////////////////////////////////////////////////////////////
    // Parameters and Data structures
    // hpx parameters
    const std::uint32_t this_locality = hpx::get_locality_id();
    const std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::uint32_t generation_counter = 1;
    // fft dimension parameters
    const std::uint32_t n_x_local = values_vec.size();
    const std::uint32_t dim_c_x = n_x_local * num_localities;
    const std::uint32_t dim_c_y = values_vec[0].size() / 2;
    const std::uint32_t dim_r_y = 2 * dim_c_y - 2;
    const std::uint32_t n_y_local = dim_c_y / num_localities;
    const std::uint32_t dim_c_y_part = 2 * dim_c_y / num_localities;
    const std::uint32_t dim_c_x_part = 2 * dim_c_x / num_localities;
    // transpose parameters
    std::uint32_t factor_out = 2 * num_localities;
    std::uint32_t factor_in;
    // communicators
    std::vector<const char*> scatter_basenames(num_localities);
    std::vector<hpx::collectives::communicator> scatter_communicators(num_localities);
    // value vectors
    vector_2d trans_values_vec(n_y_local);
    vector_2d values_prep(num_localities);
    vector_2d trans_values_prep(num_localities);
    vector_2d communication_vec(num_localities);
    // void futures
    std::vector<hpx::shared_future<void>> r2c_futures(n_x_local);
    std::vector<hpx::shared_future<void>> split_x_futures(n_x_local);
    std::vector<std::vector<hpx::shared_future<void>>> comm_x_futures(n_y_local);
    std::vector<hpx::shared_future<void>> c2c_futures(n_x_local);
    std::vector<hpx::shared_future<void>> split_y_futures(n_y_local);
    std::vector<std::vector<hpx::shared_future<void>>> comm_y_futures(n_x_local);
    // holder for communication futures
    std::vector<hpx::shared_future<vector_1d>> communication_futures(num_localities);
    // FFTW plans
    fft_backend_plan plan_1d_r2c;
    fft_backend_plan plan_1d_c2c;
    // time measurement
    auto t = hpx::chrono::high_resolution_timer();
    ////////////////////////////////////////////////////////////////
    // setup communicators
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        scatter_basenames[i] = std::move(std::to_string(i).c_str());
        scatter_communicators[i] = std::move(hpx::collectives::create_communicator(scatter_basenames[i],
            hpx::collectives::num_sites_arg(num_localities), hpx::collectives::this_site_arg(this_locality)));
    });
    ////////////////////////////////////////////////////////////////
    // allocate data
    hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto i)
    {
        comm_y_futures[i].resize(num_localities);
    });
    // forward step two: c2c in x-direction
    hpx::experimental::for_loop(hpx::execution::seq, 0, n_y_local, [&](auto i)
    {
        trans_values_vec[i].resize(2*dim_c_x);
        comm_x_futures[i].resize(num_localities);
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
    plan_1d_r2c = fftw_plan_dft_r2c_1d(dim_r_y,
                                       values_vec[0].data(),
                                       reinterpret_cast<fftw_complex*>(values_vec[0].data()),
                                       PLAN_FLAG);
    // forward step two: c2c in x-direction
    plan_1d_c2c = fftw_plan_dft_1d(dim_c_x, 
                                   reinterpret_cast<fftw_complex*>(trans_values_vec[0].data()), 
                                   reinterpret_cast<fftw_complex*>(trans_values_vec[0].data()), 
                                   FFTW_FORWARD,
                                   PLAN_FLAG);
    /////////////////////////////////////////////////////////////////
    // COMPUTATION
    auto start_total = t.now();
    ////////////////////////////////
    // FFTW 1d in y-direction 
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        fft_1d_r2c_inplace(plan_1d_r2c, std::ref(values_vec[i]));
        split_vector(std::cref(values_vec[i]),std::ref(values_prep), num_localities, i);
    });
    ////////////////////////////////
    // Communication in x-direction                                        
    auto start_first_comm = t.now();
    // scatter communication
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        if(this_locality != i)
        {
            // receive from other locality
            communication_futures[i] = hpx::collectives::scatter_from<vector_1d>(scatter_communicators[i], hpx::collectives::generation_arg(generation_counter));
        }
        else
        {
            // send from this locality
            communication_futures[this_locality] = hpx::collectives::scatter_to(scatter_communicators[this_locality], std::move(values_prep), hpx::collectives::generation_arg(generation_counter));
        }
        communication_vec[i] = communication_futures[i].get();
    });
    ++generation_counter;
    /////////////////////////////////
    // Local tranpose in x-direction
    auto start_first_trans = t.now();
    factor_in = dim_c_y_part;
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto j)
        {
            hpx::experimental::for_loop(hpx::execution::seq, 0, n_y_local, [&](auto k)
            {
                trans_values_vec[k][factor_out * j + 2 * i] = communication_vec[i][factor_in * j + 2 * k];
                trans_values_vec[k][factor_out * j + 2 * i + 1] = communication_vec[i][factor_in * j + 2 * k + 1];
            });
        });    
    });
    /////////////////////////////////
    // FFTW 1d x-direction
     auto start_second_fft = t.now();
    hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto i)
    {
        fft_1d_c2c_inplace(plan_1d_c2c, std::ref(trans_values_vec[i]));
        split_vector(std::cref(trans_values_vec[i]),std::ref(trans_values_prep), num_localities, i);
    });
    //////////////////////////////////
    // Communication in y-direction 
     auto start_second_comm = t.now();
    // scatter communication
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        if(this_locality != i)
        {
            // receive from other locality
            communication_futures[i] = hpx::collectives::scatter_from<vector_1d>(scatter_communicators[i], hpx::collectives::generation_arg(generation_counter));
        }
        else
        {
            // send from this locality
            communication_futures[this_locality] = hpx::collectives::scatter_to(scatter_communicators[this_locality], std::move(trans_values_prep), hpx::collectives::generation_arg(generation_counter));
        }
        communication_vec[i] = communication_futures[i].get();
    });
    ++generation_counter;
    //////////////////////////////////////////////////////////////////
    // Local tranpose in x-direction
    auto start_second_trans = t.now();
    factor_in = dim_c_x_part;
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto j)
        {
            hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto k)
            {
                values_vec[k][factor_out * j + 2 * i] = communication_vec[i][factor_in * j + 2 * k];
                values_vec[k][factor_out * j + 2 * i + 1] = communication_vec[i][factor_in * j + 2 * k + 1];
            });
        });    
    });
    ////////////////////////////////////////////////////////////////
    auto stop_total = t.now();
    auto total = stop_total - start_total;
    auto first_fftw = start_first_comm - start_total;
    auto first_comm = start_first_trans - start_first_comm;
    auto first_trans= start_second_fft - start_first_trans;
    auto second_fftw = start_second_comm - start_second_fft;
    auto second_comm = start_second_trans - start_second_comm;
    auto second_trans= stop_total - start_second_trans;
    // print result    
    if (this_locality==0)
    {
        std::string msg = "\nLocality {1}:\nTotal runtime: {2}\nFFTW r2c     : {3}\nFirst comm   : {4}\nFirst trans  : {5}\nFFTW c2c     : {6}\nSecond comm  : {7}\nSecond trans : {8}\n";
        hpx::util::format_to(hpx::cout, msg, 
                            this_locality, 
                            total,
                            first_fftw,
                            first_comm,
                            first_trans,
                            second_fftw,
                            second_comm,
                            second_trans) << std::flush;
    }
    ////////////////////////////////////////////////
    // Cleanup
    // FFTW cleanup
    fftw_destroy_plan(plan_1d_r2c);
    fftw_destroy_plan(plan_1d_c2c);
    fftw_cleanup();
}

void fft_2d_shared(vector_2d& values_vec, const unsigned PLAN_FLAG)
{
    // test if one locality
    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    HPX_TEST_LTE(std::uint32_t(1), num_localities);
    // fft dimension parameters
    const std::uint32_t dim_c_x = values_vec.size();
    const std::uint32_t dim_c_y = values_vec[0].size() / 2;
    const std::uint32_t dim_r_y = 2 * dim_c_y - 2;
    // value vectors
    vector_2d trans_values_vec(dim_c_y);
    // FFTW plans
    fftw_plan plan_1d_r2c;
    fftw_plan plan_1d_c2c;
    // time measurement
    auto t = hpx::chrono::high_resolution_timer();
    ////////////////////////////////////////////////////////////////
    //
    // forward step two: c2c in x-direction
    hpx::experimental::for_loop(hpx::execution::seq, 0, dim_c_y, [&](auto i)
    {
        trans_values_vec[i].resize(2*dim_c_x);
    });        

    ////////////////////////////////////////////////////////////////
    //FFTW plans
    // forward step one: r2c in y-direction
    plan_1d_r2c = fftw_plan_dft_r2c_1d(dim_r_y,
                            values_vec[0].data(),
                            reinterpret_cast<fftw_complex*>(values_vec[0].data()),
                            PLAN_FLAG);
    // forward step two: c2c in x-direction
    plan_1d_c2c = fftw_plan_dft_1d(dim_c_x, 
                            reinterpret_cast<fftw_complex*>(trans_values_vec[0].data()), 
                            reinterpret_cast<fftw_complex*>(trans_values_vec[0].data()), 
                            FFTW_FORWARD, PLAN_FLAG);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////7
    // COMPUTATION
    auto start_total = t.now();
    /////////////////////////////////////////////////////////////////
    // FFTW 1D in y-direction 
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_x, [&](auto i)
    {
        fft_1d_r2c_inplace(plan_1d_r2c, std::ref(values_vec[i]));
    });
    auto start_first_trans = t.now();
    // Local tranpose in x-direction
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_x, [&](auto i)
    {
        transpose_shared(std::cref(values_vec[i]), std::ref(trans_values_vec), i);
    });
    auto start_second_fft = t.now();
     // FFTW 1D x-direction
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_y, [&](auto i)
    {
        fft_1d_c2c_inplace(plan_1d_c2c, std::ref(trans_values_vec[i]));
    });
    auto start_second_trans = t.now();
    // Local tranpose in y-direction
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_y, [&](auto i)
    {
        transpose_shared(std::cref(trans_values_vec[i]), std::ref(values_vec), i);
    });
    ////////////////////////////////////////////////////////////////
    auto stop_total = t.now();
    auto total = stop_total - start_total;
    auto first_fftw = start_first_trans - start_total;
    auto first_trans = start_second_fft - start_first_trans;
    auto second_fftw = start_second_trans - start_second_fft;
    auto second_trans = stop_total - start_second_trans;
    // print result    
    if (1)
    {
        const std::uint32_t this_locality = hpx::get_locality_id();
        std::string msg = "\nLocality {1}:\nTotal runtime: {2}\nFFTW r2c     : {3}\nFirst trans  : {4}\nFFTW c2c     : {5}\nSecond trans : {6}\n";
        hpx::util::format_to(hpx::cout, msg, 
                            this_locality, 
                            total,
                            first_fftw,
                            first_trans,
                            second_fftw,
                            second_trans) << std::flush;
    }
}

int hpx_main(hpx::program_options::variables_map& vm)
{
     ////////////////////////////////////////////////////////////////
    // Parameters and Data structures
    // hpx parameters
    const std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    //std::uint32_t generation_counter = 1;
    // fft dimension parameters
    const std::uint32_t dim_c_x = vm["nx"].as<std::uint32_t>();//N_X; 
    const std::uint32_t dim_r_y = vm["ny"].as<std::uint32_t>();//N_Y;
    const std::uint32_t dim_c_y = dim_r_y / 2 + 1;
    // division parameters
    const std::uint32_t n_x_local = dim_c_x / num_localities;
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
    hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto i)
    {
        values_vec[i].resize(2*dim_c_y);
        std::iota(values_vec[i].begin(), values_vec[i].end() - 2, 0.0);
    });

    ////////////////////////////////////////////////////////////////
    // computation   
    if ( run_flag == "scatter")
    {
        fft_2d_scatter(std::ref(values_vec), FFT_BACKEND_PLAN_FLAG);
    } 
    else if ( run_flag == "ata")
    {
        fft_2d_all_to_all(std::ref(values_vec), FFT_BACKEND_PLAN_FLAG);
    } 
    else if ( run_flag == "shared")
    {
        fft_2d_shared(std::ref(values_vec), FFT_BACKEND_PLAN_FLAG);
    } 
    

    ////////////////////////////////////////////////////////////////
    // print results
    if (print_result)
    {
        const std::uint32_t this_locality = hpx::get_locality_id();
        sleep(this_locality);
        std::string msg = "\nAlgorithm {1}\nLocality {2}\n";
        hpx::util::format_to(hpx::cout, msg, 
                        run_flag, this_locality) << std::flush;
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
    ("run",value<std::string>()->default_value("task_scatter"), "Choose 2d FFT algorithm");

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {"hpx.run_hpx_main!=1"};

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    return hpx::init(argc, argv, init_args);
}

#endif