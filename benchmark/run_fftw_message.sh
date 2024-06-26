#!/bin/bash
#SBATCH --job-name=fftw_backend_job     # Job name
#SBATCH --output=fftw_backend_job.log   # Standard output and error log
#SBATCH --mail-type=NONE                # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=alexander.strack@ipvs.uni-stuttgart.de       # Where to send mail	
#SBATCH --time=10:00:00                 # Time limit hrs:min:sec
#SBATCH --exclusive                     # Exclusive ressource access
# optional
#SBATCH --partition=buran               # Name of partition
#SBATCH --nodes=2                      # Number of nodes
#SBATCH --ntasks=2                     # Number of MPI ranks
#SBATCH --cpus-per-task=48              # Number of cores per MPI rank 

# Benchmark script for distributed memory strong scaling
# $1: Executable name
# $2: FFTW planning flag (estimate/measure)
# $3: Largest message size
# $4: Number of threads
# $5: Number of runs
# Parameters
LOOP=$5
POW_START=1
POW_STOP=$3
THREADS=$4
BASE_SIZE=128
# Get run command
COMMAND="srun -N 2 -n 2"
EXECUTABLE="../$1"
ARGUMENTS="$BASE_SIZE $BASE_SIZE $2"
# Log Info
pwd; hostname; date
# Create directories to store data
mkdir result
mkdir plans
# Strong scaling loop from 2^pow_start to 2^pow_stop nodes
$COMMAND $EXECUTABLE $THREADS $ARGUMENTS 1
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND $EXECUTABLE $THREADS $ARGUMENTS 0
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    SIZE=$(($i*$BASE_SIZE))
    ARGUMENTS="$SIZE $SIZE $2"
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND $EXECUTABLE $THREADS $ARGUMENTS 0
    done
done
# Log Info
date