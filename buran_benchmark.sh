#!/bin/bash
################################################################################
# Run benchmark script
################################################################################
module load openmpi
if [[ "$1" == "strong" ]]
then
    # strong
    mkdir result
    mkdir plans
    BASE_SIZE=32768
    OFFSET=0
    LOOP=7
    FFTW_PLAN=measure
    THREADS=48
    # Compute benchmark script from 2^start to 2^stop
    POW_START=1
    POW_STOP=4
    ###############################
    # HPX loop
    # scatter loop
    srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -p buran -N $i -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
        done
    done
    ###############################
    # HPX future
    # scatter task agas
    srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -p buran -N $i -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
        done
    done
    ###############################
    # FFTW
    # MPI + OpenMP
    srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -N 1 -c $THREADS ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -N 1 -c $THREADS ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -p buran -N $i -t 1:00:00 --exclusive -n $i ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0 
        done
    done
    # MPI + Threads
    srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -N 1 -c $THREADS ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -N 1 -c $THREADS ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -p buran -N $i -t 1:00:00 --exclusive -n $i ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0 
        done
    done
    # MPI total
    srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -p buran -N $i -t 1:00:00 --exclusive -n $(($i*$THREADS)) ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0 
        done
    done
    # HPX
    # tbd.
elif [[ "$1" == "weak" ]]
then
    # weak
    echo '"weak" requires update'
    exit 1
    # mkdir result
    # BASE_SIZE=1024
    # # Compute benchmark script from 2^start to 2^stop
    # POW_START=1
    # POW_STOP=2
    ###############################
    # HPX loop
    # # scatter loop
    # srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter 
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -p buran -N $(($i*$i)) -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$(($i*$BASE_SIZE))  --ny=$(($i*$BASE_SIZE))  --run=scatter 
    #     done
    # done
    ###############################
    # HPX future
    # # scatter task agas
    # srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter 
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -p buran -N $(($i*$i)) -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$(($i*$BASE_SIZE))  --ny=$(($i*$BASE_SIZE))  --run=scatter 
    #     done
    # done
    ###############################
    # FFTW
    # # MPI + OpenMP
    # srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -N 1 -c $THREADS ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -N 1 -c $THREADS ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -p buran -N $(($i*$i)) -t 1:00:00 --exclusive -n $(($i*$i)) ./build/fftw_mpi_omp $THREADS $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) $FFTW_PLAN  0 --use-hwthread-cpus
    #     done
    # done

    # # MPI total
    # srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -p buran -N 1 -c $THREADS -t 1:00:00 --exclusive -n $THREADS ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -p buran -N $(($i*$i)) -t 1:00:00 --exclusive -n $(($i*$i*$THREADS)) ./build/fftw_mpi_omp 1 $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) $FFTW_PLAN  0 --use-hwthread-cpus
    #     done
    # done
    # HPX
    #tbd.

else
  echo 'Please specify benchmark: "strong" or "weak"'
  exit 1
fi