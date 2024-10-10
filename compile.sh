#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Preprocessing
################################################################################
# working directory
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"
# build directory
export BUILD_DIR=build_$1
## MODIFY: Adjust compiler
module load llvm/17.0.6 
## MODIFY: Adjust paths and modules for all parceloprts
if [[ "$1" == "mpi" ]]
then
    export HPX_DIR="${ROOT}/installation_scripts/hpx_1.9_mpi/install/lib64"
    export FFTW_DIR="${ROOT}/installation_scripts/fftw_seq/install"
    export TARGET=fft_hpx_loop
    module load openmpi/5.0.3
elif [[ "$1" == "lci" ]]
then
    export HPX_DIR="${ROOT}/installation_scripts/hpx_1.9_lci/install/lib64"
    export FFTW_DIR="${ROOT}/installation_scripts/fftw_seq/install"
    export TARGET=fft_hpx_loop
elif [[ "$1" == "tcp" ]]
then
    export HPX_DIR="${ROOT}/installation_scripts/hpx_1.9_tcp/install/lib64"
    export FFTW_DIR="${ROOT}/installation_scripts/fftw_seq/install"
    export TARGET=fft_hpx_loop
elif [[ "$1" == "fftw" ]]
then
    export HPX_DIR="${ROOT}/installation_scripts/hpx_1.9_mpi/install/lib64"
    export FFTW_DIR="${ROOT}/installation_scripts/fftw_threads_mpi/install"
    export TARGET=fftw_mpi_threads
    module load openmpi/5.0.3
else
  echo 'Please specify parcelport'
  exit 1
fi
export CMAKE_COMMAND=cmake
export PKG_CONFIG_PATH="$FFTW_DIR/lib64/pkgconfig:$FFTW_DIR/lib/pkgconfig:$PKG_CONFIG_PATH"

################################################################################
# Compilation
################################################################################
rm -rf $BUILD_DIR && mkdir $BUILD_DIR && cd $BUILD_DIR
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX"
make VERBOSE=1  -j $(grep -c ^processor /proc/cpuinfo) $TARGET
