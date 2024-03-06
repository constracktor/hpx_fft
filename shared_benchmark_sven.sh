#!/bin/bash
################################################################################
# Benchmark script for shared memory 
# $1: FFTW planning flag (estimate/measure)
# requires: srun -p risc5 -N 1 -n 1 -c 64 --pty bash
module load mpi/openmpi-riscv64
LOOP=1
THREAD_POW=6
FFTW_PLAN=$1
THREADS=$((2**$THREAD_POW))
cd benchmark
# HPX implementations
# shared only
./run_hpx_shared_node.sh fft_hpx_loop_shared $FFTW_PLAN $THREAD_POW $LOOP 
./run_hpx_shared_node.sh fft_hpx_task_sync_shared $FFTW_PLAN $THREAD_POW $LOOP
./run_hpx_shared_node.sh fft_hpx_task_shared $FFTW_PLAN $THREAD_POW $LOOP
./run_hpx_shared_node.sh fft_hpx_task_naive_shared $FFTW_PLAN $THREAD_POW $LOOP
# FFTW backends
# shared only
#./run_fftw_shared_node.sh fftw_hpx $FFTW_PLAN $THREAD_POW $LOOP
# distributed possible
#./run_fftw_shared_node.sh fftw_mpi_threads $FFTW_PLAN $THREAD_POW $LOOP
#./run_fftw_shared_node.sh fftw_mpi_omp $FFTW_PLAN $THREAD_POW $LOOP

