# A HPX Communication Benchmark: Distributed FFT using Collectives
 
This repo supplements the Poster Paper submitted to Euro-Par2024

## Dependencies

Execute scripts in installation directory. Adjust modules to your system.

- `./install_hpx_tcp/mpi/lci.sh`
- `./install_fftw_treads_mpi.sh`

## Compile

- `./compile.sh tcp/mpi/lci`

## Run

- `./distributed_benchmark.sh tcp/mpi/lci scatter/all_to_all`

- `./message_benchmark.sh tcp/mpi/lci scatter/all_to_all`

## Contact

- alexander.strack@ipvs.uni-stuttgart.de
- dirk.pflueger@ipvs.uni-stuttgart.de
