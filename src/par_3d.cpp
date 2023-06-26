#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>

#include <fftw3.h>

template <typename T>
using strides_loc = std::vector<std::vector<T>>;

using plans_loc = std::vector<fftw_plan>;

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

///
int hpx_main(hpx::program_options::variables_map& vm)
{
    ///////////////////////////////////////////////////////////////////////////////////
    // 3D
    // dimensions
    const int dim_r_z = 4; 
    const int dim_r_y = 4;
    const int dim_r_x = 4; 

    const int dim_c_z = dim_r_z / 2 + 1;
    const int dim_c_y = dim_r_y;
    const int dim_c_x = dim_r_x;
    
    const int N_r = dim_r_x * dim_r_y;
    const int N_c = 2 * dim_c_z * dim_c_y;

    // parallel 
    const int n_locs = 2;
    // assume dim_r_x > n_locs and dim_r_y > n_locs
    const int loc_size_x = dim_c_x / n_locs;
    const int loc_size_y = dim_c_y / n_locs;
    
    // strides distributed across localities
    std::vector<strides_loc<double>> data_locs(n_locs);
    // placeholder for rearrange
    std::vector<strides_loc<double>> r_data_locs(n_locs);
    // FFTW plans
    std::vector<plans_loc> r2c_plans_locs(n_locs);
    std::vector<plans_loc> c2r_plans_locs(n_locs);
    std::vector<plans_loc> f_c2c_plans_locs(n_locs);
    std::vector<plans_loc> b_c2c_plans_locs(n_locs);

    //////////////////////////////////////////////////////////////////////////////////////
    // intialize with from 0 with spacers
    for(auto & strides : data_locs) // parallel loop over n_locs
    {
        strides.resize(loc_size_x);
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
    // print 
    // for(auto & strides : data_locs)
    // {
    //     for(auto & stride : strides)
    //     {
    //         print_real(stride, dim_r_x, dim_r_y, dim_c_z);
    //     }
    // }

    //////////////////////////////////////////////////////////////////////////////////////
    // forward
    // step 1: r2c in z- and y-direction
    // create fftw plans for r2c in z- and y-direction
    for(int loc = 0; loc < n_locs; ++loc) // parallel loop over n_locs
    {
        r2c_plans_locs[loc].resize(loc_size_x);
        for(int i=0; i<loc_size_x; ++i)
        {     
            r2c_plans_locs[loc][i] = fftw_plan_dft_r2c_2d(dim_r_y, dim_r_z,
                                                    data_locs[loc][i].data(),
                                                    reinterpret_cast<fftw_complex*>(data_locs[loc][i].data()),
                                                    FFTW_ESTIMATE);
        }
    }
    // launch fftw plans
    for(auto & r2c_plans : r2c_plans_locs) // parallel loop over n_locs
    {
        for(auto & r2c_plan : r2c_plans)
        {
            fftw_execute(r2c_plan);
        }
    }
    // print
    // std::cout << "Forward 2D r2c: " << std::endl;
    // for(auto & strides : data_locs) // parallel loop over n_locs
    // {
    //     std::cout << "Locality:" << std::endl;
    //     for(auto & stride : strides)
    //     {
    //         print_complex(stride, dim_r_y, dim_c_z);
    //     }
    // }
    ////////////////////////////
    // step 2: dirty communication
    // rearrange 
    for(int loc = 0; loc < n_locs; ++loc) // parallel loop over n_locs
    {
        r_data_locs[loc].resize(dim_c_y);
        for(int i=0; i<dim_c_y; ++i)
        {
            r_data_locs[loc][i].resize(2 * dim_c_z * loc_size_x);
            for(int j=0; j<loc_size_x; ++j)
            { 
                std::cout << "Before: Data: " << data_locs[loc][j][0] << " Rdata: " << r_data_locs[loc][i][0] << std::endl;
                std::move(data_locs[loc][j].begin() + i * 2* dim_c_z, 
                          data_locs[loc][j].begin() + (i+1) * 2 * dim_c_z, 
                          r_data_locs[loc][i].begin() + j * 2 * dim_c_z);

                std::cout << "After: Data: " << data_locs[loc][j][0] << " Rdata: " << r_data_locs[loc][i][0] << std::endl;
            }
        }
        
    } 
    // print
    // std::cout << "Forward rearrange: " << std::endl;
    // for(auto & r_strides : r_data_locs) // parallel loop over n_locs
    // {
    //     std::cout << "Locality:" << std::endl;
    //     for(auto & stride : r_strides)
    //     {
    //         print_complex(stride, loc_size_x, dim_c_z);
    //     }
        
    // }
    // communicate
    for(int loc = 0; loc < n_locs; ++loc) // parallel loop over n_locs
    {
        data_locs[loc].resize(loc_size_y);
        // collect data from own Locality: 
        for(int i=0; i<loc_size_y; ++i)
        {
            data_locs[loc][i].resize(2*dim_c_z*dim_c_x);
            std::fill(data_locs[loc][i].begin(),data_locs[loc][i].end(),1.0);
   
            std::move(r_data_locs[loc][i + loc * loc_size_y].begin(), 
                      r_data_locs[loc][i + loc * loc_size_y].end(), 
                      data_locs[loc][i].begin() + loc * loc_size_x * 2 * dim_c_z);
        }  
        // collect data from other Locality:es
        for(int loc_r = 0; loc_r < n_locs; ++loc_r) // parallel loop over n_locs
        {
            if(loc_r != loc)
            {
                for(int i=0; i<loc_size_y; ++i)
                {
                    std::move(r_data_locs[loc_r][i + loc * loc_size_y].begin(), 
                              r_data_locs[loc_r][i + loc * loc_size_y].end(), 
                              data_locs[loc][i].begin() + loc_r * loc_size_x * 2 * dim_c_z);
                } 
            }
        }
    }
    // print
    // std::cout << "Forward communicate: " << std::endl;
    // for(auto & strides : data_locs) // parallel loop over n_locs
    // {
    //     std::cout << "Locality:" << std::endl;
    //     for(auto & stride : strides)
    //     {
    //         print_complex(stride, dim_c_x, dim_c_z);
    //     }
    // }
    /////////////////////////////////////////////
    // step 3: strided forward c2c in x-direction
    // create fftw plans for forward c2c in x-direction
    for(int loc = 0; loc < n_locs; ++loc) // parallel loop over n_locs
    {
        f_c2c_plans_locs[loc].resize(loc_size_y);
        for(int i=0; i<loc_size_y; ++i)
        {
            f_c2c_plans_locs[loc][i] = fftw_plan_many_dft(1, &dim_c_x, dim_c_z,
                                            reinterpret_cast<fftw_complex*>(data_locs[loc][i].data()), NULL,
                                            dim_c_z, 1,
                                            reinterpret_cast<fftw_complex*>(data_locs[loc][i].data()), NULL,
                                            dim_c_z, 1,
                                            FFTW_FORWARD, FFTW_ESTIMATE);
        }
        
    }
    // launch fftw plans
    for(auto & c2c_plans : f_c2c_plans_locs) // parallel loop over n_locs
    {
        for(auto & c2c_plan : c2c_plans)
        {
            fftw_execute(c2c_plan);
        }
    }
    // print
    // std::cout << "3D FFT: " << std::endl;
    // for(auto & strides : data_locs) // parallel loop over n_locs
    // {
    //     std::cout << "Locality:" << std::endl;
    //     for(auto & stride : strides)
    //     {
    //         print_complex(stride, dim_c_x, dim_c_z);
    //     }
        
    // }

    //////////////////////////////////////////////////////////////////////////////////////
    // backward
    // step 1: strided backward c2c in x-direction
    // create fftw plans for backward c2c in x-direction
    for(int loc = 0; loc < n_locs; ++loc) // parallel loop over n_locs
    {
        b_c2c_plans_locs[loc].resize(loc_size_y);
        for(int i=0; i<loc_size_y; ++i)
        {
            b_c2c_plans_locs[loc][i] = fftw_plan_many_dft(1, &dim_c_x, dim_c_z,
                                            reinterpret_cast<fftw_complex*>(data_locs[loc][i].data()), NULL,
                                            dim_c_z, 1,
                                            reinterpret_cast<fftw_complex*>(data_locs[loc][i].data()), NULL,
                                            dim_c_z, 1,
                                            FFTW_BACKWARD, FFTW_ESTIMATE);
        }
        
    }
    // launch fftw plans
    for(auto & c2c_plans : b_c2c_plans_locs) // parallel loop over n_locs
    {
        for(auto & c2c_plan : c2c_plans)
        {
            fftw_execute(c2c_plan);
        }
    }
    // print
    // std::cout << "Backward 1D c2c: " << std::endl;
    // for(auto & strides : data_locs) // parallel loop over n_locs
    // {
    //     std::cout << "Locality:" << std::endl;
    //     for(auto & stride : strides)
    //     {
    //         print_complex(stride, dim_c_x, dim_c_z);
    //     }
        
    // }
    //////////////////////////////
    // step 2: dirty communication
    // rearrange 
    for(int loc = 0; loc < n_locs; ++loc) // parallel loop over n_locs
    {
        r_data_locs[loc].resize(dim_c_x);
        for(int i=0; i<dim_c_x; ++i)
        {
            r_data_locs[loc][i].resize(2 * dim_c_z * loc_size_y);
            std::fill(r_data_locs[loc][i].begin(),r_data_locs[loc][i].end(),1.0);
            for(int j=0; j<loc_size_y; ++j)
            { 
                std::move(data_locs[loc][j].begin() + i * 2* dim_c_z, 
                          data_locs[loc][j].begin() + (i+1) * 2 * dim_c_z, 
                          r_data_locs[loc][i].begin() + j * 2 * dim_c_z);
            }
        }
    } 
    // print
    // std::cout << "Backward rearrange" << std::endl;
    // for(auto & strides : r_data_locs) // parallel loop over n_locs
    // {
    //     std::cout << "Locality:" << std::endl;
    //     for(auto & stride : strides)
    //     {
    //         print_complex(stride, loc_size_y, dim_c_z);
    //     }
        
    // }
    // communicate
    for(int loc = 0; loc < n_locs; ++loc) // parallel loop over n_locs
    {
        data_locs[loc].resize(loc_size_x);
        // collect data from own Locality: 
        for(int i=0; i<loc_size_x; ++i)
        {
            data_locs[loc][i].resize(2*dim_c_z*dim_c_x);
            std::fill(data_locs[loc][i].begin(),data_locs[loc][i].end(),1.0);
   
            std::move(r_data_locs[loc][i + loc * loc_size_x].begin(), 
                      r_data_locs[loc][i + loc * loc_size_x].end(), 
                      data_locs[loc][i].begin() + loc * loc_size_y * 2 * dim_c_z);
        }  
        // collect data from other Locality:es
        for(int loc_r = 0; loc_r < n_locs; ++loc_r) // parallel loop over n_locs
        {
            if(loc_r != loc)
            {
                for(int i=0; i<loc_size_x; ++i)
                {
                    std::move(r_data_locs[loc_r][i + loc * loc_size_x].begin(), 
                              r_data_locs[loc_r][i + loc * loc_size_x].end(), 
                              data_locs[loc][i].begin() + loc_r * loc_size_y * 2 * dim_c_z);
                } 
            }
        }
    }
    // print
    // std::cout << "Backward communicate" << std::endl;
    // for(auto & strides : data_locs) // parallel loop over n_locs
    // {
    //     std::cout << "Locality:" << std::endl;
    //     for(auto & stride : strides)
    //     {
    //         print_complex(stride, dim_c_y, dim_c_z);
    //     }
        
    // }
    ////////////////////////////////////
    // step 3: c2r in y- and z-direction
    // create fftw plans for c2r in z- and y-direction
    for(int loc = 0; loc < n_locs; ++loc) // parallel loop over n_locs
    {
        c2r_plans_locs[loc].resize(loc_size_x);
        for(int i=0; i<loc_size_x; ++i)
        {     
            c2r_plans_locs[loc][i] = fftw_plan_dft_c2r_2d(dim_r_y, dim_r_z,
                                        reinterpret_cast<fftw_complex*>(data_locs[loc][i].data()),
                                        data_locs[loc][i].data(),
                                        FFTW_ESTIMATE);
        }
    }
    // launch fftw plans
    for(auto & c2r_plans : c2r_plans_locs) // parallel loop over n_locs
    {
        for(auto & c2r_plan : c2r_plans)
        {
            fftw_execute(c2r_plan);
        }
    }
    // print
    std::cout << "3D IFFT: " << std::endl;
    for(auto & strides : data_locs) // parallel loop over n_locs
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