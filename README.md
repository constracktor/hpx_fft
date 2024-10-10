# A HPX Communication Benchmark: Distributed FFT using Collectives
 
This repo complements the identically titled poster paper submitted to Euro-Par 2024.

## Dependencies

Execute scripts in installation directory. Adjust modules to your system.

- `./install_hpx_{tcp/mpi/lci}.sh`
- `./install_fftw_{seq/threads_mpi}.sh`

## Compile

- `./compile.sh tcp/mpi/lci/fftw`

## Run

- `./distributed_benchmark.sh tcp/mpi/lci/fftw scatter/all_to_all`

- `./message_benchmark.sh tcp/mpi/lci/fftw scatter/all_to_all`

## Contact

- alexander.strack@ipvs.uni-stuttgart.de
- dirk.pflueger@ipvs.uni-stuttgart.de
