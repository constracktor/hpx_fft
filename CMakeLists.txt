cmake_minimum_required(VERSION 3.18)
project(hpx_fft CXX)

find_package(HPX REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)

###########################################################
# HPX code 
add_executable(fft_hpx_loop src/fft_hpx_loop.cpp)
target_link_libraries(fft_hpx_loop
                        HPX::hpx
                        PkgConfig::FFTW)
# FFTW MPI+Threads
find_library(
    FFTW_TH_MPI_LIB
    NAMES "fftw3_mpi"
    PATHS $ENV{FFTW_TH_DIR}
)
find_library(
    FFTW_TH_LIB
    NAMES "fftw3_threads"
    PATHS $ENV{FFTW_TH_DIR}
)
add_executable(fftw_mpi_threads src/fftw_mpi_threads.cpp)
target_link_libraries(fftw_mpi_threads
                        PkgConfig::FFTW
                        ${FFTW_TH_MPI_LIB}
                        ${FFTW_TH_LIB})