#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Command-line options (currently disabled)
################################################################################
# Compute benchmark script from 2^start to 2^stop
# POW_START=$1
# POW_STOP=$2
LOOP=5

################################################################################
# Run benchmark script
################################################################################

###################################################################################################
# strong scaling
BASE_SIZE=16384
POW_START=1
POW_STOP=4
THREADS=24

# # scatter loop
# srun -p buran -N 1 -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     srun -p buran -N 1 -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter 
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         srun -p buran -N $i -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter 
#     done
# done

# scatter task agas
# srun -p buran -N 1 -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     srun -p buran -N 1 -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter 
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         srun -p buran -N $i -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter 
#     done
# done

# # MPI + OpenMP
# srun -p buran -N 1 -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE estimate 1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     srun -p buran -N 1 -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE estimate 0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         srun -p buran -N $i -t 1:00:00 --exclusive -n $i ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE estimate 0 
#     done
# done

# # MPI total
# srun -p buran -N 1 -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     srun -p buran -N 1 -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         srun -p buran -N $i -t 1:00:00 --exclusive -n $(($i*$THREADS)) ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0 
#     done
# done

###################################################################################################
# weak scaling
BASE_SIZE=16384
POW_START=1
POW_STOP=2

# scatter loop
srun -p buran -N 1 -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p buran -N 1 -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter 
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N $(($i*$i)) -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$(($i*$BASE_SIZE))  --ny=$(($i*$BASE_SIZE))  --run=scatter 
    done
done

# scatter task agas
srun -p buran -N 1 -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p buran -N 1 -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter 
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N $(($i*$i)) -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$(($i*$BASE_SIZE))  --ny=$(($i*$BASE_SIZE))  --run=scatter 
    done
done

# MPI + OpenMP
srun -p buran -N 1 -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE estimate 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p buran -N 1 -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE estimate 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N $(($i*$i)) -t 1:00:00 --exclusive -n $(($i*$i)) ./build/fftw_mpi_omp $THREADS $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0 --use-hwthread-cpus
    done
done

# MPI total
srun -p buran -N 1 -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    srun -p buran -N 1 -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N $(($i*$i)) -t 1:00:00 --exclusive -n $(($i*$i*$THREADS)) ./build/fftw_mpi_omp 1 $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0 --use-hwthread-cpus
    done
done

###################################################################################################