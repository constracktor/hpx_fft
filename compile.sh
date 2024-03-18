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
    export BUILD_DIR=build_epyc2
    export FFTW_DIR='.'
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_apex_epyc2_v.1.9.1/build/hpx/build/lib"
    # MPI compiler
    export CXX="${HOME}/hpxsc_installations/hpx_apex_epyc2_v.1.9.1/build/openmpi/bin/mpic++"
elif [[ "$1" == "buran_mpi" ]]
then
    # buran
    export BUILD_DIR=build_buran_mpi
    export FFTW_DIR=buran
    module load gcc/11.2.1
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/hpx/build/lib"
    # MPI compiler
    export CXX="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/openmpi/bin/mpic++"
    module load openmpi
elif [[ "$1" == "buran_lci" ]]
then
    # buran
    export BUILD_DIR=build_buran_lci
    export FFTW_DIR=buran
    module load gcc/11.2.1
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_1.9.1_lci_gcc_11.2.1/build/hpx/build/lib"
    # MPI compiler
    export CXX="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/openmpi/bin/mpic++"
    module load openmpi
elif [[ "$1" == "medusa" ]]
then
    # medusa
    export BUILD_DIR=build_medusa
    export FFTW_DIR=buran
    module load gcc/11.2.1
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_medusa/build/hpx/build/lib"
    # MPI compiler
    export CXX="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_medusa/build/openmpi/bin/mpic++"
    module load openmpi
elif [[ "$1" == "buran_gasnet" ]]
then
    # buran with gasnet
    export BUILD_DIR=build_buran_gasnet
    export FFTW_DIR=buran
    module load llvm/17.0.1
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_gasnet/install/lib64"
    export PKG_CONFIG_PATH="${HOME}/hpxsc_installations/hpx_gasnet/gasnet_2023.9.0/install/lib/pkgconfig":$PKG_CONFIG_PATH
    # MPI compiler
    #export CXX="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/openmpi/bin/mpic++" 
    module load openmpi
elif [[ "$1" == "buran_openshmem" ]]
then
    # buran with openshmem
    export BUILD_DIR=build_buran_openshmem
    export FFTW_DIR=buran_clang
    module load llvm/17.0.1
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_openshmem/install/lib64"
    # MPI compiler
    #export CXX="${HOME}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1/build/openmpi/bin/mpic++" 
    module load openmpi
    export C_COMPILER=/opt/apps/llvm/17.0.1/bin/clang
elif [[ "$1" == "sven" ]]
then
    # sven
    export BUILD_DIR=build_sven
    export FFTW_DIR=sven
    # HPX lib directory
    export HPX_DIR="${HOME}/hpxsc_installations/hpx_1.9.1_gcc_13.2.1_sven/build/hpx/lib64"
    # MPI compiler
    module load mpi/openmpi-riscv64
    export CXX=mpic++
    #export PATH=/home/alex/hpxsc_installations/hpx_1.9.1_gcc_13.2.1_sven/build/boost/lib:$PATH
else
  echo 'Please specify system, e.g., "epyc2" or "buran_mpi" or "medusa"'
  exit 1
fi
export CMAKE_COMMAND=cmake
# FFTW paths
export FFTW_TH_DIR="$HOME/fft_installations/$FFTW_DIR/fftw_threads_mpi/install/lib"
export FFTW_OMP_DIR="$HOME/fft_installations/$FFTW_DIR/fftw_omp_mpi/install/lib"
export FFTW_HPX_DIR="$HOME/fft_installations/$FFTW_DIR/fftw_hpx/install/lib"
export PKG_CONFIG_PATH="$FFTW_TH_DIR/pkgconfig":$PKG_CONFIG_PATH

################################################################################
# Compilation
################################################################################
rm -rf $BUILD_DIR && mkdir $BUILD_DIR && cd $BUILD_DIR
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DCMAKE_C_COMPILER=$C_COMPILER
make -j $(grep -c ^processor /proc/cpuinfo)