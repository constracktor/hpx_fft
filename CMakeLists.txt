cmake_minimum_required(VERSION 3.18)
project(hpx_fft CXX)

###########################################################
# HPX
find_package(HPX REQUIRED)
find_package(FFTW3 REQUIRED)

add_executable(fft_hpx_task_agas src/fft_hpx_task_agas.cpp)
target_link_libraries(fft_hpx_task_agas
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)

add_executable(fft_hpx_loop src/fft_hpx_loop.cpp)
target_link_libraries(fft_hpx_loop
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)

add_executable(fft_hpx_task_agas_shared src/fft_hpx_task_agas_shared.cpp)
target_link_libraries(fft_hpx_task_agas_shared
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)

add_executable(fft_hpx_task_sync_shared src/fft_hpx_task_sync_shared.cpp)
target_link_libraries(fft_hpx_task_sync_shared
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)

add_executable(fft_hpx_task_naive_shared src/fft_hpx_task_naive_shared.cpp)
target_link_libraries(fft_hpx_task_naive_shared
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)

add_executable(fft_hpx_task_shared src/fft_hpx_task_shared.cpp)
target_link_libraries(fft_hpx_task_shared
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)

add_executable(fft_hpx_loop_shared src/fft_hpx_loop_shared.cpp)
target_link_libraries(fft_hpx_loop_shared
                        HPX::hpx
                        HPX::iostreams_component
                        FFTW3::fftw3)

###########################################################
# FFTW
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
find_package(OpenMP REQUIRED)

# search for MPI+Threads
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

# search for MPI+OpenMP
find_library(
    FFTW_OMP_MPI_LIB
    NAMES "fftw3_mpi"
    PATHS $ENV{FFTW_OMP_DIR}
)
find_library(
    FFTW_OMP_LIB
    NAMES "fftw3_omp"
    PATHS $ENV{FFTW_OMP_DIR}
)

# search for HPX (MPI wip.)
find_library(
    FFTW_HPX_MPI_LIB
    NAMES "fftw3_mpi"
    PATHS $ENV{FFTW_HPX_DIR}
)
find_library(
    FFTW_HPX_LIB
    NAMES "fftw3_hpx"
    PATHS $ENV{FFTW_HPX_DIR}
)

# 
add_executable(fftw_mpi_threads src/fftw_mpi_threads.cpp)
target_link_libraries(fftw_mpi_threads
                        PkgConfig::FFTW
                        ${FFTW_TH_MPI_LIB}
                        ${FFTW_TH_LIB})
                        
add_executable(fftw_mpi_omp src/fftw_mpi_omp.cpp)
target_link_libraries(fftw_mpi_omp
                        PkgConfig::FFTW
                        ${FFTW_OMP_MPI_LIB}
                        ${FFTW_OMP_LIB}
                        OpenMP::OpenMP_CXX)

add_executable(fftw_hpx src/fftw_hpx.cpp)
target_link_libraries(fftw_hpx
                        PkgConfig::FFTW
                        ${FFTW_HPX_LIB}
                        HPX::hpx)

add_executable(fftw_mpi_hpx src/fftw_mpi_hpx.cpp)
target_link_libraries(fftw_mpi_hpx
                        PkgConfig::FFTW
                        ${FFTW_HPX_MPI_LIB}
                        ${FFTW_HPX_LIB}
                        HPX::hpx
                        HPX::wrap_main)

###########################################################
# SEQUENTIAL
# add_executable(fftw_2d src/fftw_2d.cpp)
# target_link_libraries(fftw_2d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)
#                        # PkgConfig::FFTW)
                        
###########################################################
# OBSOLETE

# add_executable(fftw_mpi src/fftw_mpi.cpp)
# target_link_libraries(fftw_mpi
#                         PkgConfig::FFTW
#                         ${FFTW_MPI_LIB})
# add_executable(fftw_omp src/fftw_omp.cpp)
# target_link_libraries(fftw_omp
#                         PkgConfig::FFTW
#                         ${FFTW_OMP_LIB}
#                         OpenMP::OpenMP_CXX)
                        
# add_executable(fft_hpx src/fft_hpx.cpp)
# target_link_libraries(fft_hpx
#                         HPX::hpx
#                         HPX::iostreams_component
#                         FFTW3::fftw3)
# add_executable(fft_shared src/fft_shared.cpp)
# target_link_libraries(fft_shared
#                         HPX::hpx 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(scatter src/scatter.cpp)
# target_link_libraries(scatter
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(fftw_3d src/fftw_3d.cpp)
# target_link_libraries(fftw_3d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(par_2d src/par_2d.cpp)
# target_link_libraries(par_2d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(par_3d src/par_3d.cpp)
# target_link_libraries(par_3d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(all_to_all src/all_to_all.cpp)
# target_link_libraries(all_to_all
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         FFTW3::fftw3)

# add_executable(sender_receiver src/sender_receiver.cpp)
# target_link_libraries(sender_receiver 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component)

# add_executable(test_fftw src/test_fftw.cpp)
# target_link_libraries(test_fftw 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         PkgConfig::FFTW)

# add_executable(dist_3d src/dist_3d.cpp)
# target_link_libraries(dist_3d 
#                         HPX::hpx 
#                         HPX::wrap_main 
#                         HPX::iostreams_component
#                         HPX::partitioned_vector_component)
#                         #PkgConfig::FFTW)

#add_executable(test src/test.cpp)
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

# add_executable(stencil src/1d_stencil_8.cpp)
# target_link_libraries(stencil PUBLIC HPX::hpx HPX::partitioned_vector_component HPX::iostreams_component)

# add_executable(all_to_all src/all_to_all.cpp)
# target_link_libraries(all_to_all PUBLIC HPX::hpx HPX::partitioned_vector_component HPX::iostreams_component)