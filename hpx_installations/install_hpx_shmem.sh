#!/usr/bin/env bash
# This script installs HPX with experimental OpenSHMEM parcelport

# MODIFY: Adjust required modules
module load gcc/12.2.1 boost hwloc/2.9.1 openmpi
# structure
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/hpx_dev_shmem"
export DIR_SRC="$ROOT/src"
export DIR_BUILD="$ROOT/build"
export DIR_INSTALL="$ROOT/install"
# get files
mkdir -p $DIR_SRC
cd $DIR_SRC
export DOWNLOAD_URL="https://github.com/ct-clmsn/hpx.git"
git clone ${DOWNLOAD_URL} .
git checkout openshmem-parcelport
# build
mkdir -p $DIR_BUILD
cd $DIR_BUILD
# MODIFY: Adjust OpenSHMEM conduit
cmake -DCMAKE_CXX_COMPILER=oshc++ -DHPX_WITH_PARCELPORT_OPENSHMEM_CONDUIT=mpi -DCMAKE_C_FLAGS='-DOSHMEM_HAVE_C11' -DHPX_WITH_MALLOC=tcmalloc -DHPX_WITH_PARCELPORT_TCP=OFF -DHPX_WITH_PARCELPORT_OPENSHMEM=ON  -DHPX_WITH_FETCH_ASIO=ON -DHPX_WITH_EXAMPLES=ON -DHPX_WITH_TESTS=OFF -DHPX_WITH_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX=$DIR_INSTALL -DHPX_HAVE_NETWORKING=ON ../src
make -j $(grep -c ^processor /proc/cpuinfo)
# install
make install
