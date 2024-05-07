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
# compiler    
module load llvm/17.0.1
# export paths
if [[ "$1" == "mpi" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_1.9_mpi/install/lib64"
    module load openmpi



    # module load gcc/11.2.1
    # # HPX lib directory
    # export HPX_DIR="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/hpx/build/lib"
    # # MPI compiler
    # export CXX="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/openmpi/bin/mpic++"
    # module load openmpi
elif [[ "$1" == "lci" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_1.9_lci/install/lib64"
elif [[ "$1" == "tcp" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_1.9_tcp/install/lib64"
elif [[ "$1" == "shmem" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_dev_shmem/install/lib"
    module load openmpi
    export C_COMPILER=/tbd.
elif [[ "$1" == "gasnet" ]]
then
    export HPX_DIR="${ROOT}/hpx_installations/hpx_dev_gasnet/install/lib"
    export PKG_CONFIG_PATH="${ROOT}/hpx_installations/hpx_dev_gasnet/gasnet/install/lib/pkgconfig":$PKG_CONFIG_PATH
else
  echo 'Please specify parcelport'
  exit 1
fi
export CMAKE_COMMAND=cmake
# FFTW path
export FFTW_TH_DIR="$ROOT/fftw_installations/fftw_threads_mpi/install/lib"
export PKG_CONFIG_PATH="$FFTW_TH_DIR/pkgconfig":$PKG_CONFIG_PATH

################################################################################
# Compilation
################################################################################
rm -rf $BUILD_DIR && mkdir $BUILD_DIR && cd $BUILD_DIR
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DCMAKE_C_COMPILER=$C_COMPILER
make VERBOSE=1 -j $(grep -c ^processor /proc/cpuinfo)
 
