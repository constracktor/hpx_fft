#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <complex>

#include <fftw3.h>

void print_real(const std::vector<double>& input, int dim_r_x, int dim_r_y, int dim_r_z, int scaling = 0)
{
    double factor = 1.0;
    if (scaling)
    {
        factor = factor / (dim_r_x * dim_r_y * dim_r_z);
    }
    
    for(int k=0; k<dim_r_x; ++k)
    {
        std::cout << "dim = " << k << std::endl;
        for(int j=0; j<dim_r_y; ++j)
        {
            for(int i=0; i<dim_r_z+2; ++i)
            {
                std::cout <<  input[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i] * factor << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void print_complex(const std::vector<double>& input,int dim_r_x, int dim_r_y, int dim_r_z)
{
    for(int k=0; k<dim_r_x; ++k)
    {
        std::cout << "dim = " << k << std::endl;
        for(int j=0; j<dim_r_y; ++j)
        {
            for(int i=0; i<dim_r_z+2; i=i+2)
            {
                //std::cout <<  input[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i] * factor << " ";
                 std::cout << "(" << input[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i] << " " << input[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i +1]  << ")";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

// void print_complex_t(const std::vector<double>& input, int dim_r_x, int dim_r_y, int dim_r_z)
// {
//     // for(int i=0; i<dim_r; ++i)
//     // {
//     //     for(int j=0; j<dim_r + 2; j=j+2)
//     //     {
//     //         std::cout << "(" << input[(dim_r+2)*i + j] << " " << input[(dim_r+2)*i + j +1]  << ")";
//     //     }
//     //     std::cout << std::endl;
//     // }
//     // std::cout << std::endl;

//     // for(int i=0; i<dim_r/2+1; ++i)
//     // {
//     //     for(int j=0; j<2*dim_r; j=j+2)
//     //     {
//     //         std::cout << "(" << input[(2*dim_r)*i + j] << " " << input[(2*dim_r)*i + j +1]  << ")";
//     //     }
//     //     std::cout << std::endl;
//     // }
//     // std::cout << std::endl;

//     for(int k=0; k<dim_r_x; ++k)
//     {
//         std::cout << "dim = " << k << std::endl;
//         for(int j=0; j<dim_r_z/2 +1; ++j)
//         {
//             for(int i=0; i<dim_r_y; i=i+2)
//             {
//                 //std::cout <<  input[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i] * factor << " ";
//                  std::cout << "(" << input[(2*dim_r_z)*dim_r_y*k + (2*dim_r_z)*j + i] << " " << input[(2*dim_r_z)*dim_r_y*k + (2*dim_r_z)*j + i +1]  << ")";
//             }
//             std::cout << std::endl;
//         }
//         std::cout << std::endl;
//     }
// }

int hpx_main(hpx::program_options::variables_map& vm)
{
    ///////////////////////////////////////////////////////////////////////////////////
    // 3D
    int dim_r_z = 4;
    int dim_r_y = 4;
    int dim_r_x = 4;
    int N_r = dim_r_x * dim_r_y * dim_r_z;

    //int dim_c_y = dim_r_y / 2 + 1;
    //int dim_c_x = dim_r_x / 2 + 1;
    int dim_c_z = dim_r_z / 2 + 1;

    int N = 2 * dim_c_z * dim_r_y * dim_r_x;

    std::vector<double> input_1, input_2;
    input_1.resize(N);
    input_2.resize(N);
    

    // intialize with from 0 with spacers
    for(int k=0; k<dim_r_x; ++k)
    {
        for(int j=0; j<dim_r_y; ++j)
        {
            for(int i=0; i<dim_r_z; ++i)
            {
                input_1[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i] = ((dim_r_z)*dim_r_y*k + (dim_r_z)*j + i) % (N_r/2);
                input_2[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i] = ((dim_r_z)*dim_r_y*k + (dim_r_z)*j + i) % (N_r/2);
            }
        }
    }

    std::cout << "Input: " << std::endl;
    //print_real(input_1, dim_r_x, dim_r_y, dim_r_z);
    
    ////////////////////////////////////////////////////////////////////////////////////
    // use fftw function
    ///////////
    // forward
    fftw_plan plan_r2c_3d = fftw_plan_dft_r2c_3d(dim_r_z, dim_r_y, dim_r_x,
                                                input_1.data(),
                                                reinterpret_cast<fftw_complex*>(input_1.data()),
                                                FFTW_ESTIMATE);
    fftw_execute(plan_r2c_3d);

    std::cout << "FFT: FFTW 3D" << std::endl;
   //print_complex(input_1,dim_r_x,dim_r_y,dim_r_z);

    // backward
    fftw_plan plan_c2r_3d = fftw_plan_dft_c2r_3d(dim_r_z, dim_r_y, dim_r_x,
                                                reinterpret_cast<fftw_complex*>(input_1.data()),
                                                input_1.data(),
                                                FFTW_ESTIMATE);
    fftw_execute(plan_c2r_3d);

    std::cout << "IFFT: FFTW 3D" << std::endl;
    //print_real(input_1, dim_r_x, dim_r_y, dim_r_z,1);

    ////////////////////////////////////////////////////////////////////////////////////
    // strided fft
    /////////
    // forward step one (z direction)
    //int n[] = {10}; 
    int rank = 1;
    const int* n= new int(dim_r_z);
    int howmany = dim_r_x*dim_r_y;
    int idist = dim_r_z + 2, odist = dim_r_z/2 + 1; /* the distance in memory between the first element of the first array and the first element of the second array */
    int istride = 1, ostride = 1; /* array is contiguous in memory */


    fftw_plan plan_2_r2c_3d = fftw_plan_many_dft_r2c(rank, n, howmany,
                            input_2.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_2_r2c_3d);

    print_complex(input_2, dim_r_x, dim_r_y, dim_r_z);


    // forward step two
    const int* n1= new int(dim_r_x);
    howmany = dim_c_z * dim_r_x;
    istride = dim_c_z * dim_r_y;
    ostride = dim_c_z * dim_r_y;
    idist = 1;
    odist = 1;
    fftw_plan plan_2_c2c_x = fftw_plan_many_dft(rank, n1, howmany,
                    reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                    istride, idist,
                    reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                    ostride, odist, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan_2_c2c_x);

    print_complex(input_2, dim_r_x, dim_r_y, dim_r_z);

    // forward step three
    // // rearrange
    // std::vector<double> arange;
    // arange.resize(N);
    // for(int k=0; k<dim_r_x; ++k)
    // {
    //     //std::cout << "dim = " << k << std::endl;
    //     for(int j=0; j<dim_r_y; ++j)
    //     {
    //         for(int i=0; i<dim_r_z+2; i=i+2)
    //         {
    //             arange[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*i + j] = input_2[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i];
    //             arange[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*i + j + 1] = input_2[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i + 1];
    //             //std::cout << "(" << arange[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i] << " " << arange[(dim_r_z+2)*dim_r_y*k + (dim_r_z+2)*j + i +1]  << ")";
    //         }
    //         std::cout << std::endl;
    //     }
    //     std::cout << std::endl;
    // }
    // print_complex_t(input_2, dim_r_x, dim_r_y, dim_r_z);

    // const int* n2= new int(dim_r_y);
    // howmany = 12;//dim_r_y;
    // istride = 12;//dim_c_z;
    // ostride = 12;//dim_c_z;
    // idist = 1;
    // odist = 1;
    // fftw_plan plan_3_c2c_y = fftw_plan_many_dft(rank, n2, howmany,
    //                 reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
    //                 istride, idist,
    //                 reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
    //                 ostride, odist, FFTW_FORWARD, FFTW_ESTIMATE);
    // fftw_execute(plan_3_c2c_y);

    //print_complex(input_2, dim_r_x, dim_r_y, dim_r_z);

    
    // backward step one
    /////////
    // backward step two
    howmany = dim_c_z * dim_r_x;
    istride = dim_c_z * dim_r_y;
    ostride = dim_c_z * dim_r_y;
    idist = 1;
    odist = 1;
    fftw_plan plan_2_c2c_x_b = fftw_plan_many_dft(rank, n1, howmany,
                    reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                    istride, idist,
                    reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                    ostride, odist, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan_2_c2c_x_b);

    print_complex(input_2, dim_r_x, dim_r_y, dim_r_z);
    
    /////////
    // backward step three
    howmany = dim_r_x*dim_r_y;
    idist = dim_r_z + 2;
    odist = dim_r_z/2 + 1; /* the distance in memory between the first element of the first array and the first element of the second array */
    istride = 1;
    ostride = 1; /* array is contiguous in memory */


    fftw_plan plan_2_c2r_3d = fftw_plan_many_dft_c2r(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                            istride, idist,
                            input_2.data(), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_2_c2r_3d);
    std::cout << "IFFT: Strided 3D" << std::endl;
    print_real(input_1, dim_r_x, dim_r_y, dim_r_z,1);

                // fft_plan_Fourier_forward_z = FFTW_PLAN_MANY_DFT_R2C(1, Nz_dims, field.maxrx * field.maxry,
                //     reinterpret_cast<Real*>(RA.data()), NULL,
                //     1, field.Nz+2,
                //     reinterpret_cast<FFTW_Complex*>(RA.data()), NULL,
                //     1, field.Nz/2+1,
                //     FFTW_PLAN_FLAG);

                // fft_plan_Fourier_forward_y = FFTW_PLAN_MANY_DFT(1, Ny_dims, field.maxiz,
                //     reinterpret_cast<FFTW_Complex*>(IA.data()), NULL,
                //     field.maxiz, 1,
                //     reinterpret_cast<FFTW_Complex*>(IA.data()), NULL,
                //     field.maxiz, 1,
                //     FFTW_FORWARD, FFTW_PLAN_FLAG);

                // fft_plan_Fourier_forward_x = FFTW_PLAN_MANY_DFT(1, Nx_dims, field.maxfy * field.maxfz,
                //     reinterpret_cast<FFTW_Complex*>(FA.data()), NULL,
                //     field.maxfy * field.maxfz, 1,
                //     reinterpret_cast<FFTW_Complex*>(FA.data()), NULL,
                //     field.maxfy * field.maxfz, 1,
                //     FFTW_FORWARD, FFTW_PLAN_FLAG);
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