#!/bin/bash
################################################################################
# Benchmark script for distributed memory 
# $1: FFTW planning flag (estimate/measure)
# $2: partition (buran/medusa)
if [[ "$2" == "buran_mpi" ]]
then
    PARTITION=buran
    THREADS=48
    NODES=16
    NODES_POW=4
    BUILD_DIR=build_buran_mpi
elif [[ "$2" == "buran_lci" ]]
then
    PARTITION=buran
    THREADS=48
    NODES=16
    NODES_POW=4
    BUILD_DIR=build_buran_lci
elif [[ "$2" == "medusa" ]]
then
    PARTITION=medusa
    THREADS=40
    NODES=8
else
  echo 'Please specify benchmark: "buran" or "medusa"'
  exit 1
fi
FFTW_PLAN=$1
# load modules
module load openmpi
cd benchmark
# HPX implementations
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_hpx_dist.sh $BUILD_DIR/fft_hpx_loop $FFTW_PLAN $NODES_POW
sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_hpx_dist.sh $BUILD_DIR/fft_hpx_task_agas $FFTW_PLAN $NODES_POW
# FFTW backends
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_fftw_dist.sh $BUILD_DIR/fftw_mpi_threads $FFTW_PLAN $NODES $THREADS
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_fftw_dist.sh $BUILD_DIR/fftw_mpi_omp $FFTW_PLAN $NODES $THREADS