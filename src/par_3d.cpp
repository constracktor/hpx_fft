#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>

#include <fftw3.h>

void print_real(const std::vector<double>& input, int dim_r_x, int dim_r_y, int dim_c_z, int scaling = 0)
{
    double factor = 1.0;
    if (scaling)
    {
        factor = factor / (dim_r_x * dim_r_y * (2* dim_c_z - 2));
    }
    for(int i=0; i<dim_r_y; ++i)
    {
        for(int j=0; j<2*dim_c_z; ++j)
        {
            std::cout << input[(2*dim_c_z)*i + j] * factor << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print_complex(const std::vector<double>& input,  int dim_r_y, int dim_c_z)
{
    for(int i=0; i<dim_r_y; ++i)
    {
        for(int j=0; j<2*dim_c_z; j=j+2)
        {
            std::cout << "(" << input[(2*dim_c_z)*i + j] << " " << input[(2*dim_c_z)*i + j +1]  << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    ///////////////////////////////////////////////////////////////////////////////////
    // 3D
    const int dim_r_z = 10; 
    const int dim_r_y = 4;
    const int dim_r_x = 4; 

    const int dim_c_z = dim_r_z / 2 + 1;
    const int dim_c_y = dim_r_y;
    const int dim_c_x = dim_r_x;
    
    const int N_r = dim_r_x * dim_r_y;
    const int N_c = 2 * dim_c_z * dim_c_y;

    std::vector<std::vector<double>> strides(dim_c_x);
    std::vector<std::vector<double>> r_strides(dim_c_y);
    std::vector<fftw_plan> r2c_2d_plans(dim_c_x);
    std::vector<fftw_plan> c2c_1d_plans(dim_c_y);
    //////////////////////////////////////////////////////////////////////////////////////
    // intialize with from 0 with spacers
    for(auto & stride : strides)
    {
        stride.resize(N_c);
        for(int i=0; i<dim_r_y; ++i)
        {
            for(int j=0; j<dim_r_z; ++j)
            {
                stride[(2 * dim_c_z)*i + j] = (dim_r_z)*i + j;
            }
        }
    }
    // std::cout << "Input: " << std::endl;
    // for(auto & stride : strides)
    // {
    //     print_real(stride, dim_r_x, dim_r_y, dim_c_z);
    // }
    //////////////////////////////////////////////////////////////////////////////////////
    // forward
    // step 1: r2c in z- and y-direction
    // create fftw plans 
    for(int i=0; i<dim_c_x; ++i)
    {     
        r2c_2d_plans[i] = fftw_plan_dft_r2c_2d(dim_r_y, dim_r_z,
                                                strides[i].data(),
                                                reinterpret_cast<fftw_complex*>(strides[i].data()),
                                                FFTW_ESTIMATE);
    }
    // launch plans
    for(auto & r2c_2d_plan : r2c_2d_plans)
    {
        fftw_execute(r2c_2d_plan);
    }
    // std::cout << "Forward 2D r2c: " << std::endl;
    // for(auto & stride : strides)
    // {
    //     print_complex(stride, dim_r_y, dim_c_z);
    // }

    // step 2: dirty communication
    for(int i=0; i<dim_c_y; ++i)
    {
        r_strides[i].resize(2 * dim_c_z * dim_c_x);
        for(int j=0; j<dim_c_x; ++j)
        {        
            std::move(strides[j].begin() + i * 2* dim_c_z, strides[j].begin() + (i+1) * 2 * dim_c_z, r_strides[i].begin() + j * 2 * dim_c_z);
        }
    }
    // std::cout << "Communication: " << std::endl;
    // for(auto & stride : r_strides)
    // {
    //     print_complex(stride, dim_r_x, dim_c_z);
    // }

    // step 3: strided c2c in x-direction
    // create fftw plans 
    for(int i=0; i<dim_c_y; ++i)
    {
        c2c_1d_plans[i] = fftw_plan_many_dft(1, &dim_c_x, dim_c_z,
                             reinterpret_cast<fftw_complex*>(r_strides[i].data()), NULL,
                             dim_c_z, 1,
                             reinterpret_cast<fftw_complex*>(r_strides[i].data()), NULL,
                             dim_c_z, 1,
                             FFTW_FORWARD, FFTW_ESTIMATE);
    }
    // launch plans
    for(auto & c2c_1d_plan : c2c_1d_plans)
    {
        fftw_execute(c2c_1d_plan);
    }
    std::cout << "3D FFT: " << std::endl;
    for(auto & stride : r_strides)
    {
        print_complex(stride, dim_r_x, dim_c_z);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // backward
    // step 1: strided c2c in x-direction
    // create fftw plans 
    for(int i=0; i<dim_c_y; ++i)
    {
        c2c_1d_plans[i] = fftw_plan_many_dft(1, &dim_c_x, dim_c_z,
                             reinterpret_cast<fftw_complex*>(r_strides[i].data()), NULL,
                             dim_c_z, 1,
                             reinterpret_cast<fftw_complex*>(r_strides[i].data()), NULL,
                             dim_c_z, 1,
                             FFTW_BACKWARD, FFTW_ESTIMATE);
    }
    // launch plans
    for(auto & c2c_1d_plan : c2c_1d_plans)
    {
        fftw_execute(c2c_1d_plan);
    }
    // std::cout << "Backward 1D c2c:" << std::endl;
    // for(auto & stride : r_strides)
    // {
    //     print_complex(stride, dim_r_x, dim_c_z);
    // }
    // step 2: dirty communication
    for(int j=0; j<dim_c_x; ++j)
    {        
        strides[j].resize(2 * dim_c_z * dim_c_x);
        for(int i=0; i<dim_c_y; ++i)
        {
                std::move(r_strides[i].begin() + j * 2* dim_c_z, r_strides[i].begin() + (j+1) * 2 * dim_c_z, strides[j].begin() + i * 2 * dim_c_z);
        
        }
    }
    // std::cout << "Communication:" << std::endl;
    // for(auto & stride : strides)
    // {
    //     print_complex(stride, dim_r_x, dim_c_z);
    // }
    // step 3: c2r in y- and z-direction
    // create fftw plans 
    for(int i=0; i<dim_c_x; ++i)
    {     
        r2c_2d_plans[i] = fftw_plan_dft_c2r_2d(dim_r_y, dim_r_z,
                                                reinterpret_cast<fftw_complex*>(strides[i].data()),
                                                strides[i].data(),
                                                FFTW_ESTIMATE);
    }
    // launch plans
    for(auto & c2r_2d_plan : r2c_2d_plans)
    {
        fftw_execute(c2r_2d_plan);
    }
    std::cout << "3D IFFT: " << std::endl;
    for(auto & stride : strides)
    {
        print_real(stride, dim_r_x, dim_r_y, dim_c_z,1);
    }

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