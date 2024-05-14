#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Preprocessing
################################################################################
# working directory
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/"
# build directory
export BUILD_DIR=build_$1
# MODIFY: Adjust compiler
module load llvm/17.0.1
# MODIFY: Adjust paths and modules for all parceloprts
if [[ "$1" == "mpi" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_1.9_mpi/install/lib64"
    module load openmpi
elif [[ "$1" == "lci" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_1.9_lci/install/lib64"
elif [[ "$1" == "tcp" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_1.9_tcp/install/lib64"
elif [[ "$1" == "shmem" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_dev_shmem/install/lib64"
    #module load openmpi
    module load gcc/12.3.0
elif [[ "$1" == "gasnet" ]]
then
    echo "Add HPX_DIR for Gasnet parcelport"
else
  echo 'Please specify parcelport'
  exit 1
fi
export CMAKE_COMMAND=cmake
# FFTW path
#export FFTW_DIR="$ROOT/fftw_installations/fftw_threads_mpi/install/lib"
export FFTW_DIR="$ROOT/fftw_installations/fftw_cmake/install/lib"
export PKG_CONFIG_PATH="$FFTW_DIR/pkgconfig":$PKG_CONFIG_PATH

################################################################################
# Compilation
################################################################################
rm -rf $BUILD_DIR && mkdir $BUILD_DIR && cd $BUILD_DIR
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX"
make VERBOSE=1 -j $(grep -c ^processor /proc/cpuinfo)
 
