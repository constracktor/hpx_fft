# HPX based FFT with FFTW3 as 1D backend and FFTW3 with HPX backend

This repo is currently under active developement.
## Structure

- /src contains different shared-memory and distributed implementations 
of the same 2D FFT example

- /benchmark contains sbatch scripts to launch strong scaling tests

## Run
### Shared
All:

- `./shared_benchmark.sh estimate/measure partition_name`

Executables only:

- fft_hpx_loop_shared/fft_hpx_task_sync_shared/fft_hpx_task_shared/fft_hpx_task_naive_shared
/fft_hpx_task_agas_shared/fft_hpx_loop/fft_hpx_task_agas:
`sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_hpx_shared.sh executable_name estimate/measure $THREAD_POW`


- fftw_hpx/fftw_mpi_threads/fftw_mpi_omp:
`sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_fftw_shared.sh executable_name estimate/measure $THREAD_POW`

### Distributed
All:

- `./distributed_benchmark.sh estimate/measure partition_name`

Executables only: 

- fftw_hpx_loop/fftw_hpx_tasg_agas:
`sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_hpx_dist.sh executable_name estimate/measure $NODES`

- fftw_mpi_threads/fftw_mpi_omp:
`sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_fftw_dist.sh executable_name estimate/measure $NODES $THREADS`

## Contact

alexander.strack@ipvs.uni-stuttgart.de
