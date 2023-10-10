import numpy as np
import os
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 14})
matplotlib.pyplot.title(r'ABC123 vs $\mathrm{ABC123}^{123}$')
# CVD accessible colors
# black - dark red - indigo (blueish) - yellow - teal (greenish) - light gray
colors = ['#000000','#c1272d', '#0000a7', '#eecc16', '#008176', '#b3b3b3']
# black - dark grey - grey - light grey - very light grey
greyscale = ['#000000', '#333333', '#666666', '#999999', '#cccccc']

# get header
n_loop = 5

################################################################################
################################################################################
# STRONG SCALING
# strong scaling data for hpx loop shared
ss_loop_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/strong_runtimes_hpx_loop_shared.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_loop_shared_matrix.shape[0]/n_loop)
ss_loop_shared_averaged = np.zeros((n_entries, ss_loop_shared_matrix.shape[1]))
for i in range (n_entries):
    ss_loop_shared_averaged[i,:] = np.mean(ss_loop_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for hpx loop scatter
ss_loop_scatter_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/strong_runtimes_hpx_loop_scatter.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_loop_scatter_matrix.shape[0]/n_loop)
ss_loop_scatter_averaged = np.zeros((n_entries, ss_loop_scatter_matrix.shape[1]))
for i in range (n_entries):
    ss_loop_scatter_averaged[i,:] = np.mean(ss_loop_scatter_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for hpx loop all_to_all
ss_loop_all_to_all_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/strong_runtimes_hpx_loop_all_to_all.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_loop_all_to_all_matrix.shape[0]/n_loop)
ss_loop_all_to_all_averaged = np.zeros((n_entries, ss_loop_all_to_all_matrix.shape[1]))
for i in range (n_entries):
    ss_loop_all_to_all_averaged[i,:] = np.mean(ss_loop_all_to_all_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for fftw_omp
ss_fftw_omp_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/strong_runtimes_fftw_omp.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_omp_matrix.shape[0]/n_loop)
ss_fftw_omp_averaged = np.zeros((n_entries, ss_fftw_omp_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_omp_averaged[i,:] = np.mean(ss_fftw_omp_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for fftw_mpi
ss_fftw_mpi_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/strong_runtimes_fftw_mpi.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_mpi_matrix.shape[0]/n_loop)
ss_fftw_mpi_averaged = np.zeros((n_entries, ss_fftw_mpi_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_mpi_averaged[i,:] = np.mean(ss_fftw_mpi_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
################################################################################
# STRONG SCALING PARALLEL EFFICIENCY
plt.figure(figsize=(10,3))
# line
points = ss_loop_shared_averaged[:,0]
plt.plot(points, 100 *np.ones(points.size), 'k:', linewidth=1)
# HPX loop shared data
parallel_efficiency = 100 * ss_loop_shared_averaged[0,7] / (ss_loop_shared_averaged[:,7] * ss_loop_shared_averaged[:,0])
plt.plot(points, parallel_efficiency, 'o-', c=colors[0], linewidth=1, label='HPX loop shared')
# HPX loop scatter data
parallel_efficiency = 100 * ss_loop_scatter_averaged[0,7] / (ss_loop_scatter_averaged[:,7] * ss_loop_scatter_averaged[:,0])
plt.plot(points, parallel_efficiency, 'o-', c=colors[1], linewidth=1, label='HPX loop scatter')
# HPX loop all_to_all data
parallel_efficiency = 100 * ss_loop_all_to_all_averaged[0,7] / (ss_loop_all_to_all_averaged[:,7] * ss_loop_all_to_all_averaged[:,0])
plt.plot(points, parallel_efficiency, 'o-', c=colors[3], linewidth=1, label='HPX loop all_to_all')
# OpenMP data
parallel_efficiency = 100 * ss_fftw_omp_averaged[0,6] / (ss_fftw_omp_averaged[:,6] * ss_fftw_omp_averaged[:,1])
plt.plot(points, parallel_efficiency, 'o-', c=colors[4], linewidth=1, label='OpenMP')
# MPI data
parallel_efficiency = 100 * ss_fftw_mpi_averaged[0,6] / (ss_fftw_mpi_averaged[:,6] * ss_fftw_mpi_averaged[:,0])
plt.plot(points[:-1], parallel_efficiency, 'o-', c=colors[5], linewidth=1, label='MPI')
# plot parameters
plt.title('Strong Scaling efficiency for shared-memory ipvs-epyc2')
plt.legend(bbox_to_anchor=(1.04, 1), loc="upper left")
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.ylabel('Parallel efficiency in %')
ticks_y = np.linspace(0, 100, num=11, endpoint=True, dtype=int)
plt.yticks(ticks=ticks_y)
plt.savefig('plot/figures/strong_scaling_efficiency.pdf', bbox_inches='tight')
################################################################################
# strong SCALING RUNTIME
plt.figure(figsize=(10,3))
# HPX loop shared data
plt.plot(points, ss_loop_shared_averaged[:,7], 'o-', c=colors[0], linewidth=1, label='HPX loop shared')
# HPX loop scatter data
plt.plot(points, ss_loop_scatter_averaged[:,7], 'o-', c=colors[1], linewidth=1, label='HPX loop scatter')
# HPX loop all_to_all data
plt.plot(points, ss_loop_all_to_all_averaged[:,7], 'o-', c=colors[3], linewidth=1, label='HPX loop all_to_all')
# OpenMP data
plt.plot(points[:], ss_fftw_omp_averaged[:,6], 'o-', c=colors[4], linewidth=1, label='OpenMP')
# MPI data
plt.plot(points[:-1], ss_fftw_mpi_averaged[:,6], 'o-', c=colors[5], linewidth=1, label='MPI')
# plot parameters
plt.title('Strong Scaling runtime for shared-memory ipvs-epyc2')
plt.legend(bbox_to_anchor=(1.04, 1), loc="upper left")
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_runtime.pdf', bbox_inches='tight')
################################################################################
# CORES PARALLEL SPEEDUP
# plot PETSc and HPX data scaling parallel efficiency
plt.figure(figsize=(10,5))
# line
plt.plot(points,points, 'k:', linewidth=1)
# HPX loop shared data
parallel_speedup = ss_loop_shared_averaged[0,7] / (ss_loop_shared_averaged[:,7])
plt.plot(points, parallel_speedup, 'o-', c=colors[0], linewidth=1, label='HPX loop shared')
# HPX loop scatter data
parallel_speedup = ss_loop_scatter_averaged[0,7] / (ss_loop_scatter_averaged[:,7])
plt.plot(points, parallel_speedup, 'o-', c=colors[1], linewidth=1, label='HPX loop scatter')
# HPX loop all_to_all data
parallel_speedup = ss_loop_all_to_all_averaged[0,7] / (ss_loop_all_to_all_averaged[:,7])
plt.plot(points, parallel_speedup, 'o-', c=colors[3], linewidth=1, label='HPX loop all_to_all')
# OpenMP data
parallel_speedup = ss_fftw_omp_averaged[0,6] / (ss_fftw_omp_averaged[:,6])
plt.plot(points, parallel_speedup, 'o-', c=colors[4], linewidth=1, label='OpenMP')
# MPI data
parallel_speedup = ss_fftw_mpi_averaged[0,6] / (ss_fftw_mpi_averaged[:,6])
plt.plot(points[:-1], parallel_speedup, 'o-', c=colors[5], linewidth=1, label='MPI')
# plot parameters
plt.title('Parallel speedup for shared-memory ipvs-epyc2')
plt.legend(loc='upper left')
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.ylabel('Parallel speedup')
plt.yscale("log")
ticks_y = np.logspace(0, 2, num=3, endpoint=True, base=10.0, dtype=int)
plt.yticks(ticks=ticks_y, labels=ticks_y)
plt.savefig('plot/figures/strong_scaling_speedup.pdf', bbox_inches='tight')
################################################################################
################################################################################                                                











################################################################################
################################################################################
# WEAK SCALING
# weak scaling data for hpx loop shared
ws_loop_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/weak_runtimes_hpx_loop_shared.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_loop_shared_matrix.shape[0]/n_loop)
ws_loop_shared_averaged = np.zeros((n_entries, ws_loop_shared_matrix.shape[1]))
for i in range (n_entries):
    ws_loop_shared_averaged[i,:] = np.mean(ws_loop_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# weak scaling data for hpx loop scatter
ws_loop_scatter_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/weak_runtimes_hpx_loop_scatter.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_loop_scatter_matrix.shape[0]/n_loop)
ws_loop_scatter_averaged = np.zeros((n_entries, ws_loop_scatter_matrix.shape[1]))
for i in range (n_entries):
    ws_loop_scatter_averaged[i,:] = np.mean(ws_loop_scatter_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# weak scaling data for hpx loop all_to_all
ws_loop_all_to_all_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/weak_runtimes_hpx_loop_all_to_all.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_loop_all_to_all_matrix.shape[0]/n_loop)
ws_loop_all_to_all_averaged = np.zeros((n_entries, ws_loop_all_to_all_matrix.shape[1]))
for i in range (n_entries):
    ws_loop_all_to_all_averaged[i,:] = np.mean(ws_loop_all_to_all_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# weak scaling data for fftw_omp
ws_fftw_omp_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/weak_runtimes_fftw_omp.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_fftw_omp_matrix.shape[0]/n_loop)
ws_fftw_omp_averaged = np.zeros((n_entries, ws_fftw_omp_matrix.shape[1]))
for i in range (n_entries):
    ws_fftw_omp_averaged[i,:] = np.mean(ws_fftw_omp_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# weak scaling data for fftw_mpi
ws_fftw_mpi_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/weak_runtimes_fftw_mpi.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_fftw_mpi_matrix.shape[0]/n_loop)
ws_fftw_mpi_averaged = np.zeros((n_entries, ws_fftw_mpi_matrix.shape[1]))
for i in range (n_entries):
    ws_fftw_mpi_averaged[i,:] = np.mean(ws_fftw_mpi_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
################################################################################
# WEAK SCALING PARALLEL EFFICIENCY
plt.figure(figsize=(10,3))
# line
points = ws_loop_shared_averaged[:,0]
plt.plot(points, 100 *np.ones(points.size), 'k:', linewidth=1)
# HPX loop shared data
parallel_efficiency = 100 * ws_loop_shared_averaged[0,7] / (ws_loop_shared_averaged[:,7])
plt.plot(points, parallel_efficiency, 'o-', c=colors[0], linewidth=1, label='HPX loop shared')
# HPX loop scatter data
parallel_efficiency = 100 * ws_loop_scatter_averaged[0,7] / (ws_loop_scatter_averaged[:,7])
plt.plot(points, parallel_efficiency, 'o-', c=colors[1], linewidth=1, label='HPX loop scatter')
# HPX loop all_to_all data
parallel_efficiency = 100 * ws_loop_all_to_all_averaged[0,7] / (ws_loop_all_to_all_averaged[:,7])
plt.plot(points, parallel_efficiency, 'o-', c=colors[3], linewidth=1, label='HPX loop all_to_all')
# OpenMP data
parallel_efficiency = 100 * ws_fftw_omp_averaged[0,6] / (ws_fftw_omp_averaged[:,6])
plt.plot(points, parallel_efficiency, 'o-', c=colors[4], linewidth=1, label='OpenMP')
# MPI data
parallel_efficiency = 100 * ws_fftw_mpi_averaged[0,6] / (ws_fftw_mpi_averaged[:,6])
plt.plot(points[:-1], parallel_efficiency, 'o-', c=colors[5], linewidth=1, label='MPI')
# plot parameters
plt.title('Approximated Weak Scaling efficiency for shared-memory ipvs-epyc2')
plt.legend(loc='upper right')
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.ylabel('Parallel efficiency in %')
ticks_y = np.linspace(0, 100, num=11, endpoint=True, dtype=int)
plt.yticks(ticks=ticks_y)
plt.savefig('plot/figures/weak scaling_efficiency.pdf', bbox_inches='tight')
################################################################################
# WEAK SCALING RUNTIME
plt.figure(figsize=(10,3))
# HPX loop shared data
plt.plot(points, ws_loop_shared_averaged[:,7], 'o-', c=colors[0], linewidth=1, label='HPX loop shared')
# HPX loop scatter data
plt.plot(points, ws_loop_scatter_averaged[:,7], 'o-', c=colors[1], linewidth=1, label='HPX loop scatter')
# HPX loop all_to_all data
plt.plot(points, ws_loop_all_to_all_averaged[:,7], 'o-', c=colors[3], linewidth=1, label='HPX loop all_to_all')
# OpenMP data
plt.plot(points[:], ws_fftw_omp_averaged[:,6], 'o-', c=colors[4], linewidth=1, label='OpenMP')
# MPI data
plt.plot(points[:-1], ws_fftw_mpi_averaged[:,6], 'o-', c=colors[5], linewidth=1, label='MPI')
# plot parameters
plt.title('Approximated Weak Scaling runtime for shared-memory ipvs-epyc2')
plt.legend(loc='upper left')
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/weak scaling_runtime.pdf', bbox_inches='tight')
################################################################################
################################################################################



# # CORES PARALLEL SPEEDUP
# # plot PETSc and HPX data scaling parallel efficiency
# plt.figure(figsize=(10,5))
# # line
# plt.plot(points,points, 'k:', linewidth=1)
# # PETSc data
# points = petsc_cores_averaged[:,0]
# parallel_speedup = petsc_cores_averaged[0,4] / petsc_cores_averaged[:,4]
# plt.plot(points, parallel_speedup, 's--', c=greyscale[0], linewidth=1, label='PETSc')
# # HPX data
# points = hpx_cores_averaged[:,0]
# parallel_speedup = hpx_cores_averaged[0,5] / hpx_cores_averaged[:,5]
# plt.plot(points, parallel_speedup, 'o-', c=greyscale[3], linewidth=1, label='HPX FP64')
# points = hpx_cores_sp_averaged[:,0]
# parallel_speedup = hpx_cores_sp_averaged[0,5] / hpx_cores_sp_averaged[:,5]
# plt.plot(points, parallel_speedup, 'o-', c=greyscale[4], linewidth=1, label='HPX FP32')
# #plt.title('Parallel speedup of HPX and PETSc for different number of cores')
# plt.legend(loc='upper left')
# plt.xlabel('N cores')
# plt.xscale("log")
# labels_x = points.astype(int).astype(str)
# plt.xticks(ticks=points, labels= labels_x)
# plt.ylabel('Parallel speedup')
# plt.yscale("log")
# ticks_y = np.logspace(0, 2, num=3, endpoint=True, base=10.0, dtype=int)
# plt.yticks(ticks=ticks_y, labels=ticks_y)
# plt.savefig('figures/cores_speedup.pdf', bbox_inches='tight')