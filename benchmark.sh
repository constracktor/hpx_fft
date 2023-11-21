#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Variables
################################################################################
#export APEX_SCREEN_OUTPUT=1 APEX_EXPORT_CSV=1 APEX_TASKGRAPH_OUTPUT=1 APEX_TASKTREE_OUTPUT=1
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/.."
#export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_apex_sgscl1_v1.9.1"
# epyc2
export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_apex_epyc2_v.1.9.1"
export FFTW_DIR="${ROOT}/fft_installations/fftw_seq/install/lib/"  
# buran
#export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1"
#export FFTW_DIR="${HPXSC_ROOT}/build/fftw/lib64/"
#
export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib
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

###################################################################################################
# strong scaling
BASE_SIZE=1024
POW_START=1
POW_STOP=8
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
#         ./build/fft_hpx_loop --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all
#     done
# done
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
# # shared
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
# # all to all
# ./build/fft_hpx_task --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all --header=true
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     ./build/fft_hpx_task --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         ./build/fft_hpx_task --hpx:threads=$i --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all
#     done
# done
# # openmp
# ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         ./build/fftw_mpi_omp $i  $BASE_SIZE $BASE_SIZE estimate 0 
#     done
# done
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
###################################################################################################


###################################################################################################
# weak scaling
# BASE_SIZE=1024
# POW_START=1
# POW_STOP=4
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
# # shared
# ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --header=true
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     ./build/fft_hpx_task_shared --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         ./build/fft_hpx_task_shared --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE))
#     done
# done
# # scatter
# ./build/fft_hpx_task --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter --header=true
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     ./build/fft_hpx_task --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=scatter
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         ./build/fft_hpx_task --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=scatter
#     done
# done
# # all to all
# ./build/fft_hpx_task --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all --header=true
# for (( j=1; j<$task; j=j+1 ))
# do
#     ./build/fft_hpx_task --hpx:threads=1 --nx=$BASE_SIZE --ny=$BASE_SIZE --run=all_to_all
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         ./build/fft_hpx_task --hpx:threads=$(($i*$i)) --nx=$(($i*$BASE_SIZE)) --ny=$(($i*$BASE_SIZE)) --run=all_to_all
#     done
# done
# # openmp
# ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     ./build/fftw_mpi_omp 1 $BASE_SIZE $BASE_SIZE estimate 0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         ./build/fftw_mpi_omp $(($i*$i))  $(($i*$BASE_SIZE)) $(($i*$BASE_SIZE)) estimate 0 --use-hwthread-cpus 
#     done
# done
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
###################################################################################################