#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Variables
################################################################################
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/.."
export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_dist_v1.8.1"
export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib
export FFTW_DIR="${ROOT}/fft_installations/fftw_seq/install/lib/"  


################################################################################
# Command-line options
################################################################################
# Compute benchmark script from 2^start to 2^stop
POW_START=$1
POW_STOP=$2
LOOP=$3

MULT_LOOP=10*$3
################################################################################
# Compile code
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DFFTW3_DIR="${FFTW_DIR}/cmake/fftw3"
make

################################################################################
# Run benchmark script
################################################################################
mkdir result

for (( i=2**POW_START; i<=2**10; i=i*2 ))
do
    
    ./fftw_2d --n=$i --f="estimate" --l=$MULT_LOOP
done
for (( i=2**10; i<=2**POW_STOP; i=i*2 ))
do
    ./fftw_2d --n=$i --f="estimate" --l=$LOOP
done
