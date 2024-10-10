#!/usr/bin/env bash
# This script installs FFTW3 from the recommended tarball with MPI+pthreads backend

## MODIFY: Adjust required modules
module load llvm/17.0.6 openmpi/5.0.3
# structure
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/fftw_threads_mpi"
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
CC=clang CXX=clang++ $DIR_SRC/configure --prefix=$DIR_INSTALL --enable-mpi --enable-threads --disable-fortran --enable-shared
make -j $(grep -c ^processor /proc/cpuinfo)
# install
make install
# update pkgconfig to link to fftw3_threads and fftw3_mpi
sed -i 's/Libs: -L${libdir} -lfftw3/Libs: -L${libdir} -lfftw3 -lfftw3_mpi -lfftw3_threads/g' $DIR_INSTALL/lib/pkgconfig/fftw3.pc
