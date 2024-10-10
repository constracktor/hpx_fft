#!/usr/bin/env bash
# This script installs FFTW3 from the recommended tarball

## MODIFY: Adjust required modules
module load llvm/17.0.6 openmpi/5.0.3
# structure
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/fftw_seq"
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
CC=clang CXX=clang++ $DIR_SRC/configure --prefix=$DIR_INSTALL --disable-fortran --enable-shared
make -j $(grep -c ^processor /proc/cpuinfo)
# install
make install
