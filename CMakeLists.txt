cmake_minimum_required(VERSION 3.18)
project(hpx_fft CXX)

###########################################################
# HPX
find_package(HPX REQUIRED)
#find_package(FFTW3 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)

add_executable(fft_hpx_loop_shared src/fft_hpx_loop_shared.cpp)
target_link_libraries(fft_hpx_loop_shared
                        HPX::hpx
                        HPX::iostreams_component
                        PkgConfig::FFTW)