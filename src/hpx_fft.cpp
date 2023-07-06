#include "hpx_fft.h"

namespace hpx::fft{

    FFF::FFF(shared_ptr<Global> global): global(global), fftw_adapter(global), communication(global) {
        set_field_vars();
        communication.Init();
        fftw_adapter.init_transform("FFF");
        init_transform();
	}


    void FFF::set_field_vars() {
        Global::Field &field = global->field;
		  
        field.Fx = field.Nx;
        field.Ix = field.Nx;
        field.Rx = field.Nx;

        field.Fy = field.Ny;
        field.Iy = field.Ny;
        field.Ry = field.Ny;

        field.Fz = field.Nz/2+1;
        field.Iz = field.Nz/2+1;
        field.Rz = field.Nz+2;

        field.Fz_unit = 2;
        field.Iz_unit = 2;
        field.Rz_unit = 1;
    }   

    /** 
     *  @brief Initialize transform related variables
     *
     */
    void FFF::init_transform() {
        Global::Field &field = global->field;
        
        normalizing_factor=1;
        IA.resize(field.IA_shape);

                
        normalizing_factor /= Real(field.Nx)*Real(field.Ny)*Real(field.Nz);
    }

    }


    /** @brief Perform the forward transform in 3D
     *
     *  @param Ar - The real space array
     *  @param Ac  - The (complex) Fourier space array
     *
     */
    void hpx::fft::forward_transform_3d(Array<Real,3> Ar, Array<Complex,3> Ac) 
    {    
        fftw_adapter.DFT_forward_R2C(Ar);

        //communication.Transpose_ZY(Ar, IA);

        fftw_adapter.DFT_forward_y(IA);

        //communication.Transpose_YX(IA, Ac);
        
        fftw_adapter.DFT_forward_x(Ac);

        // normalize fft
        Ac *= normalizing_factor;
    }


    /** @brief Perform the inverse transform in 3D
     *
     *  @param Ac  - The (complex) Fourier space array
     *  @param Ar - The real space array
     *
     */
    void hpx::fft::inverse_transform_3d(Array<Complex,3> Ac, Array<Real,3> Ar) {

        fftw_adapter.DFT_inverse_x(Ac);

        //communication.Transpose_XY(Ac, IA);

        fftw_adapter.DFT_inverse_y(IA);

        //communication.Transpose_YZ(IA, Ar);

        fftw_adapter.DFT_inverse_C2R(Ar);
    }

    void hpx::fft::Finalize() {
        
        fftw_adapter.Finalize();
        //communication.Finalize();
    }
}