#!/bin/bash
################################################################################
# Benchmark script for distributed memory 
# $1: FFTW planning flag (estimate/measure)
# $2: partition (buran/medusa with mpi/lci/shmem)
# $3 communication scheme (scatter/all_to_all)
if [[ "$2" == "buran_mpi" ]]
then
    PARTITION=buran
    THREADS=48
    NODES_POW=4
    BUILD_DIR=build_buran_mpi
elif [[ "$2" == "buran_lci" ]]
then
    PARTITION=buran
    THREADS=48
    NODES_POW=4
    BUILD_DIR=build_buran_lci
elif [[ "$2" == "buran_shmem" ]]
then
    PARTITION=buran
    THREADS=48
    NODES_POW=4
    BUILD_DIR=build_buran_shmem
elif [[ "$2" == "medusa_mpi" ]]
then
    PARTITION=medusa
    THREADS=40
    NODES_POW=3
    BUILD_DIR=build_medusa_mpi
elif [[ "$2" == "medusa_lci" ]]
then
    PARTITION=medusa
    THREADS=40
    NODES_POW=3
    BUILD_DIR=build_medusa_lci
elif [[ "$2" == "medusa_shmem" ]]
then
    PARTITION=medusa
    THREADS=40
    NODES_POW=3
    BUILD_DIR=build_medusa_shmem
else
  echo 'Please specify partition and parcelport'
  exit 1
fi
FFTW_PLAN=$1
NODES=$((2**$NODES_POW))
COLLECTIVE=$3
# load modules
module load openmpi
cd benchmark
# HPX implementations
sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_hpx_dist.sh $BUILD_DIR/fft_hpx_loop $FFTW_PLAN $NODES_POW $COLLECTIVE
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_hpx_dist.sh $BUILD_DIR/fft_hpx_task_agas $FFTW_PLAN $NODES_POW $COLLECTIVE
# FFTW backends
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_fftw_dist.sh $BUILD_DIR/fftw_mpi_threads $FFTW_PLAN $NODES $THREADS
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_fftw_dist.sh $BUILD_DIR/fftw_mpi_omp $FFTW_PLAN $NODES $THREADS