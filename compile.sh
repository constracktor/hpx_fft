#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Preprocessing
################################################################################
# working directory
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/.."
# export paths
if [[ "$1" == "buran" ]]
then
    # buran
    export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1"
elif [[ "$1" == "medusa" ]]
then
    # medusa
    export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_medusa"
else
  echo 'Please specify system: "buran" or "medusa"'
  exit 1
fi

export FFTW_SEQ_DIR="${HPXSC_ROOT}/build/fftw/lib64/"
export CMAKE_COMMAND=cmake
export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib

################################################################################
# Compilation
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DFFTW3_DIR="${FFTW_SEQ_DIR}/cmake/fftw3"
make -j $(grep -c ^processor /proc/cpuinfo)