# W.I.P 
# #!/usr/bin/env bash
# # This script installs HPX with experimental Gasnet parcelport
# module load llvm/17.0.1 boost/1.83.0 hwloc/2.9.1
# #gasnet
# #export PKG_CONFIG_PATH="/home/alex/hpxsc_installations/hpx_gasnet/gasnet_2023.9.0/install/lib/pkgconfig":$PKG_CONFIG_PATH
# # structure
# export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"
# export DIR_SRC="$ROOT/src"
# export DIR_BUILD="$ROOT/build"
# export DIR_INSTALL="$ROOT/install"
# # install openmpi
# export PKG_CONFIG_PATH="/home/alex/hpxsc_installations/hpx_dev_gasnet/openmpi/install/lib/pkgconfig":$PKG_CONFIG_PATH
# # install gasnet
# #module load gasnet
# export DIR_GASNET="$ROOT/gasnet"
# mkdir -p $DIR_GASNET
# ./install_gasnet.sh $DIR_GASNET
# export PKG_CONFIG_PATH="$DIR_GASNET/install/lib/pkgconfig":$PKG_CONFIG_PATH
# # get files
# mkdir -p $DIR_SRC
# cd $DIR_SRC
# export DOWNLOAD_URL="https://github.com/ct-clmsn/hpx.git"
# git clone ${DOWNLOAD_URL} .
# git checkout gasnet_documentation
# # build
# mkdir -p $DIR_BUILD
# cd $DIR_BUILD
# cmake -DHPX_WITH_MALLOC=tcmalloc -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$DIR_INSTALL -DHPX_WITH_PARCELPORT_TCP=OFF -DHPX_WITH_PARCELPORT_GASNET=ON -DHPX_WITH_PARCELPORT_GASNET_CONDUIT=ucx -DHPX_WITH_FETCH_ASIO=ON -DHPX_WITH_CXX_STANDARD=20 ../src
# make -j $(grep -c ^processor /proc/cpuinfo)
# # install
# make install
