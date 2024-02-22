cmake_minimum_required(VERSION 3.18)
project(hpx_fft CXX)

###########################################################
# HPX
find_package(HPX REQUIRED)
find_package(FFTW3 REQUIRED)

add_executable(fft_hpx_loop_shared src/fft_hpx_loop_shared.cpp)
target_link_libraries(fft_hpx_loop_shared
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)