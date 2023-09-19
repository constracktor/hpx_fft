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
    hpx::future<void> f = hpx::async(hpx::annotated_function(&print_hello_world, "hello_world"), this_locality);

    hpx::future<void> f2 = f.then(
        [](hpx::future<void> r)
        { 
            // this is guaranteed not to block as the continuation 
            // will be called only after `f` has become ready`)
            //r.get();
            hpx::async(hpx::annotated_function(&print_hello_world, "hello_world2"), 10);
        });
    f2.get();
  //  hpx::future<int> f = hpx::async([]{ return 42; });

// hpx::future<int> f2 = f.then(
//     [](hpx::future<int> r) 
//     {
//         // this is guaranteed not to block as the continuation 
//         // will be called only after `f` has become ready (note:
//         // `f` has been moved-to `r`)
//         int result = r.get();

//         // 'reinitialize' the future
//         r = hpx::async([]{ return 21; });

//         // ...do things with 'r'

//         return result;
//     });

// f2 now represents the result of executing the chain of the two lambdas
//std::cout << f2.get() << '\n';        // prints '42'

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