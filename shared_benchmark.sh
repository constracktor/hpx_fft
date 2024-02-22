#!/bin/bash
################################################################################
# Benchmark script for shared-memory behind slurm or on node directly
########
# strong scaling
mkdir result
mkdir plans
LOOP=50
BASE_SIZE=16384
FFTW_PLAN=estimate
#FFTW_PLAN=measure
# Compute benchmark script from 2^start to 2^stop
POW_START=1
POW_STOP=7
# get run command
if [[ "$1" == "slurm" ]]
then
    COMMAND="srun -N 1 -n 1 -c $((2**$POW_STOP)) -t 1:00:00 --exclusive"
elif [[ "$1" == "node" ]]
then
    COMMAND="mpirun"
else
  echo 'Please specify benchmark: "slurm" or "node"'
  exit 1
fi
###############################
# HPX loop
# shared
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
# scatter
$COMMAND ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fft_hpx_loop --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
    done
done
##############################
# HPX future
# shared sync
$COMMAND ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fft_hpx_task_sync_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
done
# shared optimized version
$COMMAND ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fft_hpx_task_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
done
# shared naive version
$COMMAND ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fft_hpx_task_naive_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
done
# shared agas
$COMMAND ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fft_hpx_task_agas_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
done
# scatter
$COMMAND ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fft_hpx_task_agas --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
    done
done
###############################
# FFTW
# Threads
$COMMAND ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fftw_mpi_threads $i $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
    done
done
# OpenMP
$COMMAND ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND ./build/fftw_mpi_omp $i $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
    done
done
# MPI
$COMMAND -n 1 ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND -n 1 ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND -n $i ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
    done
done
# HPX
FFTW3_HPX_NTHREADS=1 $COMMAND ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    FFTW3_HPX_NTHREADS=1 $COMMAND ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        FFTW3_HPX_NTHREADS=$i $COMMAND ./build/fftw_hpx $i  $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
    done
done