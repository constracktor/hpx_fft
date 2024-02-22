#!/bin/bash
################################################################################
# Benchmark script for shared-memory behind slurm or on node directly
########
# strong scaling
mkdir result
mkdir plans
LOOP=1
BASE_SIZE=1024
FFTW_PLAN=estimate
#FFTW_PLAN=measure
# Compute benchmark script from 2^start to 2^stop * BASE_SIZE
POW_START=1
POW_STOP=5
THREADS=40
PARTITION=medusa
# get run command
if [[ "$1" == "slurm" ]]
then
    COMMAND="srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive"
elif [[ "$1" == "node" ]]
then
    COMMAND="mpirun"
else
  echo 'Please specify benchmark: "slurm" or "node"'
  exit 1
fi
###############################
# HPX loop shared
# data scaling
$COMMAND ./build/fft_hpx_loop_shared --hpx:threads=$THREADS --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fft_hpx_loop_shared --hpx:threads=$THREADS --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fft_hpx_loop_shared --hpx:threads=$THREADS --nx=$((i * $BASE_SIZE)) --ny=$(($i * $BASE_SIZE)) --run=par --plan=$FFTW_PLAN
    done
done