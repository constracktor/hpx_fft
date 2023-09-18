#include <hpx/config.hpp>

#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

#include <iostream>

void print_hello_world(std::uint32_t locality)
{
    std::cout << "Hello World from Locality " << locality << std::endl;
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    const std::uint32_t this_locality = hpx::get_locality_id();   
    auto future = hpx::async(hpx::annotated_function(&print_hello_world, "hello_world"), this_locality);
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options();

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {"hpx.run_hpx_main!=1"};

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    return hpx::init(argc, argv, init_args);
}

#endif