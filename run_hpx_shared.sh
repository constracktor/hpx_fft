#!/bin/bash
#SBATCH --job-name=hpx_fft_job          # Job name
#SBATCH --output=hpx_fft_job.log        # Standard output and error log
#SBATCH --mail-type=END                 # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=alexander.strack@ipvs.uni-stuttgart.de       # Where to send mail	
#SBATCH --time=01:00:00                 # Time limit hrs:min:sec
#SBATCH --exclusive                     # Exclusive ressource access
#SBATCH --partition=buran               # Name of partition
#SBATCH --nodes=1                       # Number of nodes
#SBATCH --ntasks=1                      # Number of MPI ranks
#SBATCH --cpus-per-task=32              # Number of cores per MPI rank 

################################################################################
# Benchmark script for shared-memory behind slurm
# Parameters
LOOP=1
BASE_SIZE=16384
FFTW_PLAN=estimate
#FFTW_PLAN=measure
# Compute benchmark script from 2^start to 2^stop
POW_START=1
POW_STOP=5
# get run command
COMMAND="srun -N 1 -n 1 -c 1"
EXECUTABLE="./build/$1"
ARGUMENTS="--nx=$BASE_SIZE --ny=$BASE_SIZE --run=par --plan=$FFTW_PLAN"

pwd; hostname; date
 
########
# strong scaling
mkdir result
mkdir plans
# load modules
###############################
# HPX loop
# shared
$COMMAND $EXECUTABLE $ARGUMENTS --header=true 
for (( j=1; j<$LOOP; j=j+1 ))
do
    $COMMAND $EXECUTABLE $ARGUMENTS
done
for (( i=2**$POW_START; i<=2**$POW_STOP; i=i*2 ))
do
    COMMAND="srun -N 1 -n 1 -c $i"
    for (( j=0; j<$LOOP; j=j+1 ))
    do
        $COMMAND $EXECUTABLE $ARGUMENTS
    done
done
date



