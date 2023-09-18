#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Variables
################################################################################
export APEX_SCREEN_OUTPUT=1 APEX_EXPORT_CSV=1 APEX_TASKGRAPH_OUTPUT=1
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/.."
#export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_dist_v1.8.1"
export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_apex_sgscl1_v1.9.1"
export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib
export FFTW_DIR="${ROOT}/fft_installations/fftw_seq/install/lib/"  
# for mpi version
export CXX=${HPXSC_ROOT}/build/openmpi/bin/mpic++ 

################################################################################
# Command-line options
################################################################################
# Compute benchmark script from 2^start to 2^stop
POW_START=$1
POW_STOP=$2
LOOP=$3

################################################################################
# Compile code
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DFFTW3_DIR="${FFTW_DIR}/cmake/fftw3"
#make VERBOSE=1
make -j 8

################################################################################
# Run benchmark script
################################################################################
./fft_hpx --nx=8192 --ny=8192 --hpx:threads=1 --run=task_scatter

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