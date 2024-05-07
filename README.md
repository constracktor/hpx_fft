# HPX based FFT with FFTW3 as 1D backend and FFTW3 with HPX backend

This repo is currently under active developement.

## Dependencies

Execute scripts in FFTW and HPX installations. Adjust modules to your system

## Compile

- `./compile.sh tcp/mpi/lci/shmem/gasnet`

## Run

- `./distributed_benchmark.sh tcp/mpi/lci/shmem/gasnet scatter/all_to_all`

- `./message_benchmark.sh tcp/mpi/lci/shmem/gasnet scatter/all_to_all`


## Contact

alexander.strack@ipvs.uni-stuttgart.de
