#!/bin/bash
################################################################################
# Benchmark script for shared memory 
# $1: FFTW planning flag (estimate/measure)
# $2: partition (buran/medusa/epyc1)
if [[ "$2" == "buran" ]]
then
    PARTITION=buran
    THREADS=32
elif [[ "$2" == "medusa" ]]
then
    PARTITION=medusa
    THREADS=32
elif [[ "$2" == "epyc1" ]]
then
    PARTITION=ipvs-epcy1
    THREADS=128
else
  echo 'Please specify benchmark: "buran" or "medusa" or "epyc1"'
  exit 1
fi
FFTW_PLAN=$1
cd benchmark
# HPX implementations
# shared only
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_hpx_shared.sh fft_hpx_loop_shared $FFTW_PLAN
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_hpx_shared.sh fft_hpx_task_sync_shared $FFTW_PLAN
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_hpx_shared.sh fft_hpx_task_shared $FFTW_PLAN
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_hpx_shared.sh fft_hpx_task_naive_shared $FFTW_PLAN
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_hpx_shared.sh fft_hpx_task_agas_shared $FFTW_PLAN
# distributed possible
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_hpx_shared.sh fft_hpx_loop $FFTW_PLAN
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_hpx_shared.sh fft_hpx_task_agas $FFTW_PLAN
# FFTW backends
# shared only
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_fftw_shared.sh fftw_hpx $FFTW_PLAN
# distributed possible
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_fftw_shared.sh fftw_mpi_threads $FFTW_PLAN
sbatch -p $PARTITION -N 1 -n 1 -c $THREADS run_fftw_shared.sh fftw_mpi_omp $FFTW_PLAN

