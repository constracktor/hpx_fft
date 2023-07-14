# hpx_fft

export HPXSC_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/../hpxsc_installations/hpx_dist_v1.8.1"
export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib

export FFTW_DIR="/import/sgs.scratch/strackar/fft_installations/fftw_seq/install/lib/"  

rm -rf build && mkdir build && cd build 
$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Debug -DHPX_DIR="${HPX_DIR}/cmake/HPX" -DFFTW3_DIR="${FFTW_DIR}/cmake/fftw3"

make all

#export PKG_CONFIG_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/../fft_installations/fftw/install/lib/pkgconfig"