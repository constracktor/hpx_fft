#!/bin/bash
################################################################################
# Diagnostics
################################################################################
set +x

################################################################################
# Preprocessing
################################################################################
# working directory
export HOME="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/.."
# export paths
if [[ "$1" == "epyc2" ]]
then
    # epyc2
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_apex_epyc2_v.1.9.1/build/hpx/build/lib"
    # MPI compiler
    export CXX="${HOME}/hpxsc_installations/hpx_apex_epyc2_v.1.9.1/build/openmpi/bin/mpic++" 
elif [[ "$1" == "buran" ]]
then
    # buran
    module load gcc/11.2.1
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/hpx/build/lib"
    #export HPX_DIR="${HOME}/hpxsc_installations/hpx_1.9.1_lci_gcc_11.2.1/build/hpx/build/lib"
    # MPI compiler
    export CXX="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/openmpi/bin/mpic++" 
    module load openmpi
elif [[ "$1" == "medusa" ]]
then
    # medusa
    module load gcc/11.2.1
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_medusa/build/hpx/build/lib"
    # MPI compiler
    export CXX="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_medusa/build/openmpi/bin/mpic++" 
    module load openmpi
else
  echo 'Please specify system: "epyc2" or "buran" or "medusa"'
  exit 1
fi

export CMAKE_COMMAND=cmake
# FFTW paths
export FFTW_TH_DIR="$HOME/fft_installations/fftw_threads_mpi/install/lib"
export FFTW_OMP_DIR="$HOME/fft_installations/fftw_omp_mpi/install/lib"
export FFTW_HPX_DIR="$HOME/fft_installations/fftw_hpx/install/lib"
export PKG_CONFIG_PATH="$FFTW_TH_DIR/pkgconfig":$PKG_CONFIG_PATH

################################################################################
# Compilation
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX"
make -j $(grep -c ^processor /proc/cpuinfo)