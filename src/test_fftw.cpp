#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <complex>

#include <fftw3.h>


#define NUM_POINTS 4

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

void do_something_with(std::vector<std::complex<double>>& result, int N) {
  int i;
  for (i = 0; i < N; ++i) {
    // double mag = sqrt(result[i][REAL] * result[i][REAL] +
    // result[i][IMAG] * result[i][IMAG]);
    printf("%g - %g\n", result[i].real(), result[i].imag());
    //printf("%g - %g\n", result[i][REAL], result[i][IMAG]);
  }
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    // 1D
    ////////////////
    // complex to complex inplace
    std::vector<std::complex<double>> signal;
    signal.resize(NUM_POINTS);
    
    fftw_plan plan_complex = fftw_plan_dft_1d(NUM_POINTS,
            reinterpret_cast<fftw_complex*>(signal.data()),
            reinterpret_cast<fftw_complex*>(signal.data()),
            FFTW_FORWARD,
            FFTW_ESTIMATE);

    //acquire_from_somewhere(signal);
    acquire_from_somewhere(signal);
    fftw_execute(plan_complex);
    do_something_with(signal,NUM_POINTS);

    fftw_destroy_plan(plan_complex);
    /////////////
    // real to complex and back complex to real
    std::vector<double> real_signal;
    real_signal.resize(NUM_POINTS+2);

    fftw_plan plan_real_r2c = fftw_plan_dft_r2c_1d(NUM_POINTS,
            real_signal.data(),
            reinterpret_cast<fftw_complex*>(real_signal.data()),
            FFTW_ESTIMATE);

    fftw_plan plan_real_c2r = fftw_plan_dft_c2r_1d(NUM_POINTS,
            reinterpret_cast<fftw_complex*>(real_signal.data()),
            real_signal.data(),
            FFTW_ESTIMATE);

    for (int i = 0; i < NUM_POINTS; ++i) 
    {
        real_signal[i]=i;
    }

    fftw_execute(plan_real_r2c);
    
    for (int i = 0; i < NUM_POINTS + 2; ++i) 
    {
        std::cout << real_signal[i] << std::endl;
    }

    fftw_execute(plan_real_c2r);

    for (int i = 0; i < NUM_POINTS; ++i) 
    {
        std::cout << real_signal[i] / NUM_POINTS << std::endl;
    }

    /////////////////////
    // 2 x NUM_POINTS 1D r2c and c2r
    std::vector<double> real_signal_2;
    real_signal_2.resize(2 * (NUM_POINTS+2));
    for (int i = 0; i < NUM_POINTS; ++i) 
    {
        real_signal_2[i]=i;
        real_signal_2[i + NUM_POINTS +2] = i;
    }

    std::cout << "first" << std::endl;
    for (int i = 0; i < NUM_POINTS + 2; ++i) 
    {
        std::cout << real_signal_2[i] << std::endl;
    }
    std::cout << "second" << std::endl;
    for (int i = 0; i < NUM_POINTS + 2; ++i) 
    {
        std::cout << real_signal_2[i + NUM_POINTS +2] << std::endl;
    }


    int rank = 1;
    const int* n= new int(4);
    int howmany = 2;
    int idist = NUM_POINTS + 2, odist = NUM_POINTS/2 + 1; /* the distance in memory between the first element of the first array and the first element of the second array */
    int istride = 1, ostride = 1; /* array is contiguous in memory */
    //const int inembed = n+2, onembed = n+2;

    fftw_plan plan_2_r2c = fftw_plan_many_dft_r2c(rank, n, howmany,
                            real_signal_2.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(real_signal_2.data()), NULL,
                            ostride, odist, FFTW_ESTIMATE);



                            




    fftw_execute(plan_2_r2c);
    
    std::cout << "first" << std::endl;
    for (int i = 0; i < NUM_POINTS + 2; ++i) 
    {
        std::cout << real_signal_2[i] << std::endl;
    }
    std::cout << "second" << std::endl;
    for (int i = 0; i < NUM_POINTS + 2; ++i) 
    {
        std::cout << real_signal_2[i + NUM_POINTS +2] << std::endl;
    }







    //////////////////////////////////////////////////////////////////////////////////\
    // 3D
    int n_dim=4;
    int n_c = n_dim / 2 + 1;
    int N = n_dim * n_dim * n_dim;
    std::vector<double> input;
    std::vector<std::complex<double>> output;
    input.resize(N);
    output.resize(n_c * n_dim * n_dim);
    for(int i; i<N; ++i)
    {
        input[i] = i % (N/2);
    }
    // forward
    fftw_plan plan_r2c_3d = fftw_plan_dft_r2c_3d(n_dim, n_dim, n_dim,
                                                input.data(),
                                                reinterpret_cast<fftw_complex*>(output.data()),
                                                FFTW_ESTIMATE);
    fftw_execute(plan_r2c_3d);
    //do_something_with(output,N);
    for(int i; i<N; ++i)
    {
        input[i] = 0;
    }
    // backward
    fftw_plan plan_c2r_3d = fftw_plan_dft_c2r_3d(n_dim, n_dim, n_dim,
                                            reinterpret_cast<fftw_complex*>(output.data()),
                                            input.data(),
                                            FFTW_ESTIMATE);
    fftw_execute(plan_c2r_3d);
    for(int i; i<N; ++i)
    {
        //std::cout << input[i] / N << std::endl;
    }



// namespace hpxex = hpx::execution;
// namespace hpxexp = hpx::execution::experimental;
// //namespace hpxtt = hpx::this_thread::experimental;

// // Parallel execution using default executor
// std::vector v = {1.0, 2.0};
// hpx::for_each(hpxex::par, v.begin(), v.end(), [](double val) { std::cout << "default " << val << std::endl; });

// // Parallel execution using parallel_executor
// hpxex::parallel_executor exec;
// hpx::for_each(hpxex::par.on(exec), v.begin(), v.end(), [](double val) { std::cout << "parallel" << val << std::endl; });

// // Parallel asynchronous (eager) execution using parallel_executor
// hpx::future f = hpx::for_each(hpxex::par(hpxex::task).on(exec), v.begin(), v.end(), [](double val) { std::cout << "async" << val << std::endl; });
// f.get(); // wait for completion


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