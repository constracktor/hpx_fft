#!/bin/bash
################################################################################
# Benchmark script for distributed memory 
# $1: FFTW planning flag (estimate/measure)
# $2: partition (buran/medusa with mpi/lci/shmem)
# $3 communication scheme (scatter/all_to_all)
if [[ "$2" == "buran_mpi" ]] || [[ "$2" == "buran_lci" ]]
then
    PARTITION=buran
    THREADS=48
    NODES_POW=4
elif [[ "$2" == "buran_shmem" ]] || [[ "$2" == "buran_shmem_ucx" ]] || [[ "$2" == "buran_shmem_libfabric" ]]
then
    module load llvm/17.0.1
    PARTITION=buran
    THREADS=48
    NODES_POW=4
elif [[ "$2" == "medusa_mpi" ]] || [[ "$2" == "medusa_lci" ]]
then
    PARTITION=medusa
    THREADS=40
    NODES_POW=3
elif [[ "$2" == "medusa_shmem" ]] || [[ "$2" == "medusa_shmem_ucx" ]] || [[ "$2" == "medusa_shmem_libfabric" ]]
then
    module load llvm/17.0.1
    PARTITION=medusa
    THREADS=40
    NODES_POW=3
else
  echo 'Please specify partition and parcelport'
  exit 1
fi
LOOP=1
#50
FFTW_PLAN=$1
NODES=$((2**$NODES_POW))
COLLECTIVE=$3
BUILD_DIR=build_$2
# load modules
module load openmpi
cd benchmark
# HPX implementations
sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_hpx_dist.sh $BUILD_DIR/fft_hpx_loop $FFTW_PLAN $NODES_POW $COLLECTIVE $LOOP
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_hpx_dist.sh $BUILD_DIR/fft_hpx_task_agas $FFTW_PLAN $NODES_POW $COLLECTIVE $LOOP
# FFTW backends
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_fftw_dist.sh $BUILD_DIR/fftw_mpi_threads $FFTW_PLAN $NODES $THREADS $LOOP
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_fftw_dist.sh $BUILD_DIR/fftw_mpi_omp $FFTW_PLAN $NODES $THREADS $LOOP