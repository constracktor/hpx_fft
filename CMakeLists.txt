cmake_minimum_required(VERSION 3.18)
project(hpx_fft CXX)

###########################################################
# HPX
find_package(HPX REQUIRED)
find_package(FFTW3 REQUIRED)

# add_executable(fft_hpx src/fft_hpx.cpp ${HEADER_FILES})
# target_link_libraries(fft_hpx
#                         HPX::hpx
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

add_executable(fft_hpx_task src/fft_hpx_task.cpp ${HEADER_FILES})
target_link_libraries(fft_hpx_task
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)
            
add_executable(fft_hpx_task_shared src/fft_hpx_task_shared.cpp ${HEADER_FILES})
target_link_libraries(fft_hpx_task_shared
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)

# add_executable(hello src/hello.cpp ${HEADER_FILES})
# target_link_libraries(hello
#                         HPX::hpx
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

###########################################################
# FFTW
set(ENV{PKG_CONFIG_PATH} "/import/sgs.scratch/strackar/fft_installations/fftw/install/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
find_library(
    FFTW_MPI_LIB
    NAMES "fftw3_mpi"
    PATHS "/import/sgs.scratch/strackar/fft_installations/fftw/install/lib"
)

add_executable(fftw_mpi src/fftw_mpi.cpp ${HEADER_FILES})
target_link_libraries(fftw_mpi
                        PkgConfig::FFTW
                        ${FFTW_MPI_LIB})

###########################################################
# SEQUENTIAL
# add_executable(fftw_2d src/fftw_2d.cpp ${HEADER_FILES})
# target_link_libraries(fftw_2d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)
#                        # PkgConfig::FFTW)
                        
###########################################################
# OBSOLETE

# add_executable(fft_shared src/fft_shared.cpp ${HEADER_FILES})
# target_link_libraries(fft_shared
#                         HPX::hpx 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(scatter src/scatter.cpp ${HEADER_FILES})
# target_link_libraries(scatter
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(fftw_3d src/fftw_3d.cpp ${HEADER_FILES})
# target_link_libraries(fftw_3d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(par_2d src/par_2d.cpp ${HEADER_FILES})
# target_link_libraries(par_2d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(par_3d src/par_3d.cpp ${HEADER_FILES})
# target_link_libraries(par_3d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(all_to_all src/all_to_all.cpp ${HEADER_FILES})
# target_link_libraries(all_to_all
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

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

# add_executable(dist_3d src/dist_3d.cpp ${HEADER_FILES})
# target_link_libraries(dist_3d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         HPX::partitioned_vector_component)
#                         #PkgConfig::FFTW)

#add_executable(test src/test.cpp ${HEADER_FILES})
#target_link_directories(test PUBLIC "$HPX_DIR/../../")
#target_link_libraries(test PUBLIC HPX::hpx HPX::partitioned_vector_component HPX::iostreams_component)
#target_link_libraries(test libhpx_partitioned_vector)
# target_link_libraries(test
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         HPX::component
#                         HPX::plugin)
                        #PkgConfig::FFTW)
#add_hpx_executable(test src/test.cpp)

# add_executable(stencil src/1d_stencil_8.cpp ${HEADER_FILES})
# target_link_libraries(stencil PUBLIC HPX::hpx HPX::partitioned_vector_component HPX::iostreams_component)

# add_executable(all_to_all src/all_to_all.cpp ${HEADER_FILES})
# target_link_libraries(all_to_all PUBLIC HPX::hpx HPX::partitioned_vector_component HPX::iostreams_component)