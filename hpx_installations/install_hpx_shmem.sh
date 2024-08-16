#!/usr/bin/env bash
# This script installs HPX with experimental OpenSHMEM parcelport
module load llvm/18.1.5 boost hwloc openmpi
# structure
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"
export DIR_SRC="$ROOT/src"
export DIR_BUILD="$ROOT/build"
export DIR_INSTALL="$ROOT/install"
# get files
mkdir -p $DIR_SRC
cd $DIR_SRC
export DOWNLOAD_URL="https://github.com/ct-clmsn/hpx.git"
#export DOWNLOAD_URL="https://github.com/constracktor/hpx-shmem.git"
git clone ${DOWNLOAD_URL} .
git checkout openshmem-parcelport
# build
mkdir -p $DIR_BUILD
cd $DIR_BUILD
cmake -DHPX_WITH_MALLOC=tcmalloc \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_INSTALL_PREFIX=$DIR_INSTALL \
      -DHPX_WITH_NETWORKING=ON \
      -DHPX_WITH_PARCELPORT_TCP=OFF \
      -DHPX_WITH_PARCELPORT_MPI=OFF \
      -DHPX_WITH_FETCH_OPENSHMEM=OFF \
      -DHPX_WITH_PARCELPORT_OPENSHMEM=ON \
      -DHPX_WITH_PARCELPORT_OPENSHMEM_CONDUIT=mpi \
      -DHPX_WITH_CXX_STANDARD=20 \
      -DHPX_WITH_FETCH_ASIO=ON ../src
make -j $(grep -c ^processor /proc/cpuinfo)
# install
make install
# Run command: salloc -N 2 -p buran oshrun -n 2 ./hello_world_distributed |& less
