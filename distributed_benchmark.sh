#!/bin/bash
################################################################################
# Benchmark script for distributed memory 
# $1: FFTW planning flag (estimate/measure)
# load modules
module load openmpi
cd benchmark
# HPX implementations
sbatch run_hpx_dist.sh fft_hpx_loop $1
sbatch run_hpx_dist.sh fft_hpx_task_agas $1
# # FFTW backends
# sbatch run_fftw_dist.sh fftw_mpi_threads $1
# sbatch run_fftw_dist.sh fftw_mpi_omp $1





# #!/bin/bash
# ################################################################################
# # Run benchmark script for distributed slurm
# ################################################################################
# # load modules
# module load openmpi
# # strong scaling
# mkdir result
# mkdir plans
# LOOP=50
# BASE_SIZE=16384
# FFTW_PLAN=measure
# # Compute benchmark script from 2^start to 2^stop
# POW_START=1
# # get partition
# if [[ "$1" == "buran" ]]
# then
#     PARTITION=buran
#     THREADS=48
#     POW_STOP=4
# elif [[ "$1" == "medusa" ]]
# then
#     PARTITION=medusa
#     THREADS=40
#     POW_STOP=3
# else
#   echo 'Please specify benchmark: "buran" or "medusa"'
#   exit 1
# fi
# ###############################
# # HPX loop
# # scatter loop
# srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_loop --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true --plan=$FFTW_PLAN
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         srun -p $PARTITION -N $i -n $i -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_loop --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
#     done
# done
# ###############################
# # HPX future
# # scatter task agas
# srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --header=true --plan=$FFTW_PLAN
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         srun -p $PARTITION -N $i -n $i -c $THREADS -t 1:00:00 --exclusive ./build/fft_hpx_task_agas --hpx:threads=$THREADS --nx=$BASE_SIZE  --ny=$BASE_SIZE  --run=scatter --plan=$FFTW_PLAN 
#     done
# done
# ###############################
# # FFTW
# # MPI + OpenMP
# srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         srun -p $PARTITION -N $i -n $i -c $THREADS -t 1:00:00 --exclusive ./build/fftw_mpi_omp $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0 
#     done
# done
# # MPI + Threads
# srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  1
# for (( j=1; j<$LOOP; j=j+1 ))
# do
#     srun -p $PARTITION -N 1 -n 1 -c $THREADS -t 1:00:00 --exclusive ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0
# done
# for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
# do
#     for (( j=0; j<$LOOP; j=j+1 ))
#     do
#         srun -p $PARTITION -N $i -n $i -c $THREADS -t 1:00:00 --exclusive ./build/fftw_mpi_threads $THREADS $BASE_SIZE $BASE_SIZE $FFTW_PLAN  0 
#     done
# done