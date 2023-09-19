#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>

#include <fftw3.h>

void print_real(const std::vector<double>& input, int dim_r_y, int dim_c_z, int scaling = 0)
{
    double factor = 1.0;
    if (scaling)
    {
        factor = factor / (dim_r_y * (2* dim_c_z - 2));
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

std::vector<double> rearrange_complex(std::vector<double> input,  int dim_r_y, int dim_c_z, int np)
{
    std::vector<double> output;

    for(int p=0;p<np;++p)
    {
        for(int i=0; i<dim_r_y; ++i)
        {
            int start = p*2*dim_c_z/np;
            int stop = (p+1)*2*dim_c_z/np-1;
            int dist = p*dim_c_z/np;

            for(int j=start; j<stop; j=j+2)
            {
                int index= 2*dim_c_z*j + i;
                output.push_back(input[(2*dim_c_z)*i + j]);
                output.push_back(input[(2*dim_c_z)*i + j +1]); 

                //std::cout << "(" << input[(2*dim_c_z)*i + j] << " " <<input[(2*dim_c_z)*i + j +1]  << ")";
            } 
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    return output;
}


int hpx_main(hpx::program_options::variables_map& vm)
{
    ///////////////////////////////////////////////////////////////////////////////////
    // 2D
    int dim_r_y = 4;
    int dim_r_z = 6;
    int dim_c_z = dim_r_z/2 + 1;

    int np_y =2;

    int N = 2 * dim_c_z * dim_r_y;

    std::vector<double> input_1, input_2, input_3;
    input_1.resize(N);
    input_2.resize(N/np_y);
    input_3.resize(N/np_y);
    // intialize with from 0 with spacers
    for(int i=0; i<dim_r_y; ++i)
    {
        for(int j=0; j<dim_r_z; ++j)
        {
            input_1[(2 * dim_c_z)*i + j] = (dim_r_z)*i + j;
        }
    }

    std::cout << "Input: " << std::endl;
    print_real(input_1,dim_r_y, dim_c_z);
    
    ////////////////////////////////////////////////////////////////////////////////////
    // strided fft
    /////////
    int rank = 1;
    const int* n= new int(dim_r_z);
    int howmany = dim_r_y;
    int idist = 2*dim_c_z, odist = dim_c_z; /* the distance in memory between the first element of the first array and the first element of the second array */
    int istride = 1, ostride = 1; /* array is contiguous in memory */
    // forward step one
    fftw_plan plan_3_r2c = fftw_plan_many_dft_r2c(rank, n, howmany,
                            input_1.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_1.data()), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_3_r2c);

    /////////
    // forward step two
    const int* n1= new int(dim_r_y);
    howmany = dim_c_z;
    istride = dim_c_z;
    ostride = dim_c_z;
    idist = 1;
    odist = 1;
    fftw_plan plan_3_c2c = fftw_plan_many_dft(rank, n1, howmany,
                    reinterpret_cast<fftw_complex*>(input_1.data()), NULL,
                    istride, idist,
                    reinterpret_cast<fftw_complex*>(input_1.data()), NULL,
                    ostride, odist, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan_3_c2c);

    std::cout << "FFT: Strided" << std::endl;
    print_complex(input_1,dim_r_y,dim_c_z);

    /////////
    // backward step one
    fftw_plan plan_3_c2c_b = fftw_plan_many_dft(rank, n1, howmany,
                reinterpret_cast<fftw_complex*>(input_1.data()), NULL,
                istride, idist,
                reinterpret_cast<fftw_complex*>(input_1.data()), NULL,
                ostride, odist, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan_3_c2c_b);

    /////////
    // backward step two
    howmany = dim_r_z;
    idist = dim_c_z;
    odist = 2*dim_c_z;
    istride = 1;
    ostride = 1;
    fftw_plan plan_3_c2r = fftw_plan_many_dft_c2r(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(input_1.data()), NULL,
                            istride, idist,
                            input_1.data(), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_3_c2r);

    std::cout << "IFFT: Strided" << std::endl;
    print_real(input_1,dim_r_y, dim_c_z,1);

    ////////////////////////////////////////////////////////////////////////////////////
    // parallel strided fft

    // intialize with from 0 with spacers
    for(int i=0; i<dim_r_y/np_y; ++i)
    {
        for(int j=0; j<dim_r_z; ++j)
        {
            input_2[(2 * dim_c_z)*i + j] = (dim_r_z)*i + j;
            input_3[(2 * dim_c_z)*i + j] = (dim_r_z * dim_r_y/np_y)+(dim_r_z)*i + j;
        }
    }
    print_real(input_2,dim_r_y/np_y, dim_c_z);
    print_real(input_3,dim_r_y/np_y, dim_c_z);

    const int* n2= new int(dim_r_z);
    howmany = dim_r_y/np_y;
    idist = 2*dim_c_z;
    odist = dim_c_z; /* the distance in memory between the first element of the first array and the first element of the second array */
    istride = 1;
    ostride = 1; /* array is contiguous in memory */
    // forward step one
    fftw_plan plan_3_r2c_1 = fftw_plan_many_dft_r2c(rank, n2, howmany,
                            input_2.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_3_r2c_1);

    fftw_plan plan_3_r2c_2 = fftw_plan_many_dft_r2c(rank, n2, howmany,
                            input_3.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                            ostride, odist, FFTW_ESTIMATE);
    fftw_execute(plan_3_r2c_2);

    print_complex(input_2,dim_r_y/np_y,dim_c_z);
    print_complex(input_3,dim_r_y/np_y,dim_c_z);

    // communicate
    // rearrange
    input_2 = rearrange_complex(input_2,dim_r_y/np_y,dim_c_z,np_y);
    input_3 = rearrange_complex(input_3,dim_r_y/np_y,dim_c_z,np_y);
    
    //print_complex(input_2,dim_r_y/np_y,dim_c_z);
    //print_complex(input_3,dim_r_y/np_y,dim_c_z);

    // communicate
    std::vector<double> test_1,test_2;
    int size = 2 * dim_c_z * dim_r_y / np_y / np_y; 
    std::copy(input_2.begin(), input_2.begin()+size, std::back_inserter(test_1));
    std::copy(input_3.begin(), input_3.begin()+size, std::back_inserter(test_1));

    std::copy(input_2.begin()+size, input_2.end(), std::back_inserter(test_2));
    std::copy(input_3.begin()+size, input_3.end(), std::back_inserter(test_2));
    print_complex(test_1,dim_r_y/np_y,dim_c_z);
    print_complex(test_2,dim_r_y/np_y,dim_c_z);
    // same data 

    //forward step two
    const int* n3= new int(dim_r_y);
    howmany = dim_c_z/np_y;
    idist = 1;
    odist = 1; /* the distance in memory between the first element of the first array and the first element of the second array */
    istride = dim_c_z/np_y;
    ostride = dim_c_z/np_y; /* array is contiguous in memory */
    fftw_plan plan_3_c2c_1 = fftw_plan_many_dft(rank, n3, howmany,
                        reinterpret_cast<fftw_complex*>(test_1.data()), NULL,
                        istride, idist,
                        reinterpret_cast<fftw_complex*>(test_1.data()), NULL,
                        ostride, odist, FFTW_FORWARD,FFTW_ESTIMATE);
    fftw_execute(plan_3_c2c_1);

    fftw_plan plan_3_c2c_2 = fftw_plan_many_dft(rank, n3, howmany,
                        reinterpret_cast<fftw_complex*>(test_2.data()), NULL,
                        istride, idist,
                        reinterpret_cast<fftw_complex*>(test_2.data()), NULL,
                        ostride, odist, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan_3_c2c_2);


    print_complex(test_1,dim_r_y/np_y,dim_c_z);
    print_complex(test_2,dim_r_y/np_y,dim_c_z);

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