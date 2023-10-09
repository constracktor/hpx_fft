#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Variables
################################################################################
export APEX_SCREEN_OUTPUT=1 APEX_EXPORT_CSV=1 APEX_TASKGRAPH_OUTPUT=1 APEX_TASKTREE_OUTPUT=1
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/.."
#export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_dist_v1.8.1"
#export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_apex_sgscl1_v1.9.1"
export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_apex_epyc2_v.1.9.1"
export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib
export FFTW_DIR="${ROOT}/fft_installations/fftw_seq/install/lib/"  
# for mpi version
export CXX=${HPXSC_ROOT}/build/openmpi/bin/mpic++ 

################################################################################
# Command-line options
################################################################################
# Compute benchmark script from 2^start to 2^stop
# POW_START=$1
# POW_STOP=$2
LOOP=5

################################################################################
# Compile code
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DFFTW3_DIR="${FFTW_DIR}/cmake/fftw3"
#make VERBOSE=1
make -j 8
cd ..
################################################################################
# Run benchmark script
################################################################################
#./fft_hpx --nx=8192 --ny=8192 --hpx:threads=1 --run=task_scatter
# weak scaling
BASE_SIZE=1024
POW_START=1
POW_STOP=4
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
#         ./build/fft_hpx_loop_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=par
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
#         ./build/fft_hpx_loop --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=scatter
#     done
# done
# # all to all
# ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all --header=true
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         ./build/fft_hpx_loop --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=all_to_all
#     done
# done
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
#         mpirun -n $(($i*$i)) ./build/fftw_mpi_omp 1 $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0 --use-hwthread-cpus
#     done
# done


# ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true
# for (( i=2**POW_START; i<=2**10; i=i*2 ))
# do
#     ./build/fft_hpx_loop --hpx:threads=$i --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=scatter
# done

# ./build/fft_hpx_loop --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all --header=true
# for (( i=2**POW_START; i<=2**10; i=i*2 ))
# do
#     ./build/fft_hpx_loop --hpx:threads=$i --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=all_to_all
# done

# ./build/fft_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
# for (( i=2**POW_START; i<=2**10; i=i*2 ))
# do
#     ./build/fft_mpi_omp $i $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0
# done

# ./build/fft_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
# for (( i=2**POW_START; i<=2**10; i=i*2 ))
# do
#     ./build/fft_mpi_omp $i $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0
# done

#srun -N 1 ./hello hpx:threads=1
# rm -rf result
# mkdir result

# # for (( i=2**POW_START; i<=2**10; i=i*2 ))
# # do
    
# #     ./fftw_2d --n=$i --f="estimate" --l=$((10*$LOOP))
# # done
# # for (( i=2**11; i<=2**POW_STOP; i=i*2 ))
# # do
# #     ./fftw_2d --n=$i --f="estimate" --l=$LOOP
# # done

# for (( i=2**POW_START; i<=2**10; i=i*2 ))
# do
    
#     ./fftw_2d --n=$i --f="patient" --l=$((10*$LOOP))
# done
# for (( i=2**11; i<=2**POW_STOP; i=i*2 ))
# do
#     ./fftw_2d --n=$i --f="patient" --l=$LOOP
# done