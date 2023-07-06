
#include <fftw3.h>


#include <complex>

#ifndef _FFTW_ADAPTER_H
#define _FFTW_ADAPTER_H


// Defining REAL_DOUBLE: switch for setting double
#if defined(REAL_DOUBLE)
#define Real                            double
#elif defined(REAL_FLOAT)
#define Real                            float
#endif

#define FFTW_Complex                    fftw_complex
#define FFTW_PLAN                       fftw_plan

#define FFTW_EXECUTE                    fftw_execute
#define FFTW_EXECUTE_R2R                fftw_execute_r2r
#define FFTW_EXECUTE_DFT                fftw_execute_dft
#define FFTW_EXECUTE_DFT_R2C            fftw_execute_dft_r2c
#define FFTW_EXECUTE_DFT_C2R            fftw_execute_dft_c2r
#define FFTW_DESTROY_PLAN               fftw_destroy_plan


#ifndef Complex
#define Complex  std::complex<Real>
#endif

// Switches for FFTW plans 
#if defined(ESTIMATE)
#define FFTW_PLAN_FLAG FFTW_ESTIMATE

#elif defined(MEASURE)
#define FFTW_PLAN_FLAG FFTW_MEASURE

#elif defined(PATIENT)
#define FFTW_PLAN_FLAG FFTW_PATIENT

#elif defined(EXHAUSTIVE)
#define FFTW_PLAN_FLAG FFTW_EXHAUSTIVE
#endif


#endif

namespace hpx::fft {	
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

		void DFT_forward_C2C(string basis_option, Array<Complex,3> A);
		void DFT_inverse_C2C(string basis_option, Array<Complex,3> A);


		void Finalize();
	};
}

#endif