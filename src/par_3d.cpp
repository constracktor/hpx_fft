#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>

#include <fftw3.h>

template <typename T>
using strides_proc = std::vector<std::vector<T>>;

using plans_proc = std::vector<fftw_plan>;

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
    // dimensions
    const int dim_r_z = 4; 
    const int dim_r_y = 16;
    const int dim_r_x = 8; 

    const int dim_c_z = dim_r_z / 2 + 1;
    const int dim_c_y = dim_r_y;
    const int dim_c_x = dim_r_x;
    
    const int N_r = dim_r_x * dim_r_y;
    const int N_c = 2 * dim_c_z * dim_c_y;

    // parallel 
    const int n_procs = 8;
    // assume dim_r_x > n_procs
    const int proc_size = dim_c_x / n_procs;
    const int proc_size_r = dim_c_y / n_procs;
    
    // strides distributed across processes
    std::vector<strides_proc<double>> data_procs(n_procs);
    std::vector<strides_proc<double>> r_data_procs(n_procs);

    std::vector<plans_proc> r2c_plans_procs(n_procs);
    std::vector<plans_proc> c2r_plans_procs(n_procs);
    std::vector<plans_proc> f_c2c_plans_procs(n_procs);
    std::vector<plans_proc> b_c2c_plans_procs(n_procs);

    //////////////////////////////////////////////////////////////////////////////////////
    // intialize with from 0 with spacers
    for(auto & strides : data_procs) // parallel loop over n_procs
    {
        strides.resize(proc_size);
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
    } 
    
    for(auto & strides : data_procs) // parallel loop over n_procs
    {
        for(auto & stride : strides)
        {
            print_real(stride, dim_r_x, dim_r_y, dim_c_z);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // forward
    // step 1: r2c in z- and y-direction
    // create fftw plans for r2c in z- and y-direction
    for(int proc = 0; proc < n_procs; ++proc) // parallel loop over n_procs
    {
        r2c_plans_procs[proc].resize(proc_size);
        for(int i=0; i<proc_size; ++i)
        {     
            r2c_plans_procs[proc][i] = fftw_plan_dft_r2c_2d(dim_r_y, dim_r_z,
                                                    data_procs[proc][i].data(),
                                                    reinterpret_cast<fftw_complex*>(data_procs[proc][i].data()),
                                                    FFTW_ESTIMATE);
        }
    }
    // launch fftw plans
    for(auto & r2c_plans : r2c_plans_procs) // parallel loop over n_procs
    {
        for(auto & r2c_plan : r2c_plans)
        {
            fftw_execute(r2c_plan);
        }
    }
    std::cout << "Forward 2D r2c: " << std::endl;
    for(auto & strides : data_procs) // parallel loop over n_procs
    {
        std::cout << "Process" << std::endl;
        for(auto & stride : strides)
        {
            print_complex(stride, dim_r_y, dim_c_z);
        }
    }
    ////////////////////////////
    // step 2: dirty communication
    // rearrange 
    for(int proc = 0; proc < n_procs; ++proc) // parallel loop over n_procs
    {
        r_data_procs[proc].resize(dim_c_y);
        for(int i=0; i<dim_c_y; ++i)
        {
            r_data_procs[proc][i].resize(2 * dim_c_z * proc_size);
            for(int j=0; j<proc_size; ++j)
            { 
                std::move(data_procs[proc][j].begin() + i * 2* dim_c_z, 
                          data_procs[proc][j].begin() + (i+1) * 2 * dim_c_z, 
                          r_data_procs[proc][i].begin() + j * 2 * dim_c_z);
            }
        }
    } 
    std::cout << "Forward rearrange: " << std::endl;
    for(auto & r_strides : r_data_procs) // parallel loop over n_procs
    {
        std::cout << "Process" << std::endl;
        for(auto & stride : r_strides)
        {
            print_complex(stride, proc_size, dim_c_z);
        }
        
    }
    // communicate
    for(int proc = 0; proc < n_procs; ++proc) // parallel loop over n_procs
    {
        data_procs[proc].resize(proc_size_r);
        // collect data from own process 
        for(int i=0; i<proc_size_r; ++i)
        {
            data_procs[proc][i].resize(2*dim_c_z*dim_c_x);
            std::fill(data_procs[proc][i].begin(),data_procs[proc][i].end(),1.0);
   
            std::move(r_data_procs[proc][i + proc * proc_size_r].begin(), 
                      r_data_procs[proc][i + proc * proc_size_r].end(), 
                      data_procs[proc][i].begin() + proc * proc_size * 2 * dim_c_z);
        }  
        // collect data from other processes
        for(int proc_r = 0; proc_r < n_procs; ++proc_r) // parallel loop over n_procs
        {
            if(proc_r != proc)
            {
                for(int i=0; i<proc_size_r; ++i)
                {
                    std::move(r_data_procs[proc_r][i + proc * proc_size_r].begin(), 
                              r_data_procs[proc_r][i + proc * proc_size_r].end(), 
                              data_procs[proc][i].begin() + proc_r * proc_size * 2 * dim_c_z);
                } 
            }
        }
    }
    std::cout << "Forward communicate: " << std::endl;
    for(auto & strides : data_procs) // parallel loop over n_procs
    {
        std::cout << "Process" << std::endl;
        for(auto & stride : strides)
        {
            print_complex(stride, dim_c_x, dim_c_z);
        }
    }
    /////////////////////////////////////////////
    // step 3: strided forward c2c in x-direction
    // create fftw plans for forward c2c in x-direction
    for(int proc = 0; proc < n_procs; ++proc) // parallel loop over n_procs
    {
        f_c2c_plans_procs[proc].resize(proc_size_r);
        for(int i=0; i<proc_size_r; ++i)
        {
            f_c2c_plans_procs[proc][i] = fftw_plan_many_dft(1, &dim_c_x, dim_c_z,
                                            reinterpret_cast<fftw_complex*>(data_procs[proc][i].data()), NULL,
                                            dim_c_z, 1,
                                            reinterpret_cast<fftw_complex*>(data_procs[proc][i].data()), NULL,
                                            dim_c_z, 1,
                                            FFTW_FORWARD, FFTW_ESTIMATE);
        }
        
    }
    // launch fftw plans
    for(auto & c2c_plans : f_c2c_plans_procs) // parallel loop over n_procs
    {
        for(auto & c2c_plan : c2c_plans)
        {
            fftw_execute(c2c_plan);
        }
    }
    std::cout << "3D FFT: " << std::endl;
    for(auto & strides : data_procs) // parallel loop over n_procs
    {
        std::cout << "Process" << std::endl;
        for(auto & stride : strides)
        {
            print_complex(stride, dim_c_x, dim_c_z);
        }
        
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // backward
    // step 1: strided backward c2c in x-direction
    // create fftw plans for backward c2c in x-direction
    for(int proc = 0; proc < n_procs; ++proc) // parallel loop over n_procs
    {
        b_c2c_plans_procs[proc].resize(proc_size_r);
        for(int i=0; i<proc_size_r; ++i)
        {
            b_c2c_plans_procs[proc][i] = fftw_plan_many_dft(1, &dim_c_x, dim_c_z,
                                            reinterpret_cast<fftw_complex*>(data_procs[proc][i].data()), NULL,
                                            dim_c_z, 1,
                                            reinterpret_cast<fftw_complex*>(data_procs[proc][i].data()), NULL,
                                            dim_c_z, 1,
                                            FFTW_BACKWARD, FFTW_ESTIMATE);
        }
        
    }
    // launch fftw plans
    for(auto & c2c_plans : b_c2c_plans_procs) // parallel loop over n_procs
    {
        for(auto & c2c_plan : c2c_plans)
        {
            fftw_execute(c2c_plan);
        }
    }
    std::cout << "Backward 1D c2c: " << std::endl;
    for(auto & strides : data_procs) // parallel loop over n_procs
    {
        std::cout << "Process" << std::endl;
        for(auto & stride : strides)
        {
            print_complex(stride, dim_c_x, dim_c_z);
        }
        
    }
    //////////////////////////////
    // step 2: dirty communication
    // rearrange 
    for(int proc = 0; proc < n_procs; ++proc) // parallel loop over n_procs
    {
        r_data_procs[proc].resize(dim_c_x);
        for(int i=0; i<dim_c_x; ++i)
        {
            r_data_procs[proc][i].resize(2 * dim_c_z * proc_size_r);
            std::fill(r_data_procs[proc][i].begin(),r_data_procs[proc][i].end(),1.0);
            for(int j=0; j<proc_size_r; ++j)
            { 
                std::move(data_procs[proc][j].begin() + i * 2* dim_c_z, 
                          data_procs[proc][j].begin() + (i+1) * 2 * dim_c_z, 
                          r_data_procs[proc][i].begin() + j * 2 * dim_c_z);
            }
        }
    } 
    std::cout << "Backward rearrange" << std::endl;
    for(auto & strides : r_data_procs) // parallel loop over n_procs
    {
        std::cout << "Process" << std::endl;
        for(auto & stride : strides)
        {
            print_complex(stride, proc_size_r, dim_c_z);
        }
        
    }
    // communicate
    for(int proc = 0; proc < n_procs; ++proc) // parallel loop over n_procs
    {
        data_procs[proc].resize(proc_size);
        // collect data from own process 
        for(int i=0; i<proc_size; ++i)
        {
            data_procs[proc][i].resize(2*dim_c_z*dim_c_x);
            std::fill(data_procs[proc][i].begin(),data_procs[proc][i].end(),1.0);
   
            std::move(r_data_procs[proc][i + proc * proc_size].begin(), 
                      r_data_procs[proc][i + proc * proc_size].end(), 
                      data_procs[proc][i].begin() + proc * proc_size_r * 2 * dim_c_z);
        }  
        // collect data from other processes
        for(int proc_r = 0; proc_r < n_procs; ++proc_r) // parallel loop over n_procs
        {
            if(proc_r != proc)
            {
                for(int i=0; i<proc_size; ++i)
                {
                    std::move(r_data_procs[proc_r][i + proc * proc_size].begin(), 
                              r_data_procs[proc_r][i + proc * proc_size].end(), 
                              data_procs[proc][i].begin() + proc_r * proc_size_r * 2 * dim_c_z);
                } 
            }
        }
    }
    std::cout << "Backward communicate" << std::endl;
    for(auto & strides : data_procs) // parallel loop over n_procs
    {
        std::cout << "Process" << std::endl;
        for(auto & stride : strides)
        {
            print_complex(stride, dim_c_y, dim_c_z);
        }
        
    }
    ////////////////////////////////////
    // step 3: c2r in y- and z-direction
    // create fftw plans for c2r in z- and y-direction
    for(int proc = 0; proc < n_procs; ++proc) // parallel loop over n_procs
    {
        c2r_plans_procs[proc].resize(proc_size);
        for(int i=0; i<proc_size; ++i)
        {     
            c2r_plans_procs[proc][i] = fftw_plan_dft_c2r_2d(dim_r_y, dim_r_z,
                                        reinterpret_cast<fftw_complex*>(data_procs[proc][i].data()),
                                        data_procs[proc][i].data(),
                                        FFTW_ESTIMATE);
        }
    }
    // launch fftw plans
    for(auto & c2r_plans : c2r_plans_procs) // parallel loop over n_procs
    {
        for(auto & c2r_plan : c2r_plans)
        {
            fftw_execute(c2r_plan);
        }
    }
    std::cout << "3D IFFT: " << std::endl;
    for(auto & strides : data_procs) // parallel loop over n_procs
    {
        for(auto & stride : strides)
        {
            print_real(stride, dim_r_x, dim_r_y, dim_c_z,1);
        }
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