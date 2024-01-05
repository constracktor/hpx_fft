#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Variables
################################################################################
#export APEX_SCREEN_OUTPUT=1 APEX_EXPORT_CSV=1 APEX_TASKGRAPH_OUTPUT=1 APEX_TASKTREE_OUTPUT=1
# current directory
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/.."
# determine paths
if [[ "$1" == "epyc2" ]]
then
    # epyc2
    export FFTW_DIR="${ROOT}/fft_installations/fftw_seq/install/lib/" 
    export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_apex_epyc2_v.1.9.1"
    export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib
    export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
    # for fftw mpi
    export CXX=${HPXSC_ROOT}/build/openmpi/bin/mpic++ 
elif [[ "$1" == "buran" ]]
then
    # buran
    export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1"
    export FFTW_DIR="${HPXSC_ROOT}/build/fftw/lib64/"
    export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
    export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib
    #export HPX_DIR=${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_collectives/install/lib64
    module load gcc/11.2.1
    # for fftw mpi
    module load openmpi
elif [[ "$1" == "medusa" ]]
then
    # medusa
    export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_medusa"
    export FFTW_DIR="${HPXSC_ROOT}/build/fftw/lib64/"
    export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
    export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib
    #export HPX_DIR=${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_collectives/install/lib64
    module load gcc/11.2.1
    # for fftw mpi
    module load openmpi
else
  echo 'Please specify system to compile: "epyc2" or "buran"'
  exit 1
fi
# for fftw mpi
export CXX=${HPXSC_ROOT}/build/openmpi/bin/mpic++ 
# fftw libs
export FFTW_TH_DIR="$ROOT/fft_installations/fftw_threads_mpi/install/lib"
export FFTW_OMP_DIR="$ROOT/fft_installations/fftw_omp_mpi/install/lib"
export FFTW_HPX_DIR="$ROOT/fft_installations/fftw_hpx/install/lib"
export PKG_CONFIG_PATH="$FFTW_OMP_DIR/pkgconfig":$PKG_CONFIG_PATH

################################################################################
# Compile code
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DFFTW3_DIR="${FFTW_DIR}/cmake/fftw3"
make -j $(grep -c ^processor /proc/cpuinfo)