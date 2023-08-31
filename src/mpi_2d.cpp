#include <iostream>
#include <vector>
#include <chrono>
#include <map>
#include <string>
#include <thread>
#include <fstream>

#include <fftw3-mpi.h>

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
    //       nodes      N_X N_Y loop  plan
    // srun -n 2 mpi_2d  8  14   1  estimate
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
    // MPI setup
    int rank, n_ranks;
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &n_ranks);
    
    ////////////////////////////////////////////////
    // FFTW MPI setup
    std::ptrdiff_t size_local, local_n0, local_0_start;
    fftw_mpi_init();
    // get local data size and allocate
    size_local = fftw_mpi_local_size_2d(dim_c_x, dim_r_y+2, comm,
                                         &local_n0, &local_0_start);
    vector input(size_local);

    ////////////////////////////////////////////////
    // FFTW plans
    // forward
    MPI_Barrier(comm);
    auto start_plan_fftw_r2c = t.now();
    fftw_plan plan_r2c_2d = fftw_mpi_plan_dft_r2c_2d(dim_c_x, dim_r_y,
                            input.data(),
                            reinterpret_cast<fftw_complex*>(input.data()),
                            comm,
                            FFTW_PLAN_FLAG);
    MPI_Barrier(comm);
    auto stop_plan_fftw_r2c = t.now();
    plan_times["plan_fftw_r2c"] = duration(stop_plan_fftw_r2c - start_plan_fftw_r2c).count();
    
    // backward
    MPI_Barrier(comm);
    auto start_plan_fftw_c2r = t.now();
    fftw_plan plan_c2r_2d = fftw_mpi_plan_dft_c2r_2d(dim_c_x, dim_r_y,
                            reinterpret_cast<fftw_complex*>(input.data()),
                            input.data(),
                            comm,
                            FFTW_PLAN_FLAG);
    MPI_Barrier(comm);
    auto stop_plan_fftw_c2r = t.now();
    plan_times["plan_fftw_c2r"] = duration(stop_plan_fftw_c2r - start_plan_fftw_c2r).count();

    ////////////////////////////////////////////////
    // Data initialization
    // intialize from 0 with complex spacers at the end
    for(int i=0; i<int(local_n0); ++i)
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
        MPI_Barrier(comm);
        auto start_fftw_r2c = t.now();
        fftw_execute(plan_r2c_2d);
        MPI_Barrier(comm);
        auto stop_fftw_r2c = t.now();
        runtimes["fftw_r2c"] += duration(stop_fftw_r2c - start_fftw_r2c).count();

        // ////
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000*rank)); 
        // std::cout << "Node: " << rank + 1 << " / " << n_ranks << std::endl;
        // std::cout << "FFT: FFTW 2D" << std::endl;
        // print_complex(input, dim_c_x / n_ranks, dim_r_y);
        // ////
        
        // backward
        MPI_Barrier(comm);
        auto start_fftw_c2r = t.now();
        fftw_execute(plan_c2r_2d);
        MPI_Barrier(comm);
        auto stop_fftw_c2r = t.now();
        runtimes["fftw_c2r"] += duration(stop_fftw_c2r - start_fftw_c2r).count();

        // ////
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000*rank)); 
        // std::cout << "Node: " << rank + 1 << " / " << n_ranks << std::endl;
        // std::cout << "IFFT: FFTW 2D" << std::endl;
        // print_real(input, dim_c_x / n_ranks, dim_r_y, n_ranks);
        // ////

        // rescale for next iteration
        for (std::size_t i=0; i != size_local; ++i)
        {
            input[i] = input[i] / (dim_r_y * dim_c_x);
        }
    }

    ////////////////////////////////////////////////
    // Cleanup
    // FFTW cleanup
    fftw_destroy_plan(plan_r2c_2d);
    fftw_destroy_plan(plan_c2r_2d);
    fftw_mpi_cleanup();
    // MPI cleanup
    MPI_Finalize();

    ////////////////////////////////////////////////
    // Print and store runtimes
    
    if( rank == 0)
    {
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

        if(n_ranks == 1)
        {
            runtime_file << "n_ranks;n_x;n_y;loop;plan;"
                    << "fftw_mpi_r2c;"
                    << "fftw_mpi_c2r;\n";
            plan_file << "n_ranks;n_x;n_y;loop;plan;"
                    << "plan_fftw_mpi_r2c;"
                    << "plan_fftw_mpi_c2r;\n";
        }

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
    }
    ////////////////////////////////////////////////
    return 0;
}
