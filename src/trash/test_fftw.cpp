#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <complex>

#include <fftw3.h>


#define NUM_POINTS 4
#define N_DIM 2

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
    // // 1D
    // ////////////////
    // // complex to complex inplace
    // std::vector<std::complex<double>> signal;
    // signal.resize(NUM_POINTS);
    
    // fftw_plan plan_complex = fftw_plan_dft_1d(NUM_POINTS,
    //         reinterpret_cast<fftw_complex*>(signal.data()),
    //         reinterpret_cast<fftw_complex*>(signal.data()),
    //         FFTW_FORWARD,
    //         FFTW_ESTIMATE);

    // //acquire_from_somewhere(signal);
    // acquire_from_somewhere(signal);
    // fftw_execute(plan_complex);
    // do_something_with(signal,NUM_POINTS);

    // fftw_destroy_plan(plan_complex);
    // /////////////
    // // real to complex and back complex to real
    // std::vector<double> real_signal;
    // real_signal.resize(NUM_POINTS+2);

    // fftw_plan plan_real_r2c = fftw_plan_dft_r2c_1d(NUM_POINTS,
    //         real_signal.data(),
    //         reinterpret_cast<fftw_complex*>(real_signal.data()),
    //         FFTW_ESTIMATE);

    // fftw_plan plan_real_c2r = fftw_plan_dft_c2r_1d(NUM_POINTS,
    //         reinterpret_cast<fftw_complex*>(real_signal.data()),
    //         real_signal.data(),
    //         FFTW_ESTIMATE);

    // for (int i = 0; i < NUM_POINTS; ++i) 
    // {
    //     real_signal[i]=i;
    // }

    // fftw_execute(plan_real_r2c);
    
    // for (int i = 0; i < NUM_POINTS + 2; ++i) 
    // {
    //     std::cout << real_signal[i] << std::endl;
    // }

    // fftw_execute(plan_real_c2r);

    // for (int i = 0; i < NUM_POINTS; ++i) 
    // {
    //     std::cout << real_signal[i] / NUM_POINTS << std::endl;
    // }

    // /////////////////////
    // // 2 x NUM_POINTS 1D r2c and c2r
    // std::vector<double> real_signal_2;
    // real_signal_2.resize(N_DIM * (NUM_POINTS+2));
    // for (int i = 0; i < NUM_POINTS; ++i) 
    // {
    //     for (int j = 0; j < N_DIM; ++j) 
    //     {
    //         real_signal_2[i + j * (NUM_POINTS + 2)]=i;
    //     }
    // }

    // for (int j = 0; j < N_DIM; ++j) 
    // {
    //     std::cout << "Dim: " << j << std::endl;
    //     for (int i = 0; i < NUM_POINTS + 2; ++i) 
    //     {
    //         std::cout << real_signal_2[i + j*(NUM_POINTS +2)] << std::endl;
    //     }
    // }

    // // forward
    // int rank = 1;
    // const int* n= new int(NUM_POINTS);
    // int howmany = N_DIM;
    // int idist = NUM_POINTS + 2, odist = NUM_POINTS/2 + 1; /* the distance in memory between the first element of the first array and the first element of the second array */
    // int istride = 1, ostride = 1; /* array is contiguous in memory */
    // //const int inembed = n+2, onembed = n+2;

    // fftw_plan plan_2_r2c = fftw_plan_many_dft_r2c(rank, n, howmany,
    //                         real_signal_2.data(), NULL,
    //                         istride, idist,
    //                         reinterpret_cast<fftw_complex*>(real_signal_2.data()), NULL,
    //                         ostride, odist, FFTW_ESTIMATE);
    // fftw_execute(plan_2_r2c);
    
    // for (int j = 0; j < N_DIM; ++j) 
    // {
    //     std::cout << "Dim: " << j << std::endl;
    //     for (int i = 0; i < NUM_POINTS + 2; ++i) 
    //     {
    //         std::cout << real_signal_2[i + j*(NUM_POINTS +2)] << std::endl;
    //     }
    // }

    // //backward
    // fftw_plan plan_2_c2r = fftw_plan_many_dft_c2r(rank, n, howmany,
    //                         reinterpret_cast<fftw_complex*>(real_signal_2.data()), NULL,
    //                         istride, odist,
    //                         real_signal_2.data(), NULL,
    //                         ostride, idist, FFTW_ESTIMATE);
    // fftw_execute(plan_2_c2r);

    
    // for (int j = 0; j < N_DIM; ++j) 
    // {
    //     std::cout << "Dim: " << j << std::endl;
    //     for (int i = 0; i < NUM_POINTS; ++i) 
    //     {
    //         std::cout << real_signal_2[i + j*(NUM_POINTS +2)] / NUM_POINTS << std::endl;
    //     }
    // }



    //////////////////////////////////////////////////////////////////////////////////\
    // 3D
    // int n_dim=4;
    // int n_c = n_dim / 2 + 1;
    // int N = n_dim * n_dim * n_dim;
    // std::vector<double> input;
    // std::vector<std::complex<double>> output;
    // input.resize(N);
    // output.resize(n_c * n_dim * n_dim);
    // for(int i; i<N; ++i)
    // {
    //     input[i] = i % (N/2);
    // }
    // // forward
    // fftw_plan plan_r2c_3d = fftw_plan_dft_r2c_3d(n_dim, n_dim, n_dim,
    //                                             input.data(),
    //                                             reinterpret_cast<fftw_complex*>(output.data()),
    //                                             FFTW_ESTIMATE);
    // fftw_execute(plan_r2c_3d);
    // //do_something_with(output,N);
    // for(int i; i<N; ++i)
    // {
    //     input[i] = 0;
    // }
    // // backward
    // fftw_plan plan_c2r_3d = fftw_plan_dft_c2r_3d(n_dim, n_dim, n_dim,
    //                                         reinterpret_cast<fftw_complex*>(output.data()),
    //                                         input.data(),
    //                                         FFTW_ESTIMATE);
    // fftw_execute(plan_c2r_3d);
    // for(int i; i<N; ++i)
    // {
    //     //std::cout << input[i] / N << std::endl;
    // }

    ///////////////////////////////////////////////////////////////////////////////////
    // 2D
    // fftw 
    int n_dim=4;
    int N = n_dim*n_dim;
    std::vector<double> input;
    input.resize(N);
    std::vector<double> output;
    output.resize((n_dim+2)*n_dim);
    for(int i=0; i<n_dim; ++i)
    {
        for(int j=0; j<n_dim; ++j)
        {
            input[(n_dim)*i + j] = (n_dim)*i + j;
        }
    }
    for(int i; i<N; ++i)
    {
        //std::cout << input[i] << std::endl;
    }
    // forward
    fftw_plan plan_r2c_2d = fftw_plan_dft_r2c_2d(n_dim, n_dim,
                                                input.data(),
                                                reinterpret_cast<fftw_complex*>(output.data()),
                                                FFTW_ESTIMATE);

    fftw_execute(plan_r2c_2d);
    for(int i; i<(n_dim+2)*n_dim; i=i+2)
    {
        std::cout << output[i] << " " << output[i+1]  << std::endl;
    }
    // backward
    fftw_plan plan_c2r_2d = fftw_plan_dft_c2r_2d(n_dim, n_dim,
                                            reinterpret_cast<fftw_complex*>(output.data()),
                                            input.data(),
                                            FFTW_ESTIMATE);
    fftw_execute(plan_c2r_2d);
    for(int i; i<N; ++i)
    {
        //std::cout << input[i] / N << std::endl;
    }
    /////////
    // own implementation
    N = (n_dim+2)*n_dim;
    std::vector<double> input_2;
    input_2.resize(N);
    for(int i=0; i<n_dim; ++i)
    {
        for(int j=0; j<n_dim; ++j)
        {
            input_2[(n_dim+2)*i + j] = (n_dim)*i + j;
        }
    }
    for(int i; i<N; ++i)
    {
        std::cout << input_2[i] << std::endl;
    }
    // forward
    int rank = 1;
    const int* n= new int(n_dim);
    int howmany = n_dim;
    int idist = n_dim + 2, odist = n_dim/2 + 1; /* the distance in memory between the first element of the first array and the first element of the second array */
    int istride = 1, ostride = 1; /* array is contiguous in memory */
    //const int inembed = n+2, onembed = n+2;

    fftw_plan plan_2_r2c = fftw_plan_many_dft_r2c(rank, n, howmany,
                            input_2.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_2_r2c);

    for (int i = 0; i < n_dim; ++i) 
    {
        std::cout << "Row: " << i << std::endl;
        for (int j = 0; j < n_dim+2; ++j) 
        {
            std::cout << input_2[j + i*(n_dim +2)] << std::endl;
        }
    }
 
    // //backward for verification
    // fftw_plan plan_2_c2r = fftw_plan_many_dft_c2r(rank, n, howmany,
    //                         reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
    //                         istride, odist,
    //                         input_2.data(), NULL,
    //                         ostride, idist, FFTW_ESTIMATE);
    // fftw_execute(plan_2_c2r);

    
    // for (int j = 0; j < n_dim; ++j) 
    // {
    //     std::cout << "Dim: " << j << std::endl;
    //     for (int i = 0; i < n_dim; ++i) 
    //     {
    //         std::cout << input_2[i + j*(n_dim +2)] / n_dim << std::endl;
    //     }
    // }

    // local transpose
    std::vector<double> transpose;
    transpose.resize(N);
    for (int i = 0; i < n_dim; ++i) 
    {
        for (int j = 0; j < n_dim/2+1; ++j) 
        {
            transpose[j * (2 * n_dim) +2*i] = input_2[i *(n_dim+2) + 2*j];
            transpose[j * (2 * n_dim) +2*i + 1] = input_2[i *(n_dim+2) + 2*j + 1];
        }
    }

    for (int i = 0; i < n_dim/2 + 1; ++i) 
    {
        std::cout << "Row: " << i << std::endl;
        for (int j = 0; j < 2 * n_dim; ++j) 
        {
            std::cout << transpose[j + i*(2*n_dim)] << std::endl;
        }
    }

    fftw_plan plan_2_c2c = fftw_plan_many_dft(rank, n, howmany,
                        reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                        istride, n_dim,
                        reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                        ostride, n_dim, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan_2_c2c);
    for(int i; i<(n_dim+2)*n_dim; i=i+2)
    {
        std::cout << transpose[i] << " " << transpose[i+1]  << std::endl;
    }
    // strided fft
    std::vector<double> input_3;
    input_3.resize(N);
    const int* n1= new int(n_dim/+1);
    fftw_plan plan_2_c2c_s = fftw_plan_many_dft(rank, n, 3,
                    reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                    3, 1,
                    reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                    3, 1, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan_2_c2c_s);
    for(int i; i<(n_dim+2)*n_dim; i=i+2)
    {
        std::cout << input_2[i] << " " << input_2[i+1]  << std::endl;
    }


    // fft_plan_Fourier_forward_z = FFTW_PLAN_MANY_DFT_R2C(1, Nz_dims, field.maxrx * field.maxry,
    //     reinterpret_cast<Real*>(RA.data()), NULL,
    //     1, field.Nz+2,
    //     reinterpret_cast<FFTW_Complex*>(RA.data()), NULL,
    //     1, field.Nz/2+1,
    //     FFTW_PLAN_FLAG);


    // fft_plan_Fourier_forward_x = FFTW_PLAN_MANY_DFT(1, Nx_dims, field.maxfy * field.maxfz,
    //     reinterpret_cast<FFTW_Complex*>(FA.data()), NULL,
    //     field.maxfy * field.maxfz, 1,
    //     reinterpret_cast<FFTW_Complex*>(FA.data()), NULL,
    //     field.maxfy * field.maxfz, 1,
    //     FFTW_FORWARD, FFTW_PLAN_FLAG);
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