#!/bin/bash
# Benchmark script for shared memory strong scaling without slurm
# $1: Executable name
# $2: FFTW planning flag (estimate/measure)
# $3: Number of threads
# $4: Number of runs
# Parameters
LOOP=$4
POW_START=1
POW_STOP=$3
BASE_SIZE=16384
# Get run command
COMMAND="mpirun -n 1"
EXECUTABLE="../$1"
ARGUMENTS="$BASE_SIZE $BASE_SIZE $2"
# Create directories to store data
mkdir result
mkdir plans
# Strong scaling loop from 2^pow_start to 2^pow_stop cores
FFTW3_HPX_NTHREADS=1 $COMMAND $EXECUTABLE 1 $ARGUMENTS 1 
for (( j=1; j<$LOOP; j=j+1 ))
do
    FFTW3_HPX_NTHREADS=1 $COMMAND $EXECUTABLE 1 $ARGUMENTS 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        FFTW3_HPX_NTHREADS=$i $COMMAND $EXECUTABLE $i $ARGUMENTS 0
    done
done