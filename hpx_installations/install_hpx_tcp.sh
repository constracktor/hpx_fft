#!/usr/bin/env bash
# This script installs HPX with TCP parcelport

# MODIFY: Adjust required modules
module load llvm/17.0.1 boost/1.83.0 hwloc/2.9.1
# structure
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/hpx_1.9_tcp"
export DIR_SRC="$ROOT/src"
export DIR_BUILD="$ROOT/build"
export DIR_INSTALL="$ROOT/install"
# get files
mkdir -p $DIR_SRC
cd $DIR_SRC
export DOWNLOAD_URL="https://github.com/STEllAR-GROUP/hpx"
git clone ${DOWNLOAD_URL} .
git checkout release-1.9.X
# build
mkdir -p $DIR_BUILD
cd $DIR_BUILD
# MODIFY: Adjust CMAKE_CXX_COMPILER
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$DIR_INSTALL -DHPX_WITH_MALLOC=tcmalloc -DHPX_WITH_PARCELPORT_TCP=ON -DHPX_WITH_PARCELPORT_MPI=OFF -DHPX_WITH_NETWORKING=ON -DHPX_WITH_CXX_STANDARD=20 -DHPX_WITH_FETCH_ASIO=ON -DHPX_WITH_EXAMPLES=OFF -DHPX_WITH_TESTS=OFF ../src
make -j $(grep -c ^processor /proc/cpuinfo)
# install
make install
