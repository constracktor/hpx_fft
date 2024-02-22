#!/bin/bash
################################################################################
# Run benchmark script for distributed slurm
################################################################################

# configure gasnet
#
export GASNET_SPAWNFN=C
export GASNET_CSPAWN_CMD=""

# strong scaling
mkdir result
mkdir plans
LOOP=1
BASE_SIZE=16384
FFTW_PLAN=estimate
# Compute benchmark script from 2^start to 2^stop
POW_START=1
# get partition
if [[ "$1" == "buran" ]]
then
    PARTITION=buran
    THREADS=48
    POW_STOP=4
elif [[ "$1" == "medusa" ]]
then
    PARTITION=medusa
    THREADS=40
    POW_STOP=3
else
  echo 'Please specify benchmark: "buran" or "medusa"'
  exit 1
fi
###############################
# HPX loop
# scatter loop
srun -p $PARTITION -N 1 -c $THREADS  -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p $PARTITION -N 1 -c $THREADS  -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p $PARTITION -N $i -c $THREADS  -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
    done
done
###############################
# HPX future
# scatter task agas
srun -p $PARTITION -N 1 -c $THREADS  -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p $PARTITION -N 1 -c $THREADS  -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p $PARTITION -N $i -c $THREADS  -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
    done
done
###############################
# FFTW
# MPI + OpenMP
srun -p $PARTITION -N 1 -c $THREADS  -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p $PARTITION -N 1 -c $THREADS  -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p $PARTITION -N $i -c $THREADS  -t 1:00:00 --exclusive -n $i ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0 
    done
done
# MPI + Threads
srun -p $PARTITION -N 1 -c $THREADS  -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p $PARTITION -N 1 -c $THREADS  -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p $PARTITION -N $i -c $THREADS  -t 1:00:00 --exclusive -n $i ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0 
    done
done
# pure MPI
srun -p $PARTITION -N 1 -c 1 -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p $PARTITION -N 1 -c 1 -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p $PARTITION -N $i -c 1 -t 1:00:00 --exclusive -n $(($i*$THREADS)) ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0 
    done
done
# HPX
# tbd.
