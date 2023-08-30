//  Copyright (c) 2019-2022 Hartmut Kaiser
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

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

using namespace hpx::collectives;

constexpr char const* all_to_all_direct_basename = "/test/all_to_all_direct/";

// void test_multiple_use_with_generation()
// {
//     std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
//     std::uint32_t this_locality = hpx::get_locality_id();

//     auto all_to_all_direct_client =
//         create_communicator(all_to_all_direct_basename,
//             num_sites_arg(num_localities), this_site_arg(this_locality));


//     // test functionality based on immediate local result value
//     for (int i = 0; i != 1; ++i)
//     {
//         std::vector<std::vector<std::uint32_t>> values(num_localities);
        
//         for (std::size_t j = 0; j != num_localities; ++j)
//         {
//             values[j].resize(3);
//             std::fill(values[j].begin(), values[j].end(), this_locality);
//         }


//         hpx::future<std::vector<std::vector<std::uint32_t>>> overall_result = all_to_all(
//             all_to_all_direct_client, std::move(values), generation_arg(i + 1));

//         std::vector<std::vector<std::uint32_t>> r = overall_result.get();

//         if(1)//this_locality == 0)
//         {
//             for (std::size_t j = 0; j != num_localities; ++j)
//             {
//                 for (auto v : r[j])
//                 {
//                 char const* msg = "Locality {1} r: {2}\n";
//                 hpx::util::format_to(hpx::cout, msg, this_locality, v)
//                     << std::flush;
//                 }
//             }
//         }
//     }
// }

void test_multiple_use_with_generation()
{
    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::uint32_t this_locality = hpx::get_locality_id();

    std::uint32_t size=4;
    std::uint32_t sub_size=size/num_localities;

    auto all_to_all_direct_client =
        create_communicator(all_to_all_direct_basename,
            num_sites_arg(num_localities), this_site_arg(this_locality));


    // do stuff with vector
    std::vector<std::uint32_t> values(size);
    std::iota(values.begin(), values.end(), 0);

    // divide vector
    std::vector<std::vector<std::uint32_t>> values_div(num_localities);
        
    for (std::size_t i = 0; i != num_localities; ++i)
    {
        std::vector<std::uint32_t> tmp(std::make_move_iterator(values.begin()+i*sub_size),
                             std::make_move_iterator(values.begin()+(i+1)*sub_size)); // move;
        values_div.push_back(tmp);
    }
    
    if(1)//this_locality == 0)
    {
            for (auto va : values_div)
            {
                for (auto v : va)
                {
                char const* msg = "Locality {1} v: {2}\n";
                hpx::util::format_to(hpx::cout, msg, this_locality, v)
                    << std::flush;
                }
            }
    }
    

    hpx::future<std::vector<std::vector<std::uint32_t>>> overall_result = all_to_all(
        all_to_all_direct_client, std::move(values_div),generation_arg(0 + 1));
    

    std::vector<std::vector<std::uint32_t>> r = overall_result.get();

    if(1)//this_locality == 0)
    {
        for (auto va : r)
        {
            std::cout << va.size() << " hello\n";
            for (auto v : va)
            {
                std::cout << "in\n";
            char const* msg = "Locality {1} r: {2}\n";
            hpx::util::format_to(hpx::cout, msg, this_locality, v)
                << std::flush;
            }
        }
    }
}


int hpx_main()
{
    //test_one_shot_use();
    //test_multiple_use();
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