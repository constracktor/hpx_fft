# hpx_fft

export HPXSC_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/../hpxsc_installations/hpx_dist_v1.8.1"
export CMAKE_COMMAND=${HPXSC_ROOT}/build/cmake/bin/cmake
export HPX_DIR=${HPXSC_ROOT}/build/hpx/build/lib

rm -rf build && mkdir build && cd build 

$CMAKE_COMMAND .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${HPX_DIR}/cmake/HPX"

make all