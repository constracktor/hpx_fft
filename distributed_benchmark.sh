#!/bin/bash
################################################################################
# Benchmark script for distributed memory 
# $1: FFTW planning flag (estimate/measure)
# load modules
module load openmpi
cd benchmark
# HPX implementations
sbatch run_hpx_dist.sh fft_hpx_loop $1
sbatch run_hpx_dist.sh fft_hpx_task_agas $1
# FFTW backends
sbatch run_fftw_dist.sh fftw_mpi_threads $1
sbatch run_fftw_dist.sh fftw_mpi_omp $1