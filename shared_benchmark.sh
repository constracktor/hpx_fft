#!/bin/bash
################################################################################
# Benchmark script for shared-memory 
cd scripts
# HPX implementations
# shared
sbatch run_hpx_shared.sh fft_hpx_loop_shared estimate
sbatch run_hpx_shared.sh fft_hpx_task_sync_shared estimate
sbatch run_hpx_shared.sh fft_hpx_task_shared estimate
sbatch run_hpx_shared.sh fft_hpx_task_naive_shared estimate
sbatch run_hpx_shared.sh fft_hpx_task_agas_shared estimate
# distributed
sbatch run_hpx_shared.sh fft_hpx_loop estimate
sbatch run_hpx_shared.sh fft_hpx_task_agas estimate
# FFTW backends
# shared
sbatch run_fftw_shared.sh fftw_hpx estimate
# distributed
sbatch run_fftw_shared.sh fftw_mpi_threads estimate
sbatch run_fftw_shared.sh fftw_mpi_omp estimate

