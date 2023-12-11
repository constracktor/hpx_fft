#!/bin/bash
################################################################################
# Run benchmark script
################################################################################
LOOP=5
if [[ "$1" == "strong" ]]
then
    # strong
    mkdir result
    BASE_SIZE=16384
    # Compute benchmark script from 2^start to 2^stop
    POW_START=1
    POW_STOP=7
    ###############################
    # HPX loop
    # # shared
    # ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         ./build/fft_hpx_loop_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par
    #     done
    # done
    # # scatter
    # ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         ./build/fft_hpx_loop --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
    #     done
    # done
    ###############################
    # HPX future
    # # shared sync
    # ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         ./build/fft_hpx_task_sync_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE
    #     done
    # done
    # # shared optimized version
    # ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         ./build/fft_hpx_task_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE
    #     done
    # done
    # # shared naive version
    # ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         ./build/fft_hpx_task_naive_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE
    #     done
    # done
    # # shared agas
    # ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         ./build/fft_hpx_task_agas_shared --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE
    #     done
    # done
    # # scatter
    # ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         ./build/fft_hpx_task_agas --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
    #     done
    # done
    ###############################
    # FFTW
    # Threads
    ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE estimate 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fftw_mpi_threads 1 $BASE_SIZE $BASE_SIZE estimate 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fftw_mpi_threads $i  $BASE_SIZE $BASE_SIZE estimate 0 
        done
    done
    # OpenMP
    ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fftw_mpi_omp $i  $BASE_SIZE $BASE_SIZE estimate 0 
        done
    done
    # # MPI
    # mpirun -n 1 ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     mpirun -n 1 ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         mpirun -n $i ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0 
    #     done
    # done
    # HPX
    # FFTW3_HPX_NTHREADS=1 ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE estimate 1
    # for (( j=1; j<$LOOP; j=j+1 ))
    # do
    #     FFTW3_HPX_NTHREADS=1./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE estimate 0
    # done
    # for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    # do
    #     for (( j=0; j<$LOOP; j=j+1 ))
    #     do
    #         FFTW3_HPX_NTHREADS=$i ./build/fftw_hpx $i  $BASE_SIZE $BASE_SIZE estimate 0 
    #     done
    # done
elif [[ "$1" == "weak" ]]
then
    # weak
    mkdir result
    BASE_SIZE=1024
    # Compute benchmark script from 2^start to 2^stop
    POW_START=1
    POW_STOP=4
    ###############################
    # HPX loop
    # shared
    ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --header=true
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fft_hpx_loop_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=par
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fft_hpx_loop_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=par
        done
    done
    # scatter
    ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fft_hpx_loop --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=scatter
        done
    done
    ###############################
    # HPX future
    # shared sync
    ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fft_hpx_task_sync_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fft_hpx_task_sync_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
        done
    done
    # shared optimized
    ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fft_hpx_task_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
        done
    done
    # shared naive
    ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fft_hpx_task_naive_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fft_hpx_task_naive_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
        done
    done
    # shared
    ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fft_hpx_task_agas_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fft_hpx_task_agas_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
        done
    done
    # scatter
    ./build/fft_hpx_task --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fft_hpx_task_agas --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fft_hpx_task_agas --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=scatter
        done
    done
    ###############################
    # FFTW
    # openmp
    ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fftw_mpi_omp $(($i*$i))  $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0 --use-hwthread-cpus 
        done
    done
    # MPI
    mpirun -n 1 ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        mpirun -n 1 ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            mpirun -n $(($i*$i)) ./build/fftw_mpi_omp 1 $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0 --use-hwthread-cpus
        done
    done
    # hpx
    ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE estimate 1
    for (( j=1; j<$LOOP; j=j+1 ))
    do
        ./build/fftw_hpx 1 $BASE_SIZE $BASE_SIZE estimate 0
    done
    for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
    do
        for (( j=0; j<$LOOP; j=j+1 ))
        do
            ./build/fftw_hpx $(($i*$i))  $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0 
        done
    done

else
  echo 'Please specify benchmark: "strong" or "weak"'
  exit 1
fi