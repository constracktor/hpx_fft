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
add_executable(fftw_mpi_threads src/fftw_mpi_threads.cpp)
target_link_libraries(fftw_mpi_threads
                      HPX::hpx
                      PkgConfig::FFTW)
