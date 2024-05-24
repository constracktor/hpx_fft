# HPX based FFT with FFTW3 as 1D backend and FFTW3 with HPX backend

This repo is currently under active developement.

## Dependencies

Execute scripts in FFTW and HPX installation directories. Adjust modules to your system.

- `./install_hpx_tcp/mpi/lci.sh`
- `./install_fftw_{sequential}/{treads_mpi}.sh`

## Compile

- `./compile.sh tcp/mpi/lci`

## Run

- `./distributed_benchmark.sh tcp/mpi/lci scatter/all_to_all`

- `./message_benchmark.sh tcp/mpi/lci scatter/all_to_all`

## Contact

alexander.strack@ipvs.uni-stuttgart.de
