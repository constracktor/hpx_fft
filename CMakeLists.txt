cmake_minimum_required(VERSION 3.18)
project(hpx_fft CXX)
find_package(HPX REQUIRED)

#set(ENV{PKG_CONFIG_PATH} "/import/sgs.scratch/strackar/fft_installations/fftw/lib")
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
#include_directories(PkgConfig::FFTW)
#link_libraries     (PkgConfig::FFTW)



# add_executable(sender_receiver src/sender_receiver.cpp ${HEADER_FILES})
# target_link_libraries(sender_receiver 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component)

# add_executable(test_fftw src/test_fftw.cpp ${HEADER_FILES})
# target_link_libraries(test_fftw 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         PkgConfig::FFTW)

# add_executable(fftw_2d src/fftw_2d.cpp ${HEADER_FILES})
# target_link_libraries(fftw_2d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         PkgConfig::FFTW)
                        
# add_executable(fftw_3d src/fftw_3d.cpp ${HEADER_FILES})
# target_link_libraries(fftw_3d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         PkgConfig::FFTW)

add_executable(par_3d src/par_3d.cpp ${HEADER_FILES})
target_link_libraries(par_3d 
                        HPX::hpx 
                        HPX::wrap_main 
                        HPX::iostreams_component
                        PkgConfig::FFTW)

add_executable(dist_3d src/dist_3d.cpp ${HEADER_FILES})
target_link_libraries(dist_3d 
                        HPX::hpx 
                        HPX::wrap_main 
                        HPX::iostreams_component)
                        #PkgConfig::FFTW)

