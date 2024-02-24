#!/bin/bash
#SBATCH --job-name=fftw_backend_job     # Job name
#SBATCH --output=fftw_backend_job.log   # Standard output and error log
#SBATCH --mail-type=NONE                # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=alexander.strack@ipvs.uni-stuttgart.de       # Where to send mail	
#SBATCH --time=01:00:00                 # Time limit hrs:min:sec
#SBATCH --exclusive                     # Exclusive ressource access
#SBATCH --partition=buran               # Name of partition
#SBATCH --nodes=1                       # Number of nodes
#SBATCH --ntasks=1                      # Number of MPI ranks
#SBATCH --cpus-per-task=32              # Number of cores per MPI rank 

# Benchmark script for shared-memory strong scaling
# $1: Executable name
# $2: FFTW planning flag (estimate/measure)
# Parameters
LOOP=1
POW_START=1
POW_STOP=5
BASE_SIZE=16384
# Get run command
COMMAND="srun -N 1 -n 1 -c 1"
EXECUTABLE="../build/$1"
ARGUMENTS="$BASE_SIZE $BASE_SIZE $2"
# Log Info
pwd; hostname; date
# Create directories to store data
mkdir result
mkdir plans
# Strong scaling loop from 2^pow_start to 2^pow_stop
FFTW3_HPX_NTHREADS=1 $COMMAND $EXECUTABLE 1 $ARGUMENTS 1 
for (( j=1; j<$LOOP; j=j+1 ))
do
    FFTW3_HPX_NTHREADS=1 $COMMAND $EXECUTABLE 1 $ARGUMENTS 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    COMMAND="srun -N 1 -n 1 -c $i"
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        FFTW3_HPX_NTHREADS=$i $COMMAND $EXECUTABLE $i $ARGUMENTS 0
    done
done
# Log Info
date