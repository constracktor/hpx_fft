#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>

int hpx_main(hpx::program_options::variables_map& vm)
{
  std::cout << "Hello World\n";
// Namespaces
// std:: namespace std
// stdex:: namespace std::execution
// stdexp:: namespace std::experimental
// hpx:: namespace hpx
// hpxex:: namespace hpx::execution
// hpxexp:: namespace hpx::execution::experimental
// hpxtt:: namespace hpx::this_thread::experimental

//HPX supports associating an executor with execution policies:
// seq: execute in-order (sequenced) on current thread
// par: allow parallel execution on different threads
// par(task): allow asynchronous operation

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

// // Parallel execution using sender_executor
using namespace hpx::execution::experimental;

auto sch = hpx::execution::experimental::thread_pool.scheduler();                                 // 1

// sender auto begin = schedule(sch);                                            // 2
// sender auto hi = then(begin, []{                                              // 3
//     std::cout << "Hello world! Have an int.";                                 // 3
//     return 13;                                                                // 3
// });                                                                           // 3
// sender auto add_42 = then(hi, [](int arg) { return arg + 42; });              // 4

// auto [i] = this_thread::sync_wait(add_42).value();                            // 5

auto sr_exec= hpxexp::scheduler_executor();
hpx::for_each(hpxex::par.on(sr_exec), v.begin(), v.end(), [](double val) { std::cout << "sender" << val << std::endl; });

// // Parallel asynchronous (lazy) execution using sender_executor
// sender auto s = hpx::for_each(par(task).on(sr_exec), v.begin(), v.end(), [](double val) { std::cout << val << std::endl; });
// hpxtt::sync_wait(s); // start execution and wait for completion


// // Executors: Parallel Algorithms
// auto exec = ex::sender_executor();
// auto result =
// hpxexp::just(std::begin(c), std::end(c), [](auto) { ... })
// | hpx::for_each(par(task).on(exec))
// | hpxtt::sync_wait();

// // Executors: async_execute with futures:
// template <typename Executor, typename F, typename ... Ts>
// auto async_execute(Executor&& exec, F&& f, Ts&&... ts)
// {
//   hpx::promise<std::invoke_result_t<F, Ts...>> p;
//   auto f = p.get_future();
//   exec.sched.launch([=, p = std::move(p)]() { // copy arguments for brevity
//   p.set_value(std::invoke(f, ts...)); // assume non-void return value
//   });
//   return f;
// }

// // Executors: async_execute with sender receivers
// template <typename Executor, typename F, typename ... Ts>
// auto async_execute(Executor&& exec, F&& f, Ts&&... ts)
// {
//   return
//   hpxexp::on(exec.sched)
//   | hpxexp::then([=]() { return std::invoke(f, ts...); }));
// }


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