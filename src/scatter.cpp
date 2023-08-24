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

using namespace hpx::collectives;

using vector = std::vector<std::uint32_t, std::allocator<std::uint32_t>>;
using split_vector = std::vector<vector>;

#define N_X 8
#define N_Y 8 

void test_multiple_use_with_generation()
{
    // parameters
    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::uint32_t this_locality = hpx::get_locality_id();
    std::uint32_t size= N_Y;
    std::uint32_t sub_size= size / num_localities;
    std::uint32_t N = N_X / num_localities;
    HPX_TEST_LTE(std::uint32_t(2), num_localities);//does not run on one locality
    // setup communicators
    std::vector<const char*> scatter_basenames(num_localities);
    std::vector<communicator> scatter_communicators(num_localities);
    hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto i)
    {
        scatter_basenames[i] = std::move(std::to_string(i).c_str());
        
        scatter_communicators[i] = std::move(hpx::collectives::create_communicator(scatter_basenames[i],
            num_sites_arg(num_localities), this_site_arg(this_locality)));
    });
    // create values and do stuff
    std::vector<vector> values_vec(N);
    hpx::experimental::for_loop(hpx::execution::par, 0, N, [&](auto i)
    {
        vector v(size);
        //std::fill(v.begin(), v.end(), this_locality);
        //std::iota(v.begin(), v.end(), 0+10*this_locality);
        std::iota(v.begin(), v.end(), 0);
        values_vec[i] = std::move(v);
    });
    // divide value vector
    std::vector<split_vector> values_div_vec(N);
    hpx::experimental::for_loop(hpx::execution::par, 0, N, [&](auto i)
    {
        values_div_vec[i].resize(num_localities);
        hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto j)
        {
            vector tmp(std::make_move_iterator(values_vec[i].begin()+j*sub_size),
                                std::make_move_iterator(values_vec[i].begin()+(j+1)*sub_size)); // move;
            values_div_vec[i][j] = std::move(tmp);
        });
    });
    // holder fot data
    std::vector<std::vector<vector>> r3(num_localities);
    // holder for futures
    std::vector<hpx::shared_future<vector>> r2(num_localities);
    // loop over N vectors per locality
    hpx::experimental::for_loop(hpx::execution::seq, 0, N, [&](auto i)
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
                r2[other_locality] = std::move(result);
            }
        });
        ///////
        // send from this locality
        hpx::shared_future<vector> result = scatter_to(scatter_communicators[this_locality], std::move(values_div_vec[i]), generation_arg(i+1));
        ///////
        // store future
        r2[this_locality] = std::move(result);

        // get futures
        hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto other_locality)
        {
            r3[other_locality].push_back(r2[other_locality].get());
        });
    });
    
    // print data
    sleep(this_locality);
    if (1)
    {
        char const* msg = "\nLocality {1}:";
        hpx::util::format_to(hpx::cout, msg, this_locality) << std::flush;
        for (auto r4 : r3)
        {
            char const* msg = "\n";
            hpx::util::format_to(hpx::cout, msg) << std::flush;
            for (auto r5 : r4)
            {
                hpx::util::format_to(hpx::cout, msg) << std::flush;
                for (auto v : r5)
                {
                    char const* msg = "{1} - ";
                    hpx::util::format_to(hpx::cout, msg, v) << std::flush;
                }
            }
        }
        char const* msg2 = "\n";
        hpx::util::format_to(hpx::cout, msg2) << std::flush;
    }

    // local transpose
    // create transpose values data structure
    std::vector<vector> trans_values_vec(sub_size);
    // IMPORTANT: rearrange loops
    hpx::experimental::for_loop(hpx::execution::par, 0, sub_size, [&](auto i)
    {
        hpx::experimental::for_loop(hpx::execution::par, 0, num_localities, [&](auto j)
        {   
            hpx::experimental::for_loop(hpx::execution::par, 0, N, [&](auto k)
            {
                trans_values_vec[i].push_back(r3[j][k][i]);
            });
        });
    });

    if (1)
    {
        char const* msg = "\nTrans Locality {1}:";
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










}

void test_multiple_use_with_generation3()
{
    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    HPX_TEST_LTE(std::uint32_t(2), num_localities);//does not run on one locality
    std::uint32_t this_locality = hpx::get_locality_id();

    
    std::vector<const char*> scatter_basenames(num_localities);
    std::vector<communicator> scatter_communicators(num_localities);

    // setup communicators
    for (std::size_t i = 0; i != num_localities; ++i)
    {
        scatter_basenames[i] = std::move(std::to_string(i).c_str());
        
        scatter_communicators[i] = std::move(hpx::collectives::create_communicator(scatter_basenames[i],
            num_sites_arg(num_localities), this_site_arg(this_locality)));
    }


    // parameters
    std::uint32_t size=8;
    std::uint32_t sub_size=size/num_localities;
    std::uint32_t N = 20;

    // create values and do stuff
    std::vector<vector> values_vec;
    std::vector<split_vector> values_div_vec(N);

    for(std::uint32_t i = 0; i != N; ++i)
    {
        vector v(size);
        //std::fill(v.begin(), v.end(), this_locality);
        std::iota(v.begin(), v.end(), 0+10*this_locality);
        values_vec.push_back(v);
    }

    // divide value vector
    for(std::uint32_t i = 0; i != N; ++i)
    {
        for (std::size_t j = 0; j != num_localities; ++j)
        {
            vector tmp(std::make_move_iterator(values_vec[i].begin()+j*sub_size),
                                std::make_move_iterator(values_vec[i].begin()+(j+1)*sub_size)); // move;
            values_div_vec[i].push_back(tmp);
        }
    }

    // //vector values;
    // split_vector values_div;
    // split_vector values_div2;
    // // create values and do stuff
    // vector values(size);
    // //std::fill(v.begin(), v.end(), this_locality);
    // std::iota(values.begin(), values.end(), 0+10*this_locality);

    // for (std::size_t j = 0; j != num_localities; ++j)
    // {
    //     vector tmp(std::make_move_iterator(values.begin()+j*sub_size),
    //                         std::make_move_iterator(values.begin()+(j+1)*sub_size)); // move;
    //     values_div.push_back(tmp);
    // }

    // for (auto vec : values_div2)
    // {
    //     char const* msg = "\n";
    //     hpx::util::format_to(hpx::cout, msg)
    //         << std::flush;
    //     for (auto v : vec)
    //     {
    //         char const* msg = "{1} - ";
    //         hpx::util::format_to(hpx::cout, msg, v)
    //             << std::flush;
    //     }
    // }

    std::vector<std::vector<vector>> r3(num_localities);
    //initialize
    //for (auto )
    //std::vector<hpx::shared_future<vector>> r2(N);
    //std::vector<hpx::shared_future<vector>> r;//(N);

    // send
    // hpx::shared_future<vector> result = scatter_to(scatter_communicators[this_locality], std::move(values_div_vec[0]), generation_arg(1));
    // r2[this_locality] = std::move(result);
    
    //r2[this_locality].resize(N);
    if (this_locality== 0)
    {
        for(std::uint32_t i = 0; i != N; ++i)
        {
            hpx::shared_future<vector> result = scatter_to(scatter_communicators[this_locality], std::move(values_div_vec[i]), generation_arg(i+1));

            //r2[this_locality][i] = std::move(result);
            //r.push_back(std::move(result));
            r3[this_locality].push_back(result.get());
            //r[i] = std::move(result);
        }
    }
    else
    {    
        // receive
        for(std::uint32_t i = 0; i != N; ++i)
        {
            hpx::shared_future<vector> result = scatter_from<vector>(scatter_communicators[0], generation_arg(i+1));
            r3[0].push_back(result.get());
            //r2[i] = std::move(result);            
        } 
    //         for(std::uint32_t i = 0; i != N; ++i)
    // {
    //     for(std::uint32_t other_locality = 0; other_locality != num_localities; ++other_locality)
    //     {
    //         if(this_locality != other_locality)
    //         {
    //             //r3[other_locality][i] = r2[other_locality][i].get();
    //             r3[other_locality].push_back(r2[i].get());
    //             ////////////////////////////////////////////////////////////77
    //             // if (this_locality == 1)
    //             // {
    //             //                     char const* msg = "\n{1}:";
    //             // hpx::util::format_to(hpx::cout, msg, i)
    //             //     << std::flush;
    //             //     for (auto v : r3[other_locality][i])
    //             //     {
    //             //             char const* msg = "{1} - ";
    //             //             hpx::util::format_to(hpx::cout, msg, v)
    //             //                 << std::flush;
    //             //     }
    //             // }
    //             /////////////////////////////////////////////////////////////////////
    //         }
    //     }
    // } 
    }



  

    //sleep(10);
    // for(std::uint32_t other_locality = 0; other_locality != num_localities; ++other_locality)
    // {
    //     r3[other_locality].resize(N);
    // }




    sleep(this_locality);

            if (1)
        {
    char const* msg = "\nLocality {1}:";
    hpx::util::format_to(hpx::cout, msg, this_locality) << std::flush;
    for (auto r4 : r3)
    {
        char const* msg = "\n";
        hpx::util::format_to(hpx::cout, msg) << std::flush;
        for (auto r5 : r4)
        {
            hpx::util::format_to(hpx::cout, msg) << std::flush;
            for (auto v : r5)
            {
                char const* msg = "{1} - ";
                hpx::util::format_to(hpx::cout, msg, v) << std::flush;
            }
        }
    }
    char const* msg2 = "\n";
    hpx::util::format_to(hpx::cout, msg2) << std::flush;
        }
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