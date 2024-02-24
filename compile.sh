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
<<<<<<< HEAD
    export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_apex_epyc2_v.1.9.1"
    export FFTW_SEQ_DIR="${ROOT}/fft_installations/fftw_seq/install/lib/" 
elif [[ "$1" == "buran" ]]
then
    # buran
    export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1"
    #export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_1.9.1_lci_gcc_11.2.1"
    export FFTW_SEQ_DIR="${HPXSC_ROOT}/build/fftw/lib64/"
elif [[ "$1" == "medusa" ]]
then
    # medusa
    export HPXSC_ROOT="${ROOT}/hpxsc_installations/hpx_1.9.1_mpi_gcc_11.2.1_medusa"
    export FFTW_SEQ_DIR="${HPXSC_ROOT}/build/fftw/lib64/"
=======
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
>>>>>>> 2da7608 (add shared sbatch jobscript)
else
  echo 'Please specify system: "epyc2" or "buran" or "medusa"'
  exit 1
fi
export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib
export CXX=${HPXSC_ROOT}/build/openmpi/bin/mpic++ 
# FFTW paths
<<<<<<< HEAD
export FFTW_TH_DIR="$ROOT/fft_installations/fftw_threads_mpi/install/lib"
export FFTW_OMP_DIR="$ROOT/fft_installations/fftw_omp_mpi/install/lib"
export FFTW_HPX_DIR="$ROOT/fft_installations/fftw_hpx/install/lib"
export PKG_CONFIG_PATH="$FFTW_OMP_DIR/pkgconfig":$PKG_CONFIG_PATH
=======
export FFTW_TH_DIR="$HOME/fft_installations/fftw_threads_mpi/install/lib"
export FFTW_OMP_DIR="$HOME/fft_installations/fftw_omp_mpi/install/lib"
export FFTW_HPX_DIR="$HOME/fft_installations/fftw_hpx/install/lib"
export PKG_CONFIG_PATH="$FFTW_TH_DIR/pkgconfig":$PKG_CONFIG_PATH
>>>>>>> 2da7608 (add shared sbatch jobscript)

################################################################################
# Compilation
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DFFTW3_DIR="${FFTW_SEQ_DIR}/cmake/fftw3"
make -j $(grep -c ^processor /proc/cpuinfo)