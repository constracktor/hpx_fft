#include "../adapter/FFTW_Adapter.h"

#ifndef _HPX_FFF_H
#define _HPX_FFF_H

namespace hpx::fft {	
	class FFT {
	    shared_ptr<Global> global;
	    FFTW_Adapter fftw_adapter;

	    Array<Complex,3> IA;
	    
	    Real normalizing_factor;
	    
	public:
		FFF(shared_ptr<Global> global);

		void set_field_vars();//?
		void init_transform();
		void Forward_transform_3d(string basis_option, Array<Real,3> Ar, Array<Complex,3> A);
		void Inverse_transform_3d(string basis_option, Array<Complex,3> A, Array<Real,3> Ar);
		void Finalize();
	};
}
