#include <hpx/config.hpp>

#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/iostream.hpp>
#include <hpx/timing/high_resolution_timer.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <fftw3.h>

typedef double real;
typedef std::vector<real, std::allocator<real>> vector_1d;
//typedef std::vector<vector_1d> vector_2d;
//typedef vector_2d;
#include "vector_2d.hpp"
void print_vector_2d(const vector_2d<real>& input);

#include "vector_3d.hpp"

struct fft
{
    typedef fftw_plan fft_backend_plan;

    public:
        fft() = default;

        void initialize(vector_2d<real> values_vec, const unsigned PLAN_FLAG);

        vector_2d<real> fft_2d_r2c_par();

        vector_2d<real> fft_2d_r2c_seq();

        real get_measurement(std::string name);

        ~fft()
        {
            fftw_destroy_plan(plan_1d_r2c_);
            fftw_destroy_plan(plan_1d_c2c_);
            fftw_cleanup();
        }

    private:
        void fft_1d_r2c_inplace(const std::size_t i);

        void fft_1d_c2c_inplace(const std::size_t i);

        void transpose_shared_y_to_x(const std::size_t index_trans);

        // void transpose_shared_x_to_y(const std::size_t index_trans);    
        void transpose_shared_x_to_y(const std::size_t index);
   
    private:
        // parameters
        std::size_t dim_r_y_, dim_c_y_, dim_c_x_;
        // FFTW plans
        unsigned PLAN_FLAG_;
        fft_backend_plan plan_1d_r2c_;
        fft_backend_plan plan_1d_c2c_;
        // value vectors
        vector_2d<real> values_vec_;
        vector_2d<real> trans_values_vec_;
        // time measurement
        hpx::chrono::high_resolution_timer t_ = hpx::chrono::high_resolution_timer();
        std::map<std::string, real> measurements_;
};

real fft::get_measurement(std::string name)
{
    return measurements_[name];
}

void fft::fft_1d_r2c_inplace(const std::size_t i)
{
    fftw_execute_dft_r2c(plan_1d_r2c_, 
                            values_vec_.row(i), 
                            reinterpret_cast<fftw_complex*>(values_vec_.row(i)));
}

void fft::fft_1d_c2c_inplace(const std::size_t i)
{
    fftw_execute_dft(plan_1d_c2c_, 
                        reinterpret_cast<fftw_complex*>(trans_values_vec_.row(i)), 
                        reinterpret_cast<fftw_complex*>(trans_values_vec_.row(i)));
}

// void fft::transpose_shared_y_to_x(const std::size_t index)
// {
//     for( std::size_t index_trans = 0; index_trans < dim_c_x_; ++index_trans)
//     {
//         trans_values_vec_(index, 2 * index_trans) = values_vec_(index_trans, 2 * index);
//         trans_values_vec_(index, 2 * index_trans + 1) = values_vec_(index_trans, 2 * index + 1);
//     }     
// }  
void fft::transpose_shared_y_to_x(const std::size_t index_trans)
{
    for( std::size_t index = 0; index < dim_c_y_; ++index)
    {
        trans_values_vec_(index, 2 * index_trans) = values_vec_(index_trans, 2 * index);
        trans_values_vec_(index, 2 * index_trans + 1) = values_vec_(index_trans, 2 * index + 1);
    }     
}

void fft::transpose_shared_x_to_y(const std::size_t index_trans)
{
    for( std::size_t index = 0; index < dim_c_x_; ++index)
    {
        values_vec_(index, 2 * index_trans) = trans_values_vec_(index_trans, 2 * index);
        values_vec_(index, 2 * index_trans + 1) = trans_values_vec_(index_trans, 2 * index + 1);
    }     
}      
// void fft::transpose_shared_x_to_y(const std::size_t index)
// {
//     for( std::size_t index_trans = 0; index_trans < dim_c_y_; ++index_trans)
//     {
//         values_vec_(index, 2 * index_trans) = trans_values_vec_(index_trans, 2 * index);
//         values_vec_(index, 2 * index_trans + 1) = trans_values_vec_(index_trans, 2 * index + 1);
//     }     
// } 

vector_2d<real> fft::fft_2d_r2c_par()
{
    /////////////////////////////////////////////////////////////////
    // first dimension
    auto start_total = t_.now();
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_x_, [&](auto i)
    {
        // 1d FFT r2c in y-direction
        fft_1d_r2c_inplace(i);
    });
    auto start_first_trans = t_.now();
    //hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_x_, [&](auto i)
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_y_, [&](auto i)
    {
        // transpose from y-direction to x-direction
        transpose_shared_y_to_x(i);
    });
    // second dimension
    auto start_second_fft = t_.now();
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_y_, [&](auto i)
    {
        // 1D FFT c2c in x-direction
        fft_1d_c2c_inplace(i);
    });
    auto start_second_trans = t_.now();
    //hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_x_, [&](auto i)
    hpx::experimental::for_loop(hpx::execution::par, 0, dim_c_y_, [&](auto i)
    {
        // transpose from x-direction to y-direction
        transpose_shared_x_to_y(i);
    });
    auto stop_total = t_.now();
    ////////////////////////////////////////////////////////////////
    // additional runtimes
    measurements_["total"] = stop_total - start_total;
    measurements_["first_fftw"] = start_first_trans - start_total;
    measurements_["first_trans"] = start_second_fft - start_first_trans;
    measurements_["second_fftw"] = start_second_trans - start_second_fft;
    measurements_["second_trans"] = stop_total - start_second_trans;

    ///////////////////////////////////////////////////////////////7
    return std::move(values_vec_);
    //return values_vec_;
}

vector_2d<real> fft::fft_2d_r2c_seq()
{
    /////////////////////////////////////////////////////////////////
    // first dimension
    auto start_total = t_.now();
    for (std::size_t i = 0; i < dim_c_x_; ++i)
    {
        // 1d FFT r2c in y-direction
        fft_1d_r2c_inplace(i);
    }
    auto start_first_trans = t_.now();
    for (std::size_t i = 0; i < dim_c_x_; ++i)
    {
        // transpose from y-direction to x-direction
        transpose_shared_y_to_x(i);
    }
    // second dimension
    auto start_second_fft = t_.now();
    for (std::size_t i = 0; i < dim_c_y_; ++i)
    {
        // 1d FFT c2c in x-direction
        fft_1d_c2c_inplace(i);
    }
    auto start_second_trans = t_.now();
    for (std::size_t i = 0; i < dim_c_y_; ++i)
    {
        // transpose from x-direction to y-direction
        transpose_shared_x_to_y(i);
    }
    ////////////////////////////////////////////////////////////////
    // additional runtimes
    auto stop_total = t_.now();
    measurements_["total"] = stop_total - start_total;
    measurements_["first_fftw"] = start_first_trans - start_total;
    measurements_["first_trans"] = start_second_fft - start_first_trans;
    measurements_["second_fftw"] = start_second_trans - start_second_fft;
    measurements_["second_trans"] = stop_total - start_second_trans;

    ///////////////////////////////////////////////////////////////7
    return std::move(values_vec_);
    //return values_vec_;
}

void fft::initialize(vector_2d<real> values_vec, const unsigned PLAN_FLAG)
{
    // move data into own data structure
    values_vec_ = std::move(values_vec);
    //values_vec_ = values_vec;
    // parameters
    dim_c_x_ = values_vec_.dim_row();
    dim_c_y_ = values_vec_.dim_col() / 2;
    dim_r_y_ = 2 * dim_c_y_ - 2;
    // resize transposed data structure
    trans_values_vec_ = std::move(vector_2d<real>(dim_c_y_, 2 * dim_c_x_));
    //create fftw plans
    PLAN_FLAG_ = PLAN_FLAG;
    // r2c in y-direction
    plan_1d_r2c_ = fftw_plan_dft_r2c_1d(dim_r_y_,
                                       values_vec_.row(0),
                                       reinterpret_cast<fftw_complex*>(values_vec_.row(0)),
                                       PLAN_FLAG);
    // c2c in x-direction
    plan_1d_c2c_ = fftw_plan_dft_1d(dim_c_x_, 
                                   reinterpret_cast<fftw_complex*>(trans_values_vec_.row(0)), 
                                   reinterpret_cast<fftw_complex*>(trans_values_vec_.row(0)), 
                                   FFTW_FORWARD,
                                   PLAN_FLAG);
}

void print_vector_2d(const vector_2d<real>& input)
{
    const std::string msg = "\n";
    
    const std::size_t dim_x = input.dim_row();
    const std::size_t dim_y = input.dim_col();

    std::size_t counter = 0;
    for( std::size_t i = 0; i < dim_x; ++i)
    {
        for( std::size_t j = 0; j < dim_y; ++j)
        {
            real element =  input(i,j);
            if(counter%2 == 0)
            {
                std::string msg = "({1} ";
                hpx::util::format_to(hpx::cout, msg, element) << std::flush;
            }
            else
            {
                std::string msg = "{1}) ";
                hpx::util::format_to(hpx::cout, msg, element) << std::flush;
            }
            ++counter;
        }    
        hpx::util::format_to(hpx::cout, msg) << std::flush;
    }
    hpx::util::format_to(hpx::cout, msg) << std::flush;
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    ////////////////////////////////////////////////////////////////
    // Parameters and Data structures
    // hpx parameters
    const std::size_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    if (std::size_t(1) != num_localities)
    {
        std::cout << "Localities " << num_localities << " instead of 1: Abort runtime\n";
        return hpx::finalize();
    }
    const std::string run_flag = vm["run"].as<std::string>();
    const std::string plan_flag = vm["plan"].as<std::string>();
    bool print_result = vm["result"].as<bool>();
    bool print_header = vm["header"].as<bool>();
    // time measurement
    auto t = hpx::chrono::high_resolution_timer();
    // fft dimension parameters
    const std::size_t dim_c_x = vm["nx"].as<std::size_t>();//N_X; 
    const std::size_t dim_r_y = vm["ny"].as<std::size_t>();//N_Y;
    const std::size_t dim_c_y = dim_r_y / 2 + 1;
    // FFTW plans
    unsigned FFT_BACKEND_PLAN_FLAG = FFTW_ESTIMATE;
    if( plan_flag == "measure" )
    {
        FFT_BACKEND_PLAN_FLAG = FFTW_MEASURE;
    }
    else if ( plan_flag == "patient")
    {
        FFT_BACKEND_PLAN_FLAG = FFTW_PATIENT;
    }
    else if ( plan_flag == "exhaustive")
    {
        FFT_BACKEND_PLAN_FLAG = FFTW_EXHAUSTIVE;
    }
    ////////////////////////////////////////////////////////////////
    // initialize values
    // data vector
    vector_2d<real> values_vec(dim_c_x, 2*dim_c_y);
    for(std::size_t i = 0; i < dim_c_x; ++i)
    {
        std::iota(values_vec.row(i), values_vec.row(i+1) - 2, 0.0);
    }
    ////////////////////////////////////////////////////////////////
    // computation   
    // create and initialize object (deleted when out of scope)
    fft fft_computer;
    auto start_total = t.now();
    fft_computer.initialize(std::move(values_vec), FFT_BACKEND_PLAN_FLAG);
    auto stop_init = t.now();
    if( run_flag == "seq" )
    {
        values_vec = fft_computer.fft_2d_r2c_seq();
    }
    else
    {
        values_vec = fft_computer.fft_2d_r2c_par();
    }
    auto stop_total = t.now();

    // optional: print results 
    if (print_result)
    {
        print_vector_2d(values_vec);
    }
    
    ////////////////////////////////////////////////////////////////
    // print and store runtimes
    auto total = stop_total - start_total;
    auto init = stop_init - start_total;
    std::string msg = "\nLocality 0 - shared - {1}\n"
                      "Total runtime : {2}\n"
                      "Initialization: {3}\n"
                      "FFT 2D runtime: {4}\n"
                      "FFTW r2c      : {5}\n"
                      "First trans   : {6}\n"
                      "FFTW c2c      : {7}\n"
                      "Second trans  : {8}\n";
    hpx::util::format_to(hpx::cout, msg,
                        run_flag,  
                        total,
                        init,
                        fft_computer.get_measurement("total"),
                        fft_computer.get_measurement("first_fftw"),
                        fft_computer.get_measurement("first_trans"),
                        fft_computer.get_measurement("second_fftw"),
                        fft_computer.get_measurement("second_trans"))
                        << std::flush;
    std::ofstream runtime_file;
    runtime_file.open ("result/runtimes_hpx_loop_shared.txt", std::ios_base::app);
    if(print_header)
    {
        runtime_file << "n_threads;n_x;n_y;plan;run_flag;total;initialization;"
                << "fft_2d_total;"
                << "first_fftw;"
                << "first_trans;"
                << "second_fftw;"
                << "second_trans;\n";
    }
    runtime_file << hpx::get_os_thread_count() << ";" 
                << dim_c_x << ";"
                << dim_r_y << ";"
                << plan_flag << ";"
                << run_flag << ";"
                << total << ";"
                << init << ";"
                << fft_computer.get_measurement("total") << ";"
                << fft_computer.get_measurement("first_fftw") << ";"
                << fft_computer.get_measurement("first_trans") << ";"
                << fft_computer.get_measurement("second_fftw") << ";"
                << fft_computer.get_measurement("second_trans") << ";\n";
    runtime_file.close();

    ////////////////////////////////////////////////////////////////
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    auto a = vector_3d<real>(1,1,1);
    a(0,0,0) = 4;
    





    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
    ("result", value<bool>()->default_value(0), "print generated results (default: false)")
    ("nx", value<std::size_t>()->default_value(8), "Total x dimension")
    ("ny", value<std::size_t>()->default_value(14), "Total y dimension")
    ("plan", value<std::string>()->default_value("estimate"), "FFTW plan (default: estimate)")
    ("run",value<std::string>()->default_value("par"), "Choose 2d FFT algorithm: par or seq")
    ("header",value<bool>()->default_value(0), "Write runtime file header");

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return hpx::init(argc, argv, init_args);
}

#endif