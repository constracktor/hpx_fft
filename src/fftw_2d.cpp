#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <complex>
#include <chrono>

#include <fftw3.h>

void print_real(const std::vector<double>& input, int dim_r, int scaling = 0)
{
    double factor = 1.0;
    if (scaling)
    {
        factor = factor / (dim_r * dim_r);
    }
    
    for(int i=0; i<dim_r; ++i)
    {
        for(int j=0; j<dim_r + 2; ++j)
        {
            std::cout << input[(dim_r+2)*i + j] * factor << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print_complex(const std::vector<double>& input, int dim_r)
{
    for(int i=0; i<dim_r; ++i)
    {
        for(int j=0; j<dim_r + 2; j=j+2)
        {
            std::cout << "(" << input[(dim_r+2)*i + j] << " " << input[(dim_r+2)*i + j +1]  << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print_complex_t(const std::vector<double>& input, int dim_r)
{
    for(int i=0; i<dim_r/2+1; ++i)
    {
        for(int j=0; j<2*dim_r; j=j+2)
        {
            std::cout << "(" << input[(2*dim_r)*i + j] << " " << input[(2*dim_r)*i + j +1]  << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    ///////////////////////////////////////////////////////////////////////////////////
    // 2D
    int dim_r = 1024;
    int dim_c = dim_r/2 + 1;

    int N = 2 * dim_c * dim_r;

    std::vector<double> input_1, input_2, input_3;
    input_1.resize(N);
    input_2.resize(N);
    input_3.resize(N);
    // intialize with from 0 with spacers
    for(int i=0; i<dim_r; ++i)
    {
        for(int j=0; j<dim_r; ++j)
        {
            input_1[(dim_r+2)*i + j] = (dim_r)*i + j;
            input_2[(dim_r+2)*i + j] = (dim_r)*i + j;
            input_3[(dim_r+2)*i + j] = (dim_r)*i + j;
        }
    }

    std::cout << "Input: " << std::endl;
    //print_real(input_1,dim_r);
    
    ////////////////////////////////////////////////////////////////////////////////////
    // use fftw function
    ///////////
    // forward
    fftw_plan plan_r2c_2d = fftw_plan_dft_r2c_2d(dim_r, dim_r,
                                                input_1.data(),
                                                reinterpret_cast<fftw_complex*>(input_1.data()),
                                                FFTW_ESTIMATE);
    auto start_fftw = std::chrono::steady_clock::now();
    fftw_execute(plan_r2c_2d);
    auto stop_fftw = std::chrono::steady_clock::now();
    
    std::cout << "FFT: FFTW 2D" << std::endl;
    //print_complex(input_1,dim_r);
    
    ///////////
    // backward
    fftw_plan plan_c2r_2d = fftw_plan_dft_c2r_2d(dim_r, dim_r,
                                            reinterpret_cast<fftw_complex*>(input_1.data()),
                                            input_1.data(),
                                            FFTW_ESTIMATE);
    fftw_execute(plan_c2r_2d);
    
    std::cout << "IFFT: FFTW 2D" << std::endl;
    //print_real(input_1,dim_r,1);

    ////////////////////////////////////////////////////////////////////////////////////
    // use local transform   
    std::vector<double> transpose;
    transpose.resize(N);
    /////////
    // forward step one
    int rank = 1;
    const int* n= new int(dim_r);
    int howmany = dim_r;
    int idist = dim_r + 2, odist = dim_r/2 + 1; /* the distance in memory between the first element of the first array and the first element of the second array */
    int istride = 1, ostride = 1; /* array is contiguous in memory */
    //const int inembed = n+2, onembed = n+2;

    fftw_plan plan_2_r2c = fftw_plan_many_dft_r2c(rank, n, howmany,
                            input_2.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    //fftw_execute(plan_2_r2c);
 
    /////////
    // forward step two
    idist = dim_r;
    odist = dim_r; 
    fftw_plan plan_2_c2c = fftw_plan_many_dft(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                            ostride, odist, FFTW_FORWARD, FFTW_ESTIMATE);
 
    auto start_trans = std::chrono::steady_clock::now();
    fftw_execute(plan_2_r2c);
     // local transpose
    for (int i = 0; i < dim_r; ++i) 
    {
        for (int j = 0; j < dim_r/2+1; ++j) 
        {
            transpose[j * (2 * dim_r) +2*i] = input_2[i *(dim_r+2) + 2*j];
            transpose[j * (2 * dim_r) +2*i + 1] = input_2[i *(dim_r+2) + 2*j + 1];
        }
    }
    fftw_execute(plan_2_c2c);
    auto stop_trans = std::chrono::steady_clock::now();

    std::cout << "FFT: Local transpose" << std::endl;
    //print_complex_t(transpose,dim_r);

    /////////
    // backward step one
    fftw_plan plan_2_c2c_b = fftw_plan_many_dft(rank, n, howmany,
                reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                istride, idist,
                reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                ostride, odist, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan_2_c2c_b);

    // local transpose
    for (int i = 0; i < dim_r; ++i) 
    {
        for (int j = 0; j < dim_r/2+1; ++j) 
        {
            input_2[i *(dim_r+2) + 2*j] = transpose[j * (2 * dim_r) +2*i];
            input_2[i *(dim_r+2) + 2*j + 1] = transpose[j * (2 * dim_r) +2*i + 1];
        }
    }

    /////////
    // backward step two
    howmany = dim_r;
    idist = dim_r/2 + 1;
    odist = dim_r + 2;
    fftw_plan plan_2_c2r = fftw_plan_many_dft_c2r(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                            istride, idist,
                            input_2.data(), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_2_c2r);

    std::cout << "IFFT: Local transpose" << std::endl;
    //print_real(input_2,dim_r,1);

    ////////////////////////////////////////////////////////////////////////////////////
    // strided fft
    /////////
    // forward step one
    idist = dim_r + 2,
    odist = dim_r/2 + 1;
    fftw_plan plan_3_r2c = fftw_plan_many_dft_r2c(rank, n, howmany,
                            input_3.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    //fftw_execute(plan_3_r2c);

    /////////
    // forward step two
    howmany = dim_c;
    istride = dim_c;
    ostride = dim_c;
    idist = 1;
    odist =1;
    fftw_plan plan_3_c2c = fftw_plan_many_dft(rank, n, howmany,
                    reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                    istride, idist,
                    reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                    ostride, odist, FFTW_FORWARD, FFTW_ESTIMATE);

    auto start_stride = std::chrono::steady_clock::now();
    fftw_execute(plan_3_r2c);
    fftw_execute(plan_3_c2c);
    auto stop_stride = std::chrono::steady_clock::now();

    std::cout << "FFT: Strided" << std::endl;
    //print_complex(input_3,dim_r);

    /////////
    // backward step one
    fftw_plan plan_3_c2c_b = fftw_plan_many_dft(rank, n, howmany,
                reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                istride, idist,
                reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                ostride, odist, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan_3_c2c_b);

    /////////
    // backward step two
    howmany = dim_r;
    idist = dim_r/2 + 1;
    odist = dim_r + 2;
    istride = 1;
    ostride = 1;
    fftw_plan plan_3_c2r = fftw_plan_many_dft_c2r(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                            istride, idist,
                            input_3.data(), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_3_c2r);

    std::cout << "IFFT: Strided" << std::endl;
    //print_real(input_3,dim_r,1);

    auto duration_fftw = std::chrono::duration_cast<std::chrono::microseconds>(stop_fftw - start_fftw);
    auto duration_stride = std::chrono::duration_cast<std::chrono::microseconds>(stop_stride - start_stride);
    auto duration_trans = std::chrono::duration_cast<std::chrono::microseconds>(stop_trans - start_trans);
    std::cout << "Elapsed fftw (ms)=" << duration_fftw.count() << std::endl;
    std::cout << "Elapsed trans(ms)=" << duration_trans.count() << std::endl;
    std::cout << "Elapsed strided(ms)=" << duration_stride.count() << std::endl;

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