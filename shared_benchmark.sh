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
# Compute benchmark script from 2^start to 2^stop
POW_START=1
POW_STOP=7
# get partition
if [[ "$1" == "slurm" ]]
then
    PARTITION="srun -t 1:00:00 --exclusive"
elif [[ "$1" == "node" ]]
then
    PARTITION="mpirun"
else
  echo 'Please specify benchmark: "slurm" or "node"'
  exit 1
fi
###############################
# HPX loop
# shared
$PARTITION ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fft_hpx_loop_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --plan=$FFTW_PLAN
    done
done
# scatter
$PARTITION ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fft_hpx_loop --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
    done
done
##############################
# HPX future
# shared sync
$PARTITION ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fft_hpx_task_sync_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
done
# shared optimized version
$PARTITION ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fft_hpx_task_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
done
# shared naive version
$PARTITION ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fft_hpx_task_naive_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
done
# shared agas
$PARTITION ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fft_hpx_task_agas_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
done
# scatter
$PARTITION ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true --plan=$FFTW_PLAN
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fft_hpx_task_agas --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
    done
done
###############################
# FFTW
# Threads
$PARTITION ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fftw_mpi_threads $i $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
    done
done
# OpenMP
$PARTITION ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION ./build/fftw_mpi_omp $i $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
    done
done
# MPI
$PARTITION -n 1 ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    $PARTITION -n 1 ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $PARTITION -n $i ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
    done
done
# HPX
FFTW3_HPX_NTHREADS=1 $PARTITION ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    FFTW3_HPX_NTHREADS=1 $PARTITION ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        FFTW3_HPX_NTHREADS=$i $PARTITION ./build/fftw_hpx $i  $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
    done
done
