#include <iostream>
#include <vector>
#include <hpx/hpx.hpp>
#include <hpx/init.hpp>
#include <hpx/future.hpp>

#include <fftw3.h>

class FFTW_Adapter {
	    shared_ptr<Global> global;
	    Utilities utilities;

	    //plans for FFF basis
		FFTW_PLAN fft_plan_Fourier_forward_x;
        FFTW_PLAN fft_plan_Fourier_inverse_x;
        FFTW_PLAN fft_plan_Fourier_forward_y;
        FFTW_PLAN fft_plan_Fourier_inverse_y;
        FFTW_PLAN fft_plan_Fourier_forward_z;
        FFTW_PLAN fft_plan_Fourier_inverse_z;

        //plans for sine/ cosine basis
        FFTW_PLAN fft_plan_Sine_forward_x;
		FFTW_PLAN fft_plan_Sine_inverse_x;
		FFTW_PLAN fft_plan_Sine_forward_y;
		FFTW_PLAN fft_plan_Sine_inverse_y;
		FFTW_PLAN fft_plan_Sine_forward_z;
		FFTW_PLAN fft_plan_Sine_inverse_z;

		FFTW_PLAN fft_plan_Cosine_forward_x;
		FFTW_PLAN fft_plan_Cosine_inverse_x;
		FFTW_PLAN fft_plan_Cosine_forward_y;
		FFTW_PLAN fft_plan_Cosine_inverse_y;
		FFTW_PLAN fft_plan_Cosine_forward_z;
		FFTW_PLAN fft_plan_Cosine_inverse_z;

        Array<Complex,3> FA;
        Array<Complex,3> IA;
        Array<Real,3> RA;

	public: 
		FFTW_Adapter();
		FFTW_Adapter(shared_ptr<Global> global);
		void init_transform(string basis);
		void DFT_forward_x(Array<Complex,2> A);
		void DFT_forward_R2C(Array<Real,2> Ar);
		void DFT_inverse_x(Array<Complex,2> A);
		void DFT_inverse_C2R(Array<Real,2> Ar);

		void DFT_forward_x(Array<Complex,3> A);
		void DFT_forward_R2C(Array<Real,3> Ar);
		void DFT_forward_y(Array<Complex,3> IA);
		void DFT_inverse_x(Array<Complex,3> A);
		void DFT_inverse_C2R(Array<Real,3> Ar);
		void DFT_inverse_y(Array<Complex,3> IA);

		void DFT_forward_R2R(string basis_option, Array<Real,2> Ar);
		void DFT_forward_R2R_x(string basis_option, Array<Complex,2> A);
		void DFT_inverse_R2R(string basis_option, Array<Real,2> Ar);
		void DFT_inverse_R2R_x(string basis_option, Array<Complex,2> A);

		void DFT_forward_R2R(string basis_option, Array<Real,3> Ar);
		void DFT_forward_R2R_x(string basis_option, Array<Complex,3> A);
		void DFT_inverse_R2R(string basis_option, Array<Real,3> Ar);
		void DFT_inverse_R2R_x(string basis_option, Array<Complex,3> A);
		void DFT_forward_R2R_y(string basis_option, Array<Complex,3> IA);
		void DFT_inverse_R2R_y(string basis_option, Array<Complex,3> IA);

		void DFT_forward_C2C(string basis_option, Array<Complex,2> A);
		void DFT_inverse_C2C(string basis_option, Array<Complex,2> A);
		void DFT_forward_C2C(string basis_option, Array<Complex,3> A);
		void DFT_inverse_C2C(string basis_option, Array<Complex,3> A);


		void Finalize();
	};
    
int hpx_main(hpx::program_options::variables_map& vm)
{
  std::cout << "Hello World\n";


namespace hpxex = hpx::execution;
namespace hpxexp = hpx::execution::experimental;
//namespace hpxtt = hpx::this_thread::experimental;

// Parallel execution using default executor
std::vector v = {1.0, 2.0};
hpx::for_each(hpxex::par, v.begin(), v.end(), [](double val) { std::cout << "default " << val << std::endl; });

// Parallel execution using parallel_executor
hpxex::parallel_executor exec;
hpx::for_each(hpxex::par.on(exec), v.begin(), v.end(), [](double val) { std::cout << "parallel" << val << std::endl; });

// Parallel asynchronous (eager) execution using parallel_executor
hpx::future f = hpx::for_each(hpxex::par(hpxex::task).on(exec), v.begin(), v.end(), [](double val) { std::cout << "async" << val << std::endl; });
f.get(); // wait for completion


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