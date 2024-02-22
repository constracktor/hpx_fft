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
    export HPX_DIR=""
elif [[ "$1" == "medusa" ]]
then
    # medusa
    export HPX_DIR="/work/karame.mp/hpx_nov/hpx/build/lib"
else
  echo 'Please specify system: "buran" or "medusa"'
  exit 1
fi

export FFTW_SEQ_DIR="/work/karame.mp/fftw/fftw_seq/install/lib"
export CMAKE_COMMAND=cmake
export PKG_CONFIG_PATH="$FFTW_SEQ_DIR/pkgconfig":$PKG_CONFIG_PATH
################################################################################
# Compilation
################################################################################
rm -rf build && mkdir build && cd build
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DFFTW3_DIR="${FFTW_SEQ_DIR}/cmake/fftw3"
make -j $(grep -c ^processor /proc/cpuinfo)