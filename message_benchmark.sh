#!/bin/bash
################################################################################
# Benchmark script for distributed memory 
# $1: parcelport (mpi/lci/tcp/fftw)
# $2: communication scheme (scatter/all_to_all)

# specify parameters
# cluster partition
PARTITION=buran
# shared memory threads
THREADS=48
# run 2D FFT for dimension 2^7 up to dimension 2^(7+SIZE_POW) 
SIZE_POW=7
# set number of runs per programms
LOOP=50
# set FFTW planning flag
FFTW_PLAN=measure

COLLECTIVE=$2
BUILD_DIR=build_$1
# set parcelport specific parameters
module load llvm/17.0.6
cd benchmark_scripts
if [[ "$1" == "fftw" ]]
then
    # FFTW
    sbatch -p $PARTITION -N 2 -n 2 -c $THREADS run_fftw_message.sh $BUILD_DIR/fftw_mpi_threads $FFTW_PLAN $SIZE_POW $THREADS $LOOP
elif  [[ "$1" == "mpi" ]] ||  [[ "$1" == "lci" ]] ||  [[ "$1" == "tcp" ]]
then
    if [[ "$1" == "mpi" ]]
    then
        module load openmpi/5.0.3
    elif [[ "$1" == "lci" ]]
    then
        export LD_LIBRARY_PATH="$(pwd)/../installation_scripts/hpx_1.9_lci/install/lib64:$LD_LIBRARY_PATH"
    fi
    # HPX
    sbatch -p $PARTITION -N 2 -n 2 -c $THREADS run_hpx_message.sh $BUILD_DIR/fft_hpx_loop $FFTW_PLAN $SIZE_POW $COLLECTIVE $LOOP
else
    echo "Specify parcelport: tcp/lci/mpi/fftw"
    exit 1
fi
