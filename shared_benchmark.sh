#!/bin/bash
################################################################################
# Benchmark script for shared memory 
# $1: FFTW planning flag (estimate/measure)
cd benchmark
# HPX implementations
# shared only
sbatch run_hpx_shared.sh fft_hpx_loop_shared $1
sbatch run_hpx_shared.sh fft_hpx_task_sync_shared $1
sbatch run_hpx_shared.sh fft_hpx_task_shared $1
sbatch run_hpx_shared.sh fft_hpx_task_naive_shared $1
sbatch run_hpx_shared.sh fft_hpx_task_agas_shared $1
# distributed possible
sbatch run_hpx_shared.sh fft_hpx_loop $1
sbatch run_hpx_shared.sh fft_hpx_task_agas $1
# FFTW backends
# shared only
sbatch run_fftw_shared.sh fftw_hpx $1
# distributed possible
sbatch run_fftw_shared.sh fftw_mpi_threads $1
sbatch run_fftw_shared.sh fftw_mpi_omp $1

