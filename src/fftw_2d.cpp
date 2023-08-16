#include <iostream>
#include <vector>
// HPX
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>
// store time measurement
#include <chrono>
#include <string>
#include <map>
// basic file operations
#include <iostream>
#include <fstream>
using namespace std;
// FFTW
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
    auto t = std::chrono::steady_clock();
    using duration = std::chrono::duration<double, std::micro >;
    ///////////////////////////////////////////////////////////////////////////////////
    std::string plan_flag = vm["f"].as<std::string>();
    unsigned FFTW_PLAN_FLAG = FFTW_ESTIMATE;
    if( plan_flag == "measure" )
    {
        FFTW_PLAN_FLAG = FFTW_MEASURE;
    }
    else if ( plan_flag == "patient")
    {
        FFTW_PLAN_FLAG = FFTW_PATIENT;
    }
    else if ( plan_flag == "exhaustive")
    {
        FFTW_PLAN_FLAG = FFTW_EXHAUSTIVE;
    }

    std::uint64_t loop = vm["l"].as<std::uint64_t>();    // Number of loops to average
    std::uint64_t dim_r = vm["n"].as<std::uint64_t>();    // Number of grid points.
    std::cout << "Size of matrix: " << dim_r << " x " << dim_r << std::endl;
    // 2D
    //int dim_r = 1024;
    int dim_c = dim_r/2 + 1;

    int N = 2 * dim_c * dim_r;

    std::vector<double> input_1, input_2, input_3;
    input_1.resize(N);
    input_2.resize(N);
    input_3.resize(N);
    std::vector<double> transpose;
    transpose.resize(N);

    std::vector<std::vector<double>> input_4;
    input_4.resize(dim_r);
    for(int i=0; i<dim_r; ++i)
    {
        input_4[i].resize(2*dim_c);
    }
    std::vector<std::vector<double>> transposes;
    transposes.resize(dim_c);
    for (int j = 0; j < dim_c; ++j) 
    {
        transposes[j].resize(2*dim_r);
    }



    // runtime storage
    std::map<std::string, double> runtimes;
    std::map<std::string, double> plan_times;
    ////////////////////////////////////////////////////////////////////////////////////
    // PLANS: use fftw 
    // forward
    auto start_plan_fftw = t.now();
    fftw_plan plan_r2c_2d = fftw_plan_dft_r2c_2d(dim_r, dim_r,
                            input_1.data(),
                            reinterpret_cast<fftw_complex*>(input_1.data()),
                            FFTW_PLAN_FLAG);
    auto stop_plan_fftw = t.now();
    plan_times["plan_fftw_r2c"] = duration(stop_plan_fftw - start_plan_fftw).count();
    // backward
    fftw_plan plan_c2r_2d = fftw_plan_dft_c2r_2d(dim_r, dim_r,
                            reinterpret_cast<fftw_complex*>(input_1.data()),
                            input_1.data(),
                            FFTW_PLAN_FLAG);

    ////////////////////////////////////////////////////////////////////////////////////
    // PLANS: use local transform
    // forward step one
    auto start_plan_trans = t.now();
    int rank = 1;
    const int* n= new int(dim_r);
    int howmany = dim_r;
    int idist = dim_r + 2, odist = dim_r/2 + 1; /* the distance in memory between the first element of the first array and the first element of the second array */
    int istride = 1, ostride = 1; /* array is contiguous in memory */
    fftw_plan plan_2_r2c = fftw_plan_many_dft_r2c(rank, n, howmany,
                            input_2.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                            ostride, odist, FFTW_PLAN_FLAG);
    auto stop_plan_trans_1 = t.now();
    plan_times["plan_trans_r2c"] = duration(stop_plan_trans_1 - start_plan_trans).count();
    // forward step two
    idist = dim_r;
    odist = dim_r; 
    howmany = dim_c;
    fftw_plan plan_2_c2c = fftw_plan_many_dft(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                            ostride, odist, FFTW_FORWARD, FFTW_PLAN_FLAG);
    auto stop_plan_trans_2 = t.now();
    plan_times["plan_trans_c2c"] = duration(stop_plan_trans_2 - stop_plan_trans_1).count();
    // backward step one
    fftw_plan plan_2_c2c_b = fftw_plan_many_dft(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(transpose.data()), NULL,
                            ostride, odist, FFTW_BACKWARD, FFTW_PLAN_FLAG);
    // backward step two
    howmany = dim_r;
    idist = dim_r/2 + 1;
    odist = dim_r + 2;
    fftw_plan plan_2_c2r = fftw_plan_many_dft_c2r(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(input_2.data()), NULL,
                            istride, idist,
                            input_2.data(), NULL,
                            ostride, odist, FFTW_PLAN_FLAG);

    //////////////////////////////////////////////////////////////////////////////////
    // PLANS: strided fft
    /////////
    // forward step one
    auto start_plan_stride = t.now();
    howmany = dim_r;
    idist = dim_r + 2,
    odist = dim_r/2 + 1;
    fftw_plan plan_3_r2c = fftw_plan_many_dft_r2c(rank, n, howmany,
                            input_3.data(), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                            ostride, odist, FFTW_PLAN_FLAG);
    auto stop_plan_stride_1 = t.now();
    plan_times["plan_stride_r2c"] = duration(stop_plan_stride_1 - start_plan_stride).count();
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
                            ostride, odist, FFTW_FORWARD, FFTW_PLAN_FLAG);
    auto stop_plan_stride_2 = t.now();
    plan_times["plan_stride_c2c"] = duration(stop_plan_stride_2 - stop_plan_stride_1).count();
    // backward step one
    fftw_plan plan_3_c2c_b = fftw_plan_many_dft(rank, n, howmany,
                            reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                            istride, idist,
                            reinterpret_cast<fftw_complex*>(input_3.data()), NULL,
                            ostride, odist, FFTW_BACKWARD, FFTW_PLAN_FLAG);
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
                            ostride, odist, FFTW_PLAN_FLAG);

    ////////////////////////////////////////////////////////////////////////////////////
    // PLANS: single ffts with local transpose 
    std::vector<fftw_plan> plans_1D_r2c, plans_1D_c2c;
    plans_1D_r2c.resize(dim_r);
    plans_1D_c2c.resize(dim_c);
    // forward step one
    auto start_plan_loop = t.now();
    for(int i=0; i<dim_r; ++i)
    {
        plans_1D_r2c[i] =  fftw_plan_dft_r2c_1d(dim_r,
                            input_4[i].data(),
                            reinterpret_cast<fftw_complex*>(input_4[i].data()),
                            FFTW_PLAN_FLAG);
    }
    auto stop_plan_loop_1 = t.now();
    plan_times["plan_loop_r2c"] = duration(stop_plan_loop_1 - start_plan_loop).count();
    // forward step two
    for(int i=0; i<dim_c; ++i)
    {
        plans_1D_c2c[i] =  fftw_plan_dft_1d(dim_r, 
                            reinterpret_cast<fftw_complex*>(transposes[i].data()), 
                            reinterpret_cast<fftw_complex*>(transposes[i].data()), 
                            FFTW_FORWARD, FFTW_PLAN_FLAG);
    }
    auto stop_plan_loop_2 = t.now();
    plan_times["plan_loop_c2c"] = duration(stop_plan_loop_2 - stop_plan_loop_1).count();
    ///////////////////////////////////////////////////////////////////////////////
    // intialize with from 0 with spacers
    for(int i=0; i<dim_r; ++i)
    {
         for(int j=0; j<dim_r; ++j)
        {
            input_1[(dim_r+2)*i + j] = (dim_r)*i + j;
            input_2[(dim_r+2)*i + j] = (dim_r)*i + j;
            input_3[(dim_r+2)*i + j] = (dim_r)*i + j;
            //input_4[i][j] = (dim_r)*i + j;
        }
    }
    // std::cout << "Input: " << std::endl;
    // print_real(input_1,dim_r);
    
    ////////////////////////////////////////////////////////////////////////////////////
    // use fftw function  
    for(int i=0; i<loop; ++i)
    {
        auto start_fftw = t.now();
        // forward
        fftw_execute(plan_r2c_2d);
        // std::cout << "FFT: FFTW 2D" << std::endl;
        // print_complex(input_1,dim_r);
        auto stop_fftw = t.now();
        runtimes["fftw"] += duration(stop_fftw - start_fftw).count();
        
        // backward
        fftw_execute(plan_c2r_2d);
        // std::cout << "IFFT: FFTW 2D" << std::endl;
        // print_real(input_1,dim_r,1);
    }


    ////////////////////////////////////////////////////////////////////////////////////
    // use local transform 
    for(int i=0; i<loop; ++i)
    {
        // forward step one
        auto start_trans = t.now();
        fftw_execute(plan_2_r2c);
        auto stop_trans_1 = t.now();
        // local transpose
        // for (int i = 0; i < dim_r; ++i) 
        // {
        for (int j = 0; j < dim_r/2+1; ++j) 
        {
            for (int i = 0; i < dim_r; ++i) 
            {
                transpose[j * (2 * dim_r) +2*i] = input_2[i *(dim_r+2) + 2*j];
                transpose[j * (2 * dim_r) +2*i + 1] = input_2[i *(dim_r+2) + 2*j + 1];
            }
        }
        // forward step two
        auto stop_trans_2 = t.now();
        fftw_execute(plan_2_c2c);
        auto stop_trans_3 = t.now();
        // std::cout << "FFT: Local transpose" << std::endl;
        // print_complex_t(transpose,dim_r);

        runtimes["trans_r2c"] += duration(stop_trans_1 - start_trans).count();
        runtimes["trans_local"] += duration(stop_trans_2 - stop_trans_1).count();
        runtimes["trans_c2c"] += duration(stop_trans_3 - stop_trans_2).count();
        runtimes["trans_total"] += duration(stop_trans_3 - start_trans).count();

        // backward step one
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
        // backward step two
        fftw_execute(plan_2_c2r);
        // std::cout << "IFFT: Local transpose" << std::endl;
        // print_real(input_2,dim_r,1);
    }
    ////////////////////////////////////////////////////////////////////////////////////
    // strided fft
    /////////
    for(int i=0; i<loop; ++i)
    {
        // forward step one
        auto start_stride = t.now();
        fftw_execute(plan_3_r2c);
        auto stop_stride_1 = t.now();
        // forward step two
        fftw_execute(plan_3_c2c);
        auto stop_stride_2 = t.now();
        //std::cout << "FFT: Strided" << std::endl;
        //print_complex(input_3,dim_r);

        runtimes["stride_total"] += duration(stop_stride_2 - start_stride).count();
        runtimes["stride_r2c"] += duration(stop_stride_1 - start_stride).count();
        runtimes["stride_c2c"] += duration(stop_stride_2 - stop_stride_1).count();

        // backward step one
        fftw_execute(plan_3_c2c_b);
        // backward step two
        fftw_execute(plan_3_c2r);
        // std::cout << "IFFT: Strided" << std::endl;
        // print_real(input_3,dim_r,1);
    }
    ////////////////////////////////////////////////////////////////////////////////////
    // single ffts with local transpose
    for(int i=0; i<loop; ++i)
    { 
        // bc no back transform
        for(int i=0; i<dim_r; ++i)
        {
            for(int j=0; j<dim_r; ++j)
            {
                input_4[i][j] = (dim_r)*i + j;
            }
        }
        // forward step one
        auto start_loop = t.now();
        for(int i=0; i<dim_r; ++i)
        {
            fftw_execute(plans_1D_r2c[i]);
        }
        auto stop_loop_1 = t.now();
        
        // transpose
        for (int i = 0; i < dim_r; ++i) 
        {
        for (int j = 0; j < dim_c; ++j) 
        {
            // for (int i = 0; i < dim_r; ++i) 
            // {
                transposes[j][2*i] = input_4[i][2*j];
                transposes[j][2*i + 1] = input_4[i][2*j + 1];
                //std::cout << "(" << transposes[j][2*i] << " " << transposes[j][2*i+1]  << ")";
            }
            //std::cout << std::endl;
        }
        auto stop_loop_2 = t.now();
        
        // forward step two
        for(int i=0; i<dim_c; ++i)
        {
            fftw_execute(plans_1D_c2c[i]);
        }
        auto stop_loop_3 = t.now();

        runtimes["loop_r2c"] += duration(stop_loop_1 - start_loop).count();
        runtimes["loop_local"] += duration(stop_loop_2 - stop_loop_1).count();
        runtimes["loop_c2c"] += duration(stop_loop_3 - stop_loop_2).count();
        runtimes["loop_total"] += duration(stop_loop_3 - start_loop).count();      
    }
    ////////////////////////////////////////////////////////////////////////////////////
    // times 
    for (auto time : runtimes)
    {
        runtimes[time.first] = time.second / loop;
    }

    std::cout << "Elapsed fftw 2D (micros): \n total = " << runtimes["fftw"] << std::endl;

    std::cout << "Elapsed stride (micros): \n total = "  << runtimes["stride_total"] <<
                                    "\n r2c 1D = " << runtimes["stride_r2c"] <<
                                    "\n c2c 1D = " << runtimes["stride_c2c"]
                                                        << std::endl;

    std::cout << "Elapsed local transpose (micros): \n total = "  << runtimes["trans_total"] <<
                                "\n r2c 1D = " << runtimes["trans_r2c"] <<
                                "\n transpose = " << runtimes["trans_local"] <<
                                "\n c2c 1D = " << runtimes["trans_c2c"]
                                                    << std::endl;

    std::cout << "Elapsed 1D loop with transpose (micros): \n total = "  << runtimes["loop_total"] <<
                                        "\n r2c loop = " << runtimes["loop_r2c"] << 
                                        "\n transpose = " << runtimes["loop_local"] <<                              
                                        "\n c2c loop = " << runtimes["loop_c2c"]
                                                    << std::endl;
    
    std::ofstream plan_file;
    std::ofstream runtime_file;

    plan_file.open ("result/plan_times.txt", std::ios_base::app);
    runtime_file.open ("result/runtimes.txt", std::ios_base::app);
    
    if(dim_r == 2)
    {
        runtime_file << "N;loop;plan;"
                << "fftw_2D_total;"
                << "stride_total;stride_r2c;stride_c2c;"
                << "trans_total;trans_r2c;trans_local;trans_c2c;"
                << "loop_total;loop_r2c;loop_local;loop_c2c;\n";
        plan_file << "N;loop;plan;"
                << "plan_fftw_2D_r2c;"
                << "plan_stride_r2c;plan_stride_c2c;"
                << "plan_trans_r2c;plan_trans_c2c;"
                << "plan_loop_r2c;plan_loop_c2c;\n";
    }

    runtime_file << dim_r << ";" 
              << loop << ";"
              << plan_flag << ";"
              << runtimes["fftw"] << ";"
              << runtimes["stride_total"] << ";"
              << runtimes["stride_r2c"] << ";"
              << runtimes["stride_c2c"] << ";"
              << runtimes["trans_total"] << ";"
              << runtimes["trans_r2c"] << ";"
              << runtimes["trans_local"] << ";"
              << runtimes["trans_c2c"] << ";"
              << runtimes["loop_total"] << ";"
              << runtimes["loop_r2c"] << ";"
              << runtimes["loop_local"] << ";"
              << runtimes["loop_c2c"] << ";\n";

    plan_file << dim_r << ";" 
              << loop << ";" 
              << plan_flag << ";"
              << plan_times["plan_fftw_r2c"] << ";" 
              <<  plan_times["plan_stride_r2c"] << ";"
              <<  plan_times["plan_stride_c2c"] << ";"  
              <<  plan_times["plan_trans_r2c"] << ";"
              <<  plan_times["plan_trans_c2c"] << ";"
              <<  plan_times["plan_loop_r2c"] << ";"
              <<  plan_times["plan_loop_c2c"] << ";\n";

    plan_file.close();
    runtime_file.close();

    return hpx::finalize();    // Handles HPX shutdown
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("n", value<std::uint64_t>()->default_value(10),
        "Size per dimension n")
        ("f", value<std::string>()->default_value("estimate"),
        "FFTW Flag: estimate, measure, patient, exhaustive")
        ("l", value<std::uint64_t>()->default_value(1),
        "Number of loop iterations");

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return hpx::init(argc, argv, init_args);
}