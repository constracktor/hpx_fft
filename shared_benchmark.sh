#!/bin/bash
################################################################################
# Benchmark script for shared-memory 
cd scripts
# HPX implementations
# shared
sbatch run_hpx_shared.sh fft_hpx_loop_shared
sbatch run_hpx_shared.sh fft_hpx_task_sync_shared
sbatch run_hpx_shared.sh fft_hpx_task_shared
sbatch run_hpx_shared.sh fft_hpx_task_naive_shared
sbatch run_hpx_shared.sh fft_hpx_task_agas_shared
# distributed
sbatch run_hpx_shared.sh fft_hpx_loop
sbatch run_hpx_shared.sh fft_hpx_task_agas
# FFTW backends
# # Threads
# $COMMAND ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     $COMMAND ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         $COMMAND ./build/fftw_mpi_threads $i $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
#     done
# done
# # OpenMP
# $COMMAND ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     $COMMAND ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         $COMMAND ./build/fftw_mpi_omp $i $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
#     done
# done
# # MPI
# $COMMAND -n 1 ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     $COMMAND -n 1 ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         $COMMAND -n $i ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
#     done
# done
# # HPX
# FFTW3_HPX_NTHREADS=1 $COMMAND ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     FFTW3_HPX_NTHREADS=1 $COMMAND ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         FFTW3_HPX_NTHREADS=$i $COMMAND ./build/fftw_hpx $i  $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
#     done
# done