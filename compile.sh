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
elif [[ "$1" == "burangas" ]]
then
    # buran with gasnet
    module load llvm/17.0.1
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_gasnet/install/lib64"                                      │      |          ^~~~~~~
    export PKG_CONFIG_PATH="${HOME}/hpxsc_installations/hpx_gasnet/gasnet_2023.9.0/install/lib/pkgconfig":$PKG_│compilation terminated.
CONFIG_PATH
    # MPI compiler
    #export CXX="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/openmpi/bin/mpic++" 
    module load openmpi
elif [[ "$1" == "sven" ]]
then
    # sven
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_1.9.1_gcc_13.2.1_sven/build/hpx/lib64"
    # MPI compiler
    module load mpi/openmpi-riscv64
    export CXX=mpic++ 
else
  echo 'Please specify system: "epyc2" or "buran" or "medusa"'
  exit 1
fi
export CMAKE_COMMAND=cmake
# FFTW paths
export FFTW_TH_DIR="$HOME/fft_installations/$1/fftw_threads_mpi/install/lib"
export FFTW_OMP_DIR="$HOME/fft_installations/$1/fftw_omp_mpi/install/lib"
export FFTW_HPX_DIR="$HOME/fft_installations/$1/fftw_hpx/install/lib"
export PKG_CONFIG_PATH="$FFTW_TH_DIR/pkgconfig":$PKG_CONFIG_PATH

################################################################################
# Compilation
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX"
make -j $(grep -c ^processor /proc/cpuinfo)