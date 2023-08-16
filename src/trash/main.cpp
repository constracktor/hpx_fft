#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <complex>

#include <fftw3.h>


#define NUM_POINTS 8

#include <stdio.h>
#include <math.h>

#define REAL 0
#define IMAG 1

void acquire_from_somewhere(std::vector<std::complex<double>>& signal) {
  /* Generate two sine waves of different frequencies and
  * amplitudes.
  */
  int i;
  for (i = 0; i < NUM_POINTS; ++i) {
    double theta = (double)i / (double)NUM_POINTS * M_PI;

    //signal[i][REAL] = i;// 1.0 * cos(10.0 * theta) + 0.5 * cos(25.0 * theta);

    //signal[i][IMAG] = NUM_POINTS - 1 - i;// 1.0 * sin(10.0 * theta) + 0.5 * sin(25.0 * theta);

    signal[i].real(i);
    signal[i].imag(0);
  }
}

void do_something_with(std::vector<std::complex<double>>& result) {
  int i;
  for (i = 0; i < NUM_POINTS; ++i) {
    // double mag = sqrt(result[i][REAL] * result[i][REAL] +
    // result[i][IMAG] * result[i][IMAG]);
    printf("%g - %g\n", result[i].real(), result[i].imag());
    //printf("%g - %g\n", result[i][REAL], result[i][IMAG]);
  }
}

int hpx_main(hpx::program_options::variables_map& vm)
{
  std::vector<std::complex<double>> signal;
  std::vector<std::complex<double>> result;
  signal.resize(NUM_POINTS);
  result.resize(NUM_POINTS);

  fftw_plan plan_complex = fftw_plan_dft_1d(NUM_POINTS,
            reinterpret_cast<fftw_complex*>(signal.data()),
            reinterpret_cast<fftw_complex*>(result.data()),
            FFTW_FORWARD,
            FFTW_ESTIMATE);

  //acquire_from_somewhere(signal);
  acquire_from_somewhere(signal);
  fftw_execute(plan_complex);
  do_something_with(result);

  fftw_destroy_plan(plan_complex);

  std::vector<double> real_signal;
  real_signal.resize(NUM_POINTS);

  fftw_plan plan_real_r2c = fftw_plan_dft_r2c_1d(NUM_POINTS,
            real_signal.data(),
            reinterpret_cast<fftw_complex*>(result.data()),
            FFTW_ESTIMATE);

  fftw_plan plan_real_c2r = fftw_plan_dft_c2r_1d(NUM_POINTS,
            reinterpret_cast<fftw_complex*>(result.data()),
            real_signal.data(),
            FFTW_ESTIMATE);

    for (int i = 0; i < NUM_POINTS; ++i) 
    {
        real_signal[i]=i;
        result[i].real(10);
        result[i].imag(10);
    }
    for (int i = 0; i < NUM_POINTS; ++i) 
    {
        std::cout << real_signal[i] << std::endl;
    }
  fftw_execute(plan_real_r2c);
  do_something_with(result);
  fftw_execute(plan_real_c2r);


    for (int i = 0; i < NUM_POINTS; ++i) 
    {
        std::cout << real_signal[i] / NUM_POINTS << std::endl;
    }
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