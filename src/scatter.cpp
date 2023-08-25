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
#include <hpx/modules/testing.hpp>

#include <hpx/iostream.hpp>


#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// FFTW
#include <fftw3.h>

using namespace hpx::collectives;

using vector = std::vector<double, std::allocator<double>>;
using split_vector = std::vector<vector>;

#define N_X 8
#define N_Y 14 

void test_multiple_use_with_generation()
{
    // hpx parameters
    std::uint32_t this_locality = hpx::get_locality_id();
    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    HPX_TEST_LTE(std::uint32_t(2), num_localities);//does not run on one locality
    // fft dimension parameters
    std::uint32_t dim_c_x = N_X; 
    std::uint32_t dim_r_y = N_Y;
    std::uint32_t dim_c_y = dim_r_y / 2 + 1;
    std::uint32_t dim_c_y_part = 2 * dim_c_y / num_localities;
    // division parameters
    std::uint32_t n_x_local = dim_c_x / num_localities;
    std::uint32_t n_y_local = dim_c_y / num_localities;
    
    
    
    
    std::vector<const char*> scatter_basenames(num_localities);
    std::vector<communicator> scatter_communicators(num_localities);
    
    std::vector<vector> values_vec(n_x_local);
    std::vector<vector> trans_values_vec(n_y_local);
    std::vector<split_vector> values_div_vec(n_x_local);
    
    unsigned FFTW_PLAN_FLAG = FFTW_ESTIMATE;
    std::vector<fftw_plan> plans_1D_r2c(n_x_local);
    std::vector<fftw_plan> plans_1D_c2c(n_y_local);
    
    // holder fot data
    std::vector<std::vector<vector>> communication_vec(num_localities);
    // holder for futures
    std::vector<hpx::shared_future<vector>> communication_tmp_futures(num_localities);
    ////////////////////////////////////////////////////////////////
    // setup communicators
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        scatter_basenames[i] = std::move(std::to_string(i).c_str());
        
        scatter_communicators[i] = std::move(hpx::collectives::create_communicator(scatter_basenames[i],
            num_sites_arg(num_localities), this_site_arg(this_locality)));
    });

    // initialize values
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        vector v(2* dim_c_y);
        //std::fill(v.begin(), v.end() - 2, 1.0);
        //std::iota(v.begin(), v.end(), 0+10*this_locality);
        std::iota(v.begin(), v.end() - 2, 0.0);
        values_vec[i] = std::move(v);
    });
    hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto i)
    {
        trans_values_vec[i].resize(2*dim_c_x);
    });
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        values_div_vec[i].resize(num_localities);
    });
    ////////////////////////////////////////////////////////////////
    //FFTW plans
    // forward step one: r2c in y-direction
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        plans_1D_r2c[i] =  fftw_plan_dft_r2c_1d(dim_r_y,
                            values_vec[i].data(),
                            reinterpret_cast<fftw_complex*>(values_vec[i].data()),
                            FFTW_PLAN_FLAG);
    });
    // forward step two: c2c in x-direction
    hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto i)
    {
        plans_1D_c2c[i] =  fftw_plan_dft_1d(dim_c_x, 
                            reinterpret_cast<fftw_complex*>(trans_values_vec[i].data()), 
                            reinterpret_cast<fftw_complex*>(trans_values_vec[i].data()), 
                            FFTW_FORWARD, FFTW_PLAN_FLAG);
    });
    /////////////////////////////////////////////////////////////////
    // FFTW 1D in x-direction 
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        fftw_execute(plans_1D_r2c[i]);
    });
    ////////////////////////////////////////////////////////////////
    // divide value vector
    hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto i)
    {
        hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto j)
        {
            vector tmp(std::make_move_iterator(values_vec[i].begin()+j*dim_c_y_part),
                                std::make_move_iterator(values_vec[i].begin()+(j+1)*dim_c_y_part)); // move;
            values_div_vec[i][j] = std::move(tmp);

        });
    });
    ////////////////////////////////////////////////////////////////
    // communication
    if(num_localities == 0)
    {
        communication_vec = std::move(values_div_vec);
    }
    else
    {
        hpx::experimental::for_loop(hpx::execution::seq, 0, n_x_local, [&](auto i)
        {
            ///////
            // receive from other localities
            hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto other_locality)
            {
                if(this_locality != other_locality)
                {
                    hpx::shared_future<vector> result = scatter_from<vector>(scatter_communicators[other_locality], generation_arg(i+1));
                    ///////
                    // store future
                    communication_tmp_futures[other_locality] = std::move(result);
                }
            });
            ///////
            // send from this locality
            hpx::shared_future<vector> result = scatter_to(scatter_communicators[this_locality], std::move(values_div_vec[i]), generation_arg(i+1));
            ///////
            // store future
            communication_tmp_futures[this_locality] = std::move(result);

            // get futures
            hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto other_locality)
            {
                communication_vec[other_locality].push_back(communication_tmp_futures[other_locality].get());
            });
        });
    }
    ////////////////////////////////////////////////////////////////
    // local transpose
    // optimize loops
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {   
        hpx::experimental::for_loop(hpx::execution::par, 0, n_x_local, [&](auto j)
        {
            hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto k)
            {
                trans_values_vec[k][j* num_localities*2 + 2*i] = communication_vec[i][j][2*k];
                trans_values_vec[k][j* num_localities*2 + 2*i+1] = communication_vec[i][j][2*k+1];
            });
        });
        
    });
    ////////////////////////////////////////////////////////////////
    // FFTW 1D y-direction
    // forward step two
    hpx::experimental::for_loop(hpx::execution::par, 0, n_y_local, [&](auto i)
    {
        fftw_execute(plans_1D_c2c[i]);
    });
    ////////////////////////////////////////////////////////////////
    // print result
    sleep(this_locality);
    if (1)
    {
        char const* msg = "\nLocality {1}:";
        hpx::util::format_to(hpx::cout, msg, this_locality) << std::flush;
        for (auto r5 : trans_values_vec)
        {
            char const* msg = "\n";
            hpx::util::format_to(hpx::cout, msg) << std::flush;
            for (auto v : r5)
            {
                char const* msg = "{1} - ";
                hpx::util::format_to(hpx::cout, msg, v) << std::flush;
            }
        }
        char const* msg2 = "\n";
        hpx::util::format_to(hpx::cout, msg2) << std::flush;
    }
    ////////////////////////////////////////////////////////////////
}

int hpx_main()
{
    test_multiple_use_with_generation();

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    std::vector<std::string> const cfg = {"hpx.run_hpx_main!=1"};

    hpx::init_params init_args;
    init_args.cfg = cfg;

    HPX_TEST_EQ(hpx::init(argc, argv, init_args), 0);
    return hpx::util::report_errors();
}

#endif