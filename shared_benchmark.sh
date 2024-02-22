#!/bin/bash
################################################################################
# Benchmark script for shared-memory behind slurm or on node directly
########
# strong scaling
mkdir result
mkdir plans
LOOP=1
BASE_SIZE=16384
FFTW_PLAN=estimate
#FFTW_PLAN=measure
# Compute benchmark script from 2^start to 2^stop
POW_START=1
POW_STOP=7
PARTITION=medusa
# get run command
if [[ "$1" == "slurm" ]]
then
    COMMAND="srun -p $PARTITION -N 1 -n 1 -c $((2**$POW_STOP)) -t 1:00:00 --exclusive"
elif [[ "$1" == "node" ]]
then
    COMMAND="mpirun"
else
  echo 'Please specify benchmark: "slurm" or "node"'
  exit 1
fi
###############################
# HPX loop shared
# core scaling
$COMMAND ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fft_hpx_loop_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --plan=$FFTW_PLAN
    done
done