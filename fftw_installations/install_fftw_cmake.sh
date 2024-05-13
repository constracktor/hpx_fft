#!/usr/bin/env bash
# This script installs FFTW3 from the recommended tarball using cmake
# load compiler and MPI module
module load llvm/17.0.1
# structure
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/fftw_cmake"
export DIR_SRC="$ROOT/src"
export DIR_BUILD="$ROOT/build"
export DIR_INSTALL="$ROOT/install"
# get files
mkdir -p $DIR_SRC
cd $DIR_SRC
export DOWNLOAD_URL="http://www.fftw.org/fftw-3.3.10.tar.gz"
wget -O- ${DOWNLOAD_URL} | tar xz --strip-components=1
# build
mkdir -p $DIR_BUILD
cd $DIR_BUILD
CC=clang CXX=clang++ cmake $DIR_SRC -DCMAKE_INSTALL_PREFIX=$DIR_INSTALL
make -j $(grep -c ^processor /proc/cpuinfo)
# install
make install