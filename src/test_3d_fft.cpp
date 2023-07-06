		fftk.Forward_transform_3d(basis_option, Ar, A);

		Print_large_Fourier_elements(A, "A");

		fftk.Inverse_transform_3d(basis_option, A, Ar);





        Forward_transform_3d  = [basis](string basis_option, Array<Real,3> RA, Array<Complex,3> FA) { 
                            basis->Forward_transform_3d(basis_option, RA, FA);
                        };
                        
        Inverse_transform_3d  = [basis](string basis_option, Array<Complex,3> FA, Array<Real,3> RA) { 
                            basis->Inverse_transform_3d(basis_option, FA, RA);
                        };


    /** @brief Perform the Forward transform
     *
     *  @param basis_option - basis_option can be "FFF", "SFF", "SSF", "SSS"
     *                      - F stands for Fourier
     *                      - S stands for Sine/Cosine
     *  @param Ar - The real space array
     *  @param A  - The Fourier space array
     *
     */
    void FFF::Forward_transform_3d(string basis_option, Array<Real,3> Ar, Array<Complex,3> A) {
        
        TIMER_START(0);
        fftw_adapter.DFT_forward_R2C(Ar);
        TIMER_END(0);

        TIMER_START(1);
        communication.Transpose_ZY(Ar, IA);
        TIMER_END(1);

        TIMER_START(2);
        fftw_adapter.DFT_forward_y(IA);
        TIMER_END(2);

        TIMER_START(3);
        communication.Transpose_YX(IA, A);
        TIMER_END(3);

        TIMER_START(4);
        fftw_adapter.DFT_forward_x(A);
        TIMER_END(4);

        TIMER_START(5);
        A *= normalizing_factor;
        TIMER_END(5);
    }


    /** @brief Perform the Inverse transform
     *
     *  @param basis_option - basis_option can be "FFF", "SFF", "SSF", "SSS"
     *                      - F stands for Fourier
     *                      - S stands for Sine/Cosine
     *  @param A  - The Fourier space array
     *  @param Ar - The real space array
     *
     */
    void FFF::Inverse_transform_3d(string basis_option, Array<Complex,3> A, Array<Real,3> Ar) {
        
        TIMER_START(6);
        fftw_adapter.DFT_inverse_x(A);
        TIMER_END(6);

        TIMER_START(7);
        communication.Transpose_XY(A, IA);
        TIMER_END(7);

        TIMER_START(8);
        fftw_adapter.DFT_inverse_y(IA);
        TIMER_END(8);

        TIMER_START(9);
        communication.Transpose_YZ(IA, Ar);
        TIMER_END(9);

        TIMER_START(10);
        fftw_adapter.DFT_inverse_C2R(Ar);
        TIMER_END(10);
    }

    void FFF::Finalize() {
        
        fftw_adapter.Finalize();
        communication.Finalize();
       
    }
}

    //3d forward transform
    void FFTW_Adapter::DFT_forward_x(Array<Complex,3> A){
            FFTW_EXECUTE_DFT(fft_plan_Fourier_forward_x, reinterpret_cast<FFTW_Complex*>(A.data()), reinterpret_cast<FFTW_Complex*>(A.data()));


        }

    void FFTW_Adapter::DFT_forward_R2C(Array<Real,3> Ar){
            //for debug purpose:
            //cout << "Ar shape = " << fft_plan_Fourier_forward_z << " " << Ar.shape() << endl;
            
            FFTW_EXECUTE_DFT_R2C(fft_plan_Fourier_forward_z, reinterpret_cast<Real*>(Ar.data()), reinterpret_cast<FFTW_Complex*>(Ar.data()));


    }

    void FFTW_Adapter::DFT_forward_y(Array<Complex,3> IA){
            for (int ix = 0; ix < global->field.maxix; ix++)
                FFTW_EXECUTE_DFT(fft_plan_Fourier_forward_y, reinterpret_cast<FFTW_Complex*>(IA(ix,Range::all(),Range::all()).data()), reinterpret_cast<FFTW_Complex*>(IA(ix,Range::all(),Range::all()).data()));

        }

    //3d inverse transform

    void FFTW_Adapter::DFT_inverse_x(Array<Complex,3> A){
            FFTW_EXECUTE_DFT(fft_plan_Fourier_inverse_x, reinterpret_cast<FFTW_Complex*>(A.data()), reinterpret_cast<FFTW_Complex*>(A.data()));

        }

    void FFTW_Adapter::DFT_inverse_C2R(Array<Real,3> Ar){
            FFTW_EXECUTE_DFT_C2R(fft_plan_Fourier_inverse_z, reinterpret_cast<FFTW_Complex*>(Ar.data()), reinterpret_cast<Real*>(Ar.data()));
    }

    void FFTW_Adapter::DFT_inverse_y(Array<Complex,3> IA){
        for (int ix = 0; ix < global->field.maxix; ix++)
            FFTW_EXECUTE_DFT(fft_plan_Fourier_inverse_y, reinterpret_cast<FFTW_Complex*>(IA(ix,Range::all(),Range::all()).data()), reinterpret_cast<FFTW_Complex*>(IA(ix,Range::all(),Range::all()).data()));
        }












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
                    if (global->mpi.my_id==0) cerr << "FFF basis failed to initialize 'fft_plan_Fourier_forward_x'" << endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                if (fft_plan_Fourier_inverse_x == NULL){
                    if (global->mpi.my_id==0) cerr << "FFF basis failed to initialize 'fft_plan_Fourier_inverse_x'" << endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }