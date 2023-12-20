#!/bin/bash
################################################################################
# Benchmark script for ipvs-epyc1 (dual-socket AMD EPYC 7742) behind slurm
########
if [[ "$1" == "strong" ]]
then
    # strong
    mkdir result
    mkdir plans
    BASE_SIZE=16384
    OFFSET=0
    LOOP=5
    FFTW_PLAN=measure
    # Compute benchmark script from 2^start to 2^stop
    POW_START=1
    POW_STOP=7
    ###############################
    # HPX loop
    # shared
    srun -t 1:00:00 --exclusive ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --plan=$FFTW_PLAN
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fft_hpx_loop_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --plan=$FFTW_PLAN
        done
    done
    # scatter
    srun -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
        done
    done
    ##############################
    # HPX future
    # shared sync
    srun -t 1:00:00 --exclusive ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fft_hpx_task_sync_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
        done
    done
    # shared optimized version
    srun -t 1:00:00 --exclusive ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fft_hpx_task_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
        done
    done
    # shared naive version
    srun -t 1:00:00 --exclusive ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fft_hpx_task_naive_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
        done
    done
    # shared agas
    srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --plan=$FFTW_PLAN
        done
    done
    # scatter
    srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true --plan=$FFTW_PLAN
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --plan=$FFTW_PLAN
        done
    done
    ###############################
    # FFTW
    # Threads
    srun -t 1:00:00 --exclusive ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fftw_mpi_threads $(($i-$OFFSET)) $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
        done
    done
    # OpenMP
    srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp $(($i-$OFFSET)) $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
        done
    done
    # MPI
    srun -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        srun -t 1:00:00 --exclusive -n 1 ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            srun -t 1:00:00 --exclusive -n $(($i-$OFFSET)) ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
        done
    done
    # HPX
    FFTW3_HPX_NTHREADS=1 srun -t 1:00:00 --exclusive ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        FFTW3_HPX_NTHREADS=1 srun -t 1:00:00 --exclusive ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            FFTW3_HPX_NTHREADS=$i srun -t 1:00:00 --exclusive ./build/fftw_hpx $(($i-$OFFSET))  $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0 
        done
    done
elif [[ "$1" == "weak" ]]
then
    echo '"weak" requires update'
    exit 1
    # # weak
    # mkdir result
    # BASE_SIZE=1024
    # # Compute benchmark script from 2^start to 2^stop
    # POW_START=1
    # POW_STOP=4
    # ###############################
    # # HPX loop
    # # shared
    # srun -t 1:00:00 --exclusive ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fft_hpx_loop_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=par
    #     done
    # done
    # # scatter
    # srun -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=scatter
    #     done
    # done
    # ###############################
    # # HPX future
    # # shared sync
    # srun -t 1:00:00 --exclusive ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fft_hpx_task_sync_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
    #     done
    # done
    # # shared optimized
    # srun -t 1:00:00 --exclusive ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fft_hpx_task_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
    #     done
    # done
    # # shared naive
    # srun -t 1:00:00 --exclusive ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fft_hpx_task_naive_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
    #     done
    # done
    # # shared
    # srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
    #     done
    # done
    # # scatter
    # srun -t 1:00:00 --exclusive ./build/fft_hpx_task --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=scatter
    #     done
    # done
    # ###############################
    # # FFTW
    # # openmp
    # srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp $(($i*$i))  $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) $FFTW_PLAN 0 --use-hwthread-cpus 
    #     done
    # done
    # # MPI
    # mpirun -n 1 srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     mpirun -n 1 srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         mpirun -n $(($i*$i)) srun -t 1:00:00 --exclusive ./build/fftw_mpi_omp 1 $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) $FFTW_PLAN 0 --use-hwthread-cpus
    #     done
    # done
    # # hpx
    # srun -t 1:00:00 --exclusive ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 1
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     srun -t 1:00:00 --exclusive ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE $FFTW_PLAN 0
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         srun -t 1:00:00 --exclusive ./build/fftw_hpx $(($i*$i))  $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) $FFTW_PLAN 0 
    #     done
    # done
else
  echo 'Please specify benchmark: "strong" or "weak"'
  exit 1
fi