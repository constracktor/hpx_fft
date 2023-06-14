cmake_minimum_required(VERSION 3.18)
project(hpx_fft CXX)
find_package(HPX REQUIRED)

add_executable(sender_receiver src/sender_receiver.cpp ${HEADER_FILES})
target_link_libraries(sender_receiver 
                        HPX::hpx 
                        HPX::wrap_main 
                        HPX::iostreams_component)

