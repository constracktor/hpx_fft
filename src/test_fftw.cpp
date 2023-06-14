#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>

#include <fftw3.h>

int hpx_main(hpx::program_options::variables_map& vm)
{
  std::cout << "Hello World\n";


namespace hpxex = hpx::execution;
namespace hpxexp = hpx::execution::experimental;
//namespace hpxtt = hpx::this_thread::experimental;

// Parallel execution using default executor
std::vector v = {1.0, 2.0};
hpx::for_each(hpxex::par, v.begin(), v.end(), [](double val) { std::cout << "default " << val << std::endl; });

// Parallel execution using parallel_executor
hpxex::parallel_executor exec;
hpx::for_each(hpxex::par.on(exec), v.begin(), v.end(), [](double val) { std::cout << "parallel" << val << std::endl; });

// Parallel asynchronous (eager) execution using parallel_executor
hpx::future f = hpx::for_each(hpxex::par(hpxex::task).on(exec), v.begin(), v.end(), [](double val) { std::cout << "async" << val << std::endl; });
f.get(); // wait for completion


  return hpx::finalize();    // Handles HPX shutdown
}

int main(int argc, char* argv[])
{
    hpx::program_options::options_description desc_commandline;
    hpx::init_params init_args;
    // Setup input arguments
    desc_commandline.add_options();
    // Run HPX main
    init_args.desc_cmdline = desc_commandline;
    return hpx::init(argc, argv, init_args);
}