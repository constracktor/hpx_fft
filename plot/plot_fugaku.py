import numpy as np
import os
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 14})
matplotlib.pyplot.title(r'ABC123 vs $\mathrm{ABC123}^{123}$')
# CVD accessible colors
# black - dark red - indigo (blueish) - yellow - teal (greenish) - light gray
#colors = ['#000000','#c1272d', '#0000a7', '#eecc16', '#008176', '#b3b3b3']
#colors = ['#000000','#E69F00', '#56B4E9', '#009E73', '#F0E442', '#0072B2', '#D55E00', '#CC79A7']

#https://lospec.com/palette-list/krzywinski-colorblind-16
#          0 black    1 d-green  2 m-green 3 l-green  4 d-blue  5 m-blue   6 l-blue  7 d-purple  8 l-purple  9 d-pink  10 l-pink  11 r-brown 12 d-brown 13 l-brown  14 y-brown  15 white
colors = ['#000000','#004949', '#009292', '#24ff24', '#006edb', '#6db6ff', '#b6dbff', '#490092', '#b66dff', '#ff6db6', '#ffb6db', '#920000', '#924900', '#db6d00', '#ffff6d', '#ffffff']
# black - dark grey - grey - light grey - very light grey
greyscale = ['#000000', '#333333', '#666666', '#999999', '#cccccc']

# get header
n_loop = 5

##########################################
# MEASURE
# ###
# # strong scaling data for hpx loop shared
# ss_loop_measure_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_hpx_loop_shared_measure.txt'), dtype='float', delimiter=';' , skip_header=1)
# n_entries = int(ss_loop_measure_shared_matrix.shape[0]/n_loop) 
# ss_loop_measure_shared_averaged = np.zeros((n_entries, ss_loop_measure_shared_matrix.shape[1]))
# for i in range (n_entries):
#     ss_loop_measure_shared_averaged[i,:] = np.mean(ss_loop_measure_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
# ###
# strong scaling data for fftw_pt_measure
ss_fftw_pt_measure_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_fftw_threads_measure.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_pt_measure_matrix.shape[0]/n_loop) 
ss_fftw_pt_measure_averaged = np.zeros((n_entries, ss_fftw_pt_measure_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_pt_measure_averaged[i,:] = np.mean(ss_fftw_pt_measure_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for fftw_omp_measure
ss_fftw_omp_measure_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_fftw_omp_measure.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_omp_measure_matrix.shape[0]/n_loop) 
ss_fftw_omp_measure_averaged = np.zeros((n_entries, ss_fftw_omp_measure_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_omp_measure_averaged[i,:] = np.mean(ss_fftw_omp_measure_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# # strong scaling data for fftw_mpi_measure
# ss_fftw_mpi_measure_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_fftw_mpi_measure.txt'), dtype='float', delimiter=';' , skip_header=1)
# n_entries = int(ss_fftw_mpi_measure_matrix.shape[0]/n_loop)
# ss_fftw_mpi_measure_averaged = np.zeros((n_entries, ss_fftw_mpi_measure_matrix.shape[1]))
# for i in range (n_entries):
#     ss_fftw_mpi_measure_averaged[i,:] = np.mean(ss_fftw_mpi_measure_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
# ###
# # strong scaling data for fftw_hpx_measure
# ss_fftw_hpx_measure_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_fftw_hpx_measure.txt'), dtype='float', delimiter=';' , skip_header=1)
# n_entries = int(ss_fftw_hpx_measure_matrix.shape[0]/n_loop) 
# ss_fftw_hpx_measure_averaged = np.zeros((n_entries, ss_fftw_hpx_measure_matrix.shape[1]))
# for i in range (n_entries):
#     ss_fftw_hpx_measure_averaged[i,:] = np.mean(ss_fftw_hpx_measure_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)


##########################################
# estimate
# ###
# # strong scaling data for hpx loop shared
# ss_loop_estimate_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_hpx_loop_shared_estimate.txt'), dtype='float', delimiter=';' , skip_header=1)
# n_entries = int(ss_loop_estimate_shared_matrix.shape[0]/n_loop) 
# ss_loop_estimate_shared_averaged = np.zeros((n_entries, ss_loop_estimate_shared_matrix.shape[1]))
# for i in range (n_entries):
#     ss_loop_estimate_shared_averaged[i,:] = np.mean(ss_loop_estimate_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
# ###
# strong scaling data for fftw_pt_estimate
ss_fftw_pt_estimate_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_fftw_threads_estimate.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_pt_estimate_matrix.shape[0]/n_loop) 
ss_fftw_pt_estimate_averaged = np.zeros((n_entries, ss_fftw_pt_estimate_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_pt_estimate_averaged[i,:] = np.mean(ss_fftw_pt_estimate_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for fftw_omp_estimate
ss_fftw_omp_estimate_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_fftw_omp_estimate.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_omp_estimate_matrix.shape[0]/n_loop) 
ss_fftw_omp_estimate_averaged = np.zeros((n_entries, ss_fftw_omp_estimate_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_omp_estimate_averaged[i,:] = np.mean(ss_fftw_omp_estimate_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
# ###
# # strong scaling data for fftw_mpi_estimate
# ss_fftw_mpi_estimate_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_fftw_mpi_estimate.txt'), dtype='float', delimiter=';' , skip_header=1)
# n_entries = int(ss_fftw_mpi_estimate_matrix.shape[0]/n_loop)
# ss_fftw_mpi_estimate_averaged = np.zeros((n_entries, ss_fftw_mpi_estimate_matrix.shape[1]))
# for i in range (n_entries):
#     ss_fftw_mpi_estimate_averaged[i,:] = np.mean(ss_fftw_mpi_estimate_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
# ###
# # strong scaling data for fftw_hpx_estimate
# ss_fftw_hpx_estimate_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/fugaku/strong_runtimes_fftw_hpx_estimate.txt'), dtype='float', delimiter=';' , skip_header=1)
# n_entries = int(ss_fftw_hpx_estimate_matrix.shape[0]/n_loop) 
# ss_fftw_hpx_estimate_averaged = np.zeros((n_entries, ss_fftw_hpx_estimate_matrix.shape[1]))
# for i in range (n_entries):
#     ss_fftw_hpx_estimate_averaged[i,:] = np.mean(ss_fftw_hpx_estimate_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

# strong SCALING RUNTIME FFTW3
plt.figure(figsize=(7,6))
points = ss_fftw_pt_estimate_averaged[:,1]
# ESTIMATE
# # MPI data
# plt.plot(points, ss_fftw_mpi_averaged[:,6], 'v--', c=greyscale[0], linewidth=1, label='FFTW3 with MPI')
# PThread data
plt.plot(points, ss_fftw_pt_estimate_averaged[:,6], 'd--', c=greyscale[2], linewidth=1, label='FFTW3 with pthreads estimate')
# OpenMP data
plt.plot(points, ss_fftw_omp_estimate_averaged[:,6], 'd--', c=greyscale[3], linewidth=1, label='FFTW3 with OpenMP estimate')
# # HPX data
# plt.plot(points, ss_fftw_hpx_averaged[:,5], 'v-', c=greyscale[4], linewidth=1, label='FFTW3 with HPX') 
# # HPX loop shared data
# plt.plot(points, ss_loop_shared_averaged[:,7], 's-', c=colors[2], linewidth=1, label='HPX for_loop')

# MEASURE
# # MPI data
# plt.plot(points, ss_fftw_mpi_measure_averaged[:,6], 'v--', c=greyscale[0], linewidth=1, label='FFTW3 with MPI')
# PThread data
plt.plot(points, ss_fftw_pt_measure_averaged[:,6], 'v-', c=greyscale[2], linewidth=1, label='FFTW3 with pthreads measure')
# OpenMP data
plt.plot(points, ss_fftw_omp_measure_averaged[:,6], 'v-', c=greyscale[3], linewidth=1, label='FFTW3 with OpenMP measure')
# # HPX data
# plt.plot(points, ss_fftw_hpx_measure_averaged[:,5], 'v-', c=greyscale[4], linewidth=1, label='FFTW3 with HPX') 
# # HPX loop measure shared data
# plt.plot(points, ss_loop_measure_shared_averaged[:,7], 's-', c=colors[2], linewidth=1, label='HPX for_loop (measure)')
# # HPX loop shared data
# plt.plot(points, ss_loop_shared_averaged[:,7], 's-', c=colors[3], linewidth=1, label='HPX for_loop (estimate)')

# plot parameters
#plt.title('Strong Scaling runtime for shared-memory ipvs-epyc2 with $2^{14}$x$2^{14}$ matrix')
plt.legend(bbox_to_anchor=(0, 1.0), loc="upper left")
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_fugaku_fftw3_runtime.pdf', bbox_inches='tight')

