#include "FFTW_Adapter.h"
#include "hpx_fft.h"

namespace FFTK {

    FFTW_Adapter::FFTW_Adapter() {}
    FFTW_Adapter::FFTW_Adapter(shared_ptr<Global> g): global(g) {}


    //initialize FFTW plans 
    void FFTW_Adapter::init_transform() {
        Global::Field &field = global->field;
        Global::MPI &mpi = global->mpi;


        //Initialize plans
        int Nx_dims[] = {field.Nx};
        int Ny_dims[] = {field.Ny};
        int Nz_dims[] = {field.Nz};


        FA.resize(field.FA_shape);
        IA.resize(field.IA_shape);
        RA.resize(field.RA_shape);


        FA.resize(shape(0,0,0));
        IA.resize(shape(0,0,0));
        RA.resize(shape(0,0,0));


        //X transforms
        fft_plan_Fourier_forward_x = FFTW_PLAN_MANY_DFT(1, Nx_dims, field.maxfy * field.maxfz,
                                                        reinterpret_cast<FFTW_Complex*>(FA.data()), NULL,
                                                        field.maxfy * field.maxfz, 1,
                                                        reinterpret_cast<FFTW_Complex*>(FA.data()), NULL,
                                                        field.maxfy * field.maxfz, 1,
                                                        FFTW_FORWARD, FFTW_PLAN_FLAG);

        fft_plan_Fourier_inverse_x = FFTW_PLAN_MANY_DFT(1, Nx_dims, field.maxfy * field.maxfz,
                                                        reinterpret_cast<FFTW_Complex*>(FA.data()), NULL,
                                                        field.maxfy * field.maxfz, 1,
                                                        reinterpret_cast<FFTW_Complex*>(FA.data()), NULL,
                                                        field.maxfy * field.maxfz, 1,
                                                        FFTW_BACKWARD, FFTW_PLAN_FLAG);

        if (fft_plan_Fourier_forward_x == NULL){
            //hpx::finalize();
        }
        if (fft_plan_Fourier_inverse_x == NULL){
            //hpx::finalize();
        }

        //Y transforms
        fft_plan_Fourier_forward_y = FFTW_PLAN_MANY_DFT(1, Ny_dims, field.maxiz,
                                                        reinterpret_cast<FFTW_Complex*>(IA.data()), NULL,
                                                        field.maxiz, 1,
                                                        reinterpret_cast<FFTW_Complex*>(IA.data()), NULL,
                                                        field.maxiz, 1,
                                                        FFTW_FORWARD, FFTW_PLAN_FLAG);

        fft_plan_Fourier_inverse_y = FFTW_PLAN_MANY_DFT(1, Ny_dims, field.maxiz,
                                                        reinterpret_cast<FFTW_Complex*>(IA.data()), NULL,
                                                        field.maxiz, 1,
                                                        reinterpret_cast<FFTW_Complex*>(IA.data()), NULL,
                                                        field.maxiz, 1,
                                                        FFTW_BACKWARD, FFTW_PLAN_FLAG);

        if (fft_plan_Fourier_forward_y == NULL){
            //hpx::finalize();
        }
        if (fft_plan_Fourier_inverse_y == NULL){
            //hpx::finalize();
        }

        // Z transforms
        fft_plan_Fourier_forward_z = FFTW_PLAN_MANY_DFT_R2C(1, Nz_dims, field.maxrx * field.maxry,
                                                            reinterpret_cast<Real*>(RA.data()), NULL,
                                                            1, field.Nz+2,
                                                            reinterpret_cast<FFTW_Complex*>(RA.data()), NULL,
                                                            1, field.Nz/2+1,
                                                            FFTW_PLAN_FLAG);

        fft_plan_Fourier_inverse_z = FFTW_PLAN_MANY_DFT_C2R(1, Nz_dims, field.maxrx * field.maxry,
                                                            reinterpret_cast<FFTW_Complex*>(RA.data()), NULL,
                                                            1, field.Nz/2+1,
                                                            reinterpret_cast<Real*>(RA.data()), NULL,
                                                            1, field.Nz+2,
                                                            FFTW_PLAN_FLAG);


        if (fft_plan_Fourier_forward_x == NULL){
            //hpx::finalize();
        }
        if (fft_plan_Fourier_inverse_x == NULL){
            //hpx::finalize();
        }
    }

    //3d forward transform
    void FFTW_Adapter::DFT_forward_R2C(Array<Real,3> Ar)
    {
        FFTW_EXECUTE_DFT_R2C(fft_plan_Fourier_forward_z, reinterpret_cast<Real*>(Ar.data()), reinterpret_cast<FFTW_Complex*>(Ar.data()));
    }

    void FFTW_Adapter::DFT_forward_y(Array<Complex,3> IA)
    {
        for (int ix = 0; ix < global->field.maxix; ix++)
            FFTW_EXECUTE_DFT(fft_plan_Fourier_forward_y, reinterpret_cast<FFTW_Complex*>(IA(ix,Range::all(),Range::all()).data()), reinterpret_cast<FFTW_Complex*>(IA(ix,Range::all(),Range::all()).data()));

    }

    void FFTW_Adapter::DFT_forward_x(Array<Complex,3> A)
    {
        FFTW_EXECUTE_DFT(fft_plan_Fourier_forward_x, reinterpret_cast<FFTW_Complex*>(A.data()), reinterpret_cast<FFTW_Complex*>(A.data()));
    }


    //3d inverse transform
    void FFTW_Adapter::DFT_inverse_x(Array<Complex,3> A)
    {
        FFTW_EXECUTE_DFT(fft_plan_Fourier_inverse_x, reinterpret_cast<FFTW_Complex*>(A.data()), reinterpret_cast<FFTW_Complex*>(A.data()));
    }

    void FFTW_Adapter::DFT_inverse_y(Array<Complex,3> IA)
    {
        for (int ix = 0; ix < global->field.maxix; ix++)
            FFTW_EXECUTE_DFT(fft_plan_Fourier_inverse_y, reinterpret_cast<FFTW_Complex*>(IA(ix,Range::all(),Range::all()).data()), reinterpret_cast<FFTW_Complex*>(IA(ix,Range::all(),Range::all()).data()));
    }

    void FFTW_Adapter::DFT_inverse_C2R(Array<Real,3> Ar)
    {
        FFTW_EXECUTE_DFT_C2R(fft_plan_Fourier_inverse_z, reinterpret_cast<FFTW_Complex*>(Ar.data()), reinterpret_cast<Real*>(Ar.data()));
    }


    void FFTW_Adapter::Finalize()
    {
            FFTW_DESTROY_PLAN(fft_plan_Fourier_forward_x);
            FFTW_DESTROY_PLAN(fft_plan_Fourier_inverse_x);
            FFTW_DESTROY_PLAN(fft_plan_Fourier_forward_y);
            FFTW_DESTROY_PLAN(fft_plan_Fourier_inverse_y);
            FFTW_DESTROY_PLAN(fft_plan_Fourier_forward_z);
            FFTW_DESTROY_PLAN(fft_plan_Fourier_inverse_z);
        }  
    }	
}
