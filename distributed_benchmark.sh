#!/bin/bash
################################################################################
# Benchmark script for distributed memory 
# $1: parcelport (mpi/lci/tcp/shmem/gasnet)
# $2: communication scheme (scatter/all_to_all)

# specify parameters
# MODIFY: Adjust compiler
PARTITION=buran
# MODIFY: Adjust shared memory threads
THREADS=48
# run from 2^0 up to 2^NODES_POW nodes
NODES_POW=4
NODES=$((2**$NODES_POW))
# # MODIFY: Adjust number of runs per programms
LOOP=50
# set FFTW planning flag
FFTW_PLAN=measure

COLLECTIVE=$2
BUILD_DIR=build_$1 
# MODIFY: Adjust parcelport specific parameters
if [[ "$1" == "mpi" ]]
then
    module load llvm/17.0.1
    module load openmpi
elif [[ "$1" == "lci" ]]
then
    module load llvm/17.0.1
    export LD_LIBRARY_PATH=/home/alex/test_chris/hpx_fft/hpx_installations/hpx_1.9_lci/install/lib64:$LD_LIBRARY_PATH
elif [[ "$1" == "tcp" ]]
then
    module load llvm/17.0.1
elif [[ "$1" == "shmem" ]]
then
    module load gcc/12.3.0
elif [[ "$1" == "gasnet" ]]
then
    module load llvm/17.0.1
else
  echo 'Please specify parcelport and collective'
  exit 1
fi
cd benchmark
# HPX
sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_hpx_dist.sh $BUILD_DIR/fft_hpx_loop $FFTW_PLAN $NODES_POW $COLLECTIVE $LOOP
# FFTW
#sbatch -p $PARTITION -N $NODES -n $NODES -c $THREADS run_fftw_dist.sh $BUILD_DIR/fftw_mpi_threads $FFTW_PLAN $NODES $THREADS $LOOP