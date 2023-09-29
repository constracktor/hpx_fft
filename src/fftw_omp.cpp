// #include"omp.h"  
// #include <mutex>
// #include <thread>
// #include <cstring>
// #include "fftw3.h"

// static const int nx = 128;
// static const int ny = 128;
// static const int ncomp = 2;
// static const int nyk = ny/2 + 1;

// class MyFftwClass {
// public:
//     MyFftwClass(void);
//     ~MyFftwClass(void);
//     void execute(double rArr[], double cArr[][ncomp]);

// private:
//     static fftw_plan s_plan; // <-- shared by all instances
//     double *m_buffer_in;
//     fftw_complex *m_buffer_out;
// };


// class MyFftwClass1 {
// public:
//     MyFftwClass1(void);
//     ~MyFftwClass1(void);
//     void execute1(double cArr[][ncomp],double rArr[]);

// private:
//     static fftw_plan s_plan1; // <-- shared by all instances
//     fftw_complex *m_buffer_in1;
//     double *m_buffer_out1;
// };

// std::mutex g_my_fftw_mutex; // <-- must lock before using any fftw_*() function, except for fftw_execute*()
// MyFftwClass1::MyFftwClass1(void)
// {
//     // serialize the initialization!
//     std::lock_guard<std::mutex> lock(g_my_fftw_mutex);

//     // allocate separate buffers for each instance
//     m_buffer_in1 = fftw_alloc_complex(nx * nyk);
//     m_buffer_out1 = fftw_alloc_real(nx * ny); 
//     if (!(m_buffer_in1 && m_buffer_out1))
//     {
//         throw new std::runtime_error("Failed to allocate memory!");
//     }

//     // create plan *once* for all instances/threads
//     if (!s_plan1)
//     {
//         s_plan1 = fftw_plan_dft_c2r_2d(nx, ny, m_buffer_in1, m_buffer_out1, FFTW_PATIENT);
//         if (!s_plan1)
//         {
//             throw new std::runtime_error("Failed to create plan!");
//         }
//     }

// }
// MyFftwClass1::~MyFftwClass1(void)
// {
//     std::lock_guard<std::mutex> lock(g_my_fftw_mutex);
//     fftw_destroy_plan(s_plan1);
//     fftw_free(m_buffer_in1);
//     fftw_free(m_buffer_out1);
// }

// void MyFftwClass1::execute1(double cArr[][ncomp],double rArr[]) //void MyFftwClass::execute(double *const in, fftw_complex *const out)
// {
//     // No serialization is needed here
//     memcpy(m_buffer_in1,cArr, sizeof(fftw_complex) * nx*(nyk));
//     fftw_execute_dft_c2r(s_plan1, m_buffer_in1, m_buffer_out1); //instead of fftw_excute(plan)
//     memcpy(rArr, m_buffer_out1,sizeof(double) * nx*ny);
//     //(rArr, 1.0 / (nx*ny), rArr); //renormalize 
// }

// fftw_plan MyFftwClass1::s_plan1 = NULL;

// //std::mutex g_my_fftw_mutex; // <-- must lock before using any fftw_*() function, except for fftw_execute*()
// MyFftwClass::MyFftwClass(void)
// //int r2cfft_initialize(r2cfft_t *const ctx, const std::size_t nx, const std::size_t ny)
// {
//     // serialize the initialization!
//     std::lock_guard<std::mutex> lock(g_my_fftw_mutex);

//     //allocating buffers first before plan creating gets rid off seg fault error

//     // allocate separate buffers for each instance
//     m_buffer_in = fftw_alloc_real(nx * ny); 
//     m_buffer_out = fftw_alloc_complex(nx * nyk);
//     if (!(m_buffer_in && m_buffer_out))
//     {
//         throw new std::runtime_error("Failed to allocate memory!");
//     }

//     // create plan *once* for all instances/threads
//    if (!s_plan)
//     {
//         s_plan = fftw_plan_dft_r2c_2d(nx, ny, m_buffer_in, m_buffer_out, FFTW_PATIENT);
//         if (!s_plan)
//         {
//             throw new std::runtime_error("Failed to create plan!");
//         }
//     }

    
// }

// MyFftwClass::~MyFftwClass(void)
// {
//     std::lock_guard<std::mutex> lock(g_my_fftw_mutex);
//     fftw_free(m_buffer_in);
//     fftw_free(m_buffer_out);
//     fftw_destroy_plan(s_plan);
// }

// void MyFftwClass::execute(double rArr[], double cArr[][ncomp]) //void MyFftwClass::execute(double *const in, fftw_complex *const out)
// {
//     // No serialization is needed here
//     memcpy(m_buffer_in, rArr,  sizeof(double) * nx*ny);
//     fftw_execute_dft_r2c(s_plan, m_buffer_in, m_buffer_out); //instead of fftw_excute(plan)
//     memcpy(cArr, m_buffer_out, sizeof(fftw_complex) * nx*(nyk));
// }

// fftw_plan MyFftwClass::s_plan = NULL;

// int main(){

//     fftw_init_threads();
//     //fftw_import_system_wisdom();
//     //fftw_import_wisdom_from_filename("/var/tmp/my_application_fftw.wisdom");
//     int nThreads =1;//4;
//     omp_set_num_threads(nThreads);
//     fftw_plan_with_nthreads(nThreads);


//     double *Function;
//     Function= (double*) fftw_malloc(nx*ny*sizeof(double));
//     for (int i = 0; i < nx; i++){
//         for (int j = 0; j < ny; j++){
//             Function[j + ny*i]  = 1.;//some initialization;
//         }
//     }

//     fftw_complex *Functionk;
//     Functionk= (fftw_complex*) fftw_malloc(nx*nyk*sizeof(fftw_complex));
//     memset(Functionk, 42, nx*nyk* sizeof(fftw_complex));

//     double *Out;
//     Out = (double*) fftw_malloc(nx*ny*sizeof(double));
//     memset(Out, 42, nx*ny* sizeof(double));
//     MyFftwClass r2c1; //declare r2c1 of type MyFftwClass
//     MyFftwClass1 c2r1; //declare r2c1 of type MyFftwClass
//     for(int i = 0; i < 100000; ++i) {
//         r2c1.execute(Function,Functionk);
//         c2r1.execute1(Functionk,Out);
//     }
//     //fftw_export_wisdom_to_filename("/var/tmp/my_application_fftw.wisdom");
//     //fftw_free stuff
// }


//     ////////////////////////////////////////////////////////////////////////////////////
//     // PLANS: use fftw 
//     // forward
//     auto start_plan_fftw = t.now();
//     fftw_plan plan_r2c_2d = fftw_plan_dft_r2c_2d(dim_r, dim_r,
//                             input_1.data(),
//                             reinterpret_cast<fftw_complex*>(input_1.data()),
//                             FFTW_PLAN_FLAG);
//     auto stop_plan_fftw = t.now();
//     plan_times["plan_fftw_r2c"] = duration(stop_plan_fftw - start_plan_fftw).count();
//     // backward
//     fftw_plan plan_c2r_2d = fftw_plan_dft_c2r_2d(dim_r, dim_r,
//                             reinterpret_cast<fftw_complex*>(input_1.data()),
//                             input_1.data(),
//                             FFTW_PLAN_FLAG);


//   // use fftw function  
//     for(int i=0; i<loop; ++i)
//     {
//         auto start_fftw = t.now();
//         // forward
//         fftw_execute(plan_r2c_2d);
//         // std::cout << "FFT: FFTW 2D" << std::endl;
//         // print_complex(input_1,dim_r);
//         auto stop_fftw = t.now();
//         runtimes["fftw"] += duration(stop_fftw - start_fftw).count();
        
//         // backward
//         fftw_execute(plan_c2r_2d);
//         // std::cout << "IFFT: FFTW 2D" << std::endl;
//         // print_real(input_1,dim_r,1);
//     }

#include <iostream>
#include <vector>
#include <chrono>
#include <map>
#include <string>
#include <thread>
#include <fstream>

#include "fftw3.h"
#include "omp.h" 

using vector = std::vector<double, std::allocator<double>>;

// #define N_X 8
// #define N_Y 14 

void print_real(const std::vector<double>& input, int dim_x, int dim_y, int scaling = 0)
{
    double factor = 1.0;
    if (scaling > 0)
    {
        factor = factor / (scaling * dim_x * dim_y);
    }
    
    for(int i=0; i<dim_x; ++i)
    {
        for(int j=0; j<dim_y + 2; ++j)
        {
            std::cout << input[(dim_y+2)*i + j] * factor << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print_complex(const std::vector<double>& input, int dim_x, int dim_y)
{
    for(int i=0; i<dim_x; ++i)
    {
        for(int j=0; j<dim_y + 2; j=j+2)
        {
            std::cout << "(" << input[(dim_y+2)*i + j] << " " << input[(dim_y+2)*i + j +1]  << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


int main(int argc, char* argv[])
{
    //          N_X N_Y loop plan     threads
    //  fftw_omp 8  14   1  estimate    1
    ////////////////////////////////////////////////
    // Parameters and Data Structures
    // fft dimension parameters
    const std::uint32_t dim_c_x = std::stoi(argv[1]);//N_X; 
    const std::uint32_t dim_r_y = std::stoi(argv[2]);//N_Y;
    const std::uint32_t dim_c_y = dim_r_y / 2 + 1;
    // fftw flag
    std::string plan_flag = argv[4];
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
    std::uint32_t loop = std::stoi(argv[3]);//1;    // Number of loops to average
    ////////////////////////////////////////////////
    // Time measurement
    auto t = std::chrono::steady_clock();
    using duration = std::chrono::duration<double>;
    // runtime storage
    std::map<std::string, double> runtimes;
    std::map<std::string, double> plan_times;
    
    ////////////////////////////////////////////////
    // OpenMP setup
    fftw_init_threads();
    //fftw_import_system_wisdom();
    //fftw_import_wisdom_from_filename("/var/tmp/my_application_fftw.wisdom");
    int nThreads = std::stoi(argv[5]);//1;//4;
    omp_set_num_threads(nThreads);
    fftw_plan_with_nthreads(nThreads);
    int n_ranks = nThreads;
    ////////////////////////////////////////////////
    vector input(dim_c_x * dim_c_y * 2);

    ////////////////////////////////////////////////
    // FFTW plans
    // forward
    auto start_plan_fftw_r2c = t.now();
    fftw_plan plan_r2c_2d = fftw_plan_dft_r2c_2d(dim_c_x, dim_r_y,
                            input.data(),
                            reinterpret_cast<fftw_complex*>(input.data()),
                            FFTW_PLAN_FLAG);
    auto stop_plan_fftw_r2c = t.now();
    plan_times["plan_fftw_r2c"] = duration(stop_plan_fftw_r2c - start_plan_fftw_r2c).count();
    
    // backward
    auto start_plan_fftw_c2r = t.now();
    fftw_plan plan_c2r_2d = fftw_plan_dft_c2r_2d(dim_c_x, dim_r_y,
                            reinterpret_cast<fftw_complex*>(input.data()),
                            input.data(),
                            FFTW_PLAN_FLAG);
    auto stop_plan_fftw_c2r = t.now();
    plan_times["plan_fftw_c2r"] = duration(stop_plan_fftw_c2r - start_plan_fftw_c2r).count();

    ////////////////////////////////////////////////
    // Data initialization
    // intialize from 0 with complex spacers at the end
    for(int i=0; i<int(dim_c_x); ++i)
    {
         for(int j=0; j<dim_r_y; ++j)
        {
            input[(dim_r_y+2)*i + j] = j;//(dim_r_y)*i + j;
        }
    }

    // ////
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000*rank)); 
    // std::cout << "Node: " << rank + 1 << " / " << n_ranks << std::endl;
    // std::cout << "Before:" << std::endl;
    // print_real(input, dim_c_x / n_ranks, dim_r_y, 0);
    // ////

    ////////////////////////////////////////////////
    // Compute FFTW
    for(int i=0; i<loop; ++i)
    {
        // forward
        auto start_fftw_r2c = t.now();
        fftw_execute(plan_r2c_2d);
        auto stop_fftw_r2c = t.now();
        runtimes["fftw_r2c"] += duration(stop_fftw_r2c - start_fftw_r2c).count();

        // ////
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000*rank)); 
        // std::cout << "Node: " << rank + 1 << " / " << n_ranks << std::endl;
        // std::cout << "FFT: FFTW 2D" << std::endl;
        // print_complex(input, dim_c_x / n_ranks, dim_r_y);
        // ////
        
        // backward
        auto start_fftw_c2r = t.now();
        fftw_execute(plan_c2r_2d);
        auto stop_fftw_c2r = t.now();
        runtimes["fftw_c2r"] += duration(stop_fftw_c2r - start_fftw_c2r).count();

        // ////
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000*rank)); 
        // std::cout << "Node: " << rank + 1 << " / " << n_ranks << std::endl;
        // std::cout << "IFFT: FFTW 2D" << std::endl;
        // print_real(input, dim_c_x / n_ranks, dim_r_y, n_ranks);
        // ////

        // rescale for next iteration
        // for (std::size_t i=0; i != size_local; ++i)
        // {
        //     input[i] = input[i] / (dim_r_y * dim_c_x);
        // }
    }

    ////////////////////////////////////////////////
    // Cleanup
    // FFTW cleanup
    fftw_destroy_plan(plan_r2c_2d);
    fftw_destroy_plan(plan_c2r_2d);
    fftw_cleanup_threads();

    ////////////////////////////////////////////////
    // Print and store runtimes
    for (auto time : runtimes)
    {
        runtimes[time.first] = time.second / loop;
    }

    std::cout << "FFTW with MPI planning time:" 
            << "\n plan_r2c = " << plan_times["plan_fftw_r2c"]
            << "\n plan_c2r = " << plan_times["plan_fftw_c2r"]
            << std::endl;
    std::cout << "FFTW with MPI running time:" 
            << "\n run_r2c  = " << runtimes["fftw_r2c"]
            << "\n run_c2r  = " << runtimes["fftw_c2r"] 
            << std::endl;

    std::ofstream plan_file;
    std::ofstream runtime_file;

    plan_file.open ("result/plan_times_mpi.txt", std::ios_base::app);
    runtime_file.open ("result/runtimes_mpi.txt", std::ios_base::app);

    runtime_file << "n_ranks;n_x;n_y;loop;plan;"
            << "fftw_mpi_r2c;"
            << "fftw_mpi_c2r;\n";
    plan_file << "n_ranks;n_x;n_y;loop;plan;"
            << "plan_fftw_mpi_r2c;"
            << "plan_fftw_mpi_c2r;\n";

    runtime_file << n_ranks << ";" 
                << dim_c_x << ";"
                << dim_r_y << ";"
                << loop << ";"
                << plan_flag << ";"
                << runtimes["fftw_r2c"] << ";"
                << runtimes["fftw_c2r"] << ";\n";

    plan_file << n_ranks << ";" 
                << dim_c_x << ";"
                << dim_r_y << ";"
                << loop << ";" 
                << plan_flag << ";"
                << plan_times["plan_fftw_r2c"] << ";" 
                << plan_times["plan_fftw_c2r"] << ";\n";

    plan_file.close();
    runtime_file.close();

    ////////////////////////////////////////////////
    return 0;
}