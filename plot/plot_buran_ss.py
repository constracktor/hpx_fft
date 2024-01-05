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

###
# strong scaling data for fftw_mpi_omp
ss_fftw_mpi_omp_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/buran/strong_runtimes_fftw_mpi_omp_48.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_mpi_omp_matrix.shape[0]/n_loop)
ss_fftw_mpi_omp_averaged = np.zeros((n_entries, ss_fftw_mpi_omp_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_mpi_omp_averaged[i,:] = np.mean(ss_fftw_mpi_omp_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

###
# strong scaling data for fftw_mpi_threads
ss_fftw_mpi_threads_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/buran/strong_runtimes_fftw_mpi_threads_48.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_mpi_threads_matrix.shape[0]/n_loop)
ss_fftw_mpi_threads_averaged = np.zeros((n_entries, ss_fftw_mpi_threads_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_mpi_threads_averaged[i,:] = np.mean(ss_fftw_mpi_threads_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

###
# strong scaling data for fftw_mpi_total
ss_fftw_mpi_total_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/buran/strong_runtimes_fftw_mpi_total_48.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_mpi_total_matrix.shape[0]/n_loop)
ss_fftw_mpi_total_averaged = np.zeros((n_entries, ss_fftw_mpi_total_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_mpi_total_averaged[i,:] = np.mean(ss_fftw_mpi_total_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

###
# strong scaling data for hpx loop 24
ss_loop_48_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/buran/strong_runtimes_hpx_loop_48.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_loop_48_matrix.shape[0]/n_loop)
ss_loop_48_averaged = np.zeros((n_entries, ss_loop_48_matrix.shape[1]))
for i in range (n_entries):
    ss_loop_48_averaged[i,:] = np.mean(ss_loop_48_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

# ###
# # strong scaling data for hpx loop 24_opt
# ss_loop_48_opt_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/buran/strong_runtimes_hpx_loop_48_opt.txt'), dtype='float', delimiter=';' , skip_header=1)
# n_entries = int(ss_loop_48_opt_matrix.shape[0]/n_loop)
# ss_loop_48_opt_averaged = np.zeros((n_entries, ss_loop_48_opt_matrix.shape[1]))
# for i in range (n_entries):
#     ss_loop_48_opt_averaged[i,:] = np.mean(ss_loop_48_opt_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

###
# strong scaling data for hpx task agas_48
ss_task_agas_48_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/buran/strong_runtimes_hpx_task_agas_48.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_task_agas_48_matrix.shape[0]/n_loop)
ss_task_agas_48_averaged = np.zeros((n_entries, ss_task_agas_48_matrix.shape[1]))
for i in range (n_entries):
    ss_task_agas_48_averaged[i,:] = np.mean(ss_task_agas_48_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)    

# ###
# # strong scaling data for hpx task agas_48_opt
# ss_task_agas_48_opt_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/buran/strong_runtimes_hpx_task_agas_48_opt.txt'), dtype='float', delimiter=';' , skip_header=1)
# n_entries = int(ss_task_agas_48_opt_matrix.shape[0]/n_loop)
# ss_task_agas_48_opt_averaged = np.zeros((n_entries, ss_task_agas_48_opt_matrix.shape[1]))
# for i in range (n_entries):
#     ss_task_agas_48_opt_averaged[i,:] = np.mean(ss_task_agas_48_opt_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)     

# # print how many percent optimized version is faster
# print((ss_loop_48_opt_averaged[:,10] + ss_loop_48_opt_averaged[:,14]) / (ss_loop_48_averaged[:,10] + ss_loop_48_averaged[:,14])  * 100)
# print(ss_task_agas_48_opt_averaged[:,7] / ss_task_agas_48_averaged[:,7] * 100)
# ################################################################################
# strong SCALING RUNTIME
points = [1,2,4,8,16]
plt.figure(figsize=(7,6))
# MPI total data
plt.plot(points, ss_fftw_mpi_total_averaged[:,6], 'v--', c=greyscale[0], linewidth=1, label='FFTW3 with MPI')
# MPI + OpenMP data
plt.plot(points, ss_fftw_mpi_threads_averaged[:,6], 'v--', c=greyscale[1], linewidth=1, label='FFTW3 with MPI+pthreads')
# MPI + OpenMP data
plt.plot(points, ss_fftw_mpi_omp_averaged[:,6], 'v--', c=greyscale[3], linewidth=1, label='FFTW3 with MPI+OpenMP')
# HPX loop dist data
plt.plot(points, ss_loop_48_averaged[:,7], 's--', c=colors[2], linewidth=1, label='HPX for_loop dist')
# HPX task agas dist data
plt.plot(points, ss_task_agas_48_averaged[:,7], 'o--', c=colors[4], linewidth=1, label='HPX future agas dist')
# # HPX loop opt dist data
# plt.plot(points, ss_loop_48_opt_averaged[:,7], 's--', c=colors[3], linewidth=1, label='HPX for_loop dist opt')
# # HPX task agas dist data
# plt.plot(points, ss_task_agas_48_opt_averaged[:,7], 'o--', c=colors[5], linewidth=1, label='HPX future agas dist opt')

# plot parameters
#plt.title('Strong Scaling runtime for buran cluster with 24 threads and with $2^{14}$x$2^{14}$ matrix')
plt.legend(bbox_to_anchor=(1.0, 0), loc="lower right")
plt.xlabel('N nodes')
plt.xscale("log")
labels_x = ['1','2','4','8','16']
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_buran_runtime.pdf', bbox_inches='tight')


# ################################################################################
# ################################################################################                                                
# # Bar plot
# plt.figure(figsize=(10,3.5))
    
# # plot details
# bar_width = 0.25
# epsilon = .015
# line_width= 0.5
# opacity = 1.0
# ticks_x= np.linspace(1,5,5)

# plt.rc('hatch', color='k', linewidth=0.5)
# # HPX loop distributed bars
# ss_loop_dist_first_fft = ss_loop_48_averaged[:,8]
# ss_loop_dist_first_trans = ss_loop_48_averaged[:,11]
# ss_loop_dist_second_fft = ss_loop_48_averaged[:,12]
# ss_loop_dist_second_trans = ss_loop_48_averaged[:,15]

# ss_loop_dist_first_split = ss_loop_48_averaged[:,9]
# ss_loop_dist_first_comm = ss_loop_48_averaged[:,10]
# ss_loop_dist_second_split = ss_loop_48_averaged[:,13]
# ss_loop_dist_second_comm = ss_loop_48_averaged[:,14]

# ss_loop_dist_bar_positions = ticks_x 

# ss_loop_dist_bar_first_fft = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_fft, bar_width-epsilon,
#                             color=colors[3],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='\\',
#                             alpha=opacity,
#                             label='First FFT')
# sum = ss_loop_dist_first_fft
# ss_loop_dist_bar_second_fft = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_second_fft, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[2],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='\\',
#                             alpha=opacity,
#                             label='Second FFT')
# sum+= ss_loop_dist_second_fft
# ss_loop_dist_bar_first_trans = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_trans , bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[5],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='\\',
#                             alpha=opacity,
#                             label='First transpose')
# sum+= ss_loop_dist_first_trans
# ss_loop_dist_bar_second_trans = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_second_trans, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[6],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='\\',
#                             alpha=opacity,
#                             label='Second transpose')
# sum+= ss_loop_dist_second_trans
# ss_loop_dist_bar_both_split = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_split+ ss_loop_dist_second_split, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[8],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='\\',
#                             alpha=opacity,
#                             label='Rearrange')
# sum+= ss_loop_dist_first_split+ ss_loop_dist_second_split
# ss_loop_dist_bar_both_comm = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_comm+ ss_loop_dist_second_comm, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[9],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='\\',
#                             alpha=opacity,
#                             label='Communication')

# ss_loop_dist_bars = [ss_loop_dist_bar_first_fft, ss_loop_dist_bar_second_fft, ss_loop_dist_bar_first_trans, ss_loop_dist_bar_second_trans, ss_loop_dist_bar_both_split, ss_loop_dist_bar_both_comm]
# ss_loop_dist_legend = plt.legend(ss_loop_dist_bars, ['First FFT', 'Second FFT', 'First transpose', 'Second transpose', 'Rearrange', 'Communication'], title='HPX loop dist', bbox_to_anchor=(1.0, 1.0), loc="upper left")
# plt.gca().add_artist(ss_loop_dist_legend)


# # HPX loop opt distributed bars
# ss_loop_opt_dist_first_fft = ss_loop_48_opt_averaged[:,8]
# ss_loop_opt_dist_first_trans = ss_loop_48_opt_averaged[:,11]
# ss_loop_opt_dist_second_fft = ss_loop_48_opt_averaged[:,12]
# ss_loop_opt_dist_second_trans = ss_loop_48_opt_averaged[:,15]

# ss_loop_opt_dist_first_split = ss_loop_48_opt_averaged[:,9]
# ss_loop_opt_dist_first_comm = ss_loop_48_opt_averaged[:,10]
# ss_loop_opt_dist_second_split = ss_loop_48_opt_averaged[:,13]
# ss_loop_opt_dist_second_comm = ss_loop_48_opt_averaged[:,14]

# ss_loop_opt_dist_bar_positions = ticks_x  + bar_width

# ss_loop_opt_dist_bar_first_fft = plt.bar(ss_loop_opt_dist_bar_positions, ss_loop_opt_dist_first_fft, bar_width-epsilon,
#                             color=colors[3],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='',
#                             alpha=opacity,
#                             label='First FFT')
# sum = ss_loop_opt_dist_first_fft
# ss_loop_opt_dist_bar_second_fft = plt.bar(ss_loop_opt_dist_bar_positions, ss_loop_opt_dist_second_fft, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[2],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='',
#                             alpha=opacity,
#                             label='Second FFT')
# sum+= ss_loop_opt_dist_second_fft
# ss_loop_opt_dist_bar_first_trans = plt.bar(ss_loop_opt_dist_bar_positions, ss_loop_opt_dist_first_trans , bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[5],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='',
#                             alpha=opacity,
#                             label='First transpose')
# sum+= ss_loop_opt_dist_first_trans
# ss_loop_opt_dist_bar_second_trans = plt.bar(ss_loop_opt_dist_bar_positions, ss_loop_opt_dist_second_trans, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[6],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='',
#                             alpha=opacity,
#                             label='Second transpose')
# sum+= ss_loop_opt_dist_second_trans
# ss_loop_opt_dist_bar_both_split = plt.bar(ss_loop_opt_dist_bar_positions, ss_loop_opt_dist_first_split+ ss_loop_opt_dist_second_split, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[8],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='',
#                             alpha=opacity,
#                             label='Rearrange')
# sum+= ss_loop_opt_dist_first_split+ ss_loop_opt_dist_second_split
# ss_loop_opt_dist_bar_both_comm = plt.bar(ss_loop_opt_dist_bar_positions, ss_loop_opt_dist_first_comm+ ss_loop_opt_dist_second_comm, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[9],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='',
#                             alpha=opacity,
#                             label='Communication')

# ss_loop_opt_dist_bars = [ss_loop_opt_dist_bar_first_fft, ss_loop_opt_dist_bar_second_fft, ss_loop_opt_dist_bar_first_trans, ss_loop_opt_dist_bar_second_trans, ss_loop_opt_dist_bar_both_split, ss_loop_opt_dist_bar_both_comm]
# ss_loop_opt_dist_legend = plt.legend(ss_loop_opt_dist_bars, ['First FFT', 'Second FFT', 'First transpose', 'Second transpose', 'Rearrange', 'Communication'], title='HPX loop opt dist', bbox_to_anchor=(1.0, 0.5), loc="upper left")
# plt.gca().add_artist(ss_loop_opt_dist_legend)

# # plot parameters
# plt.title('Strong Scaling distribution for buran cluster with with 24 threads and $2^{14}$x$2^{14}$ matrix')
# plt.xlabel('N nodes')
# #plt.xscale("log")
# labels_x = ['1','2','4','8','16']
# plt.xticks(ticks=ticks_x, labels= labels_x)
# #plt.yscale("log")
# plt.ylabel('Runtime in s')
# plt.savefig('plot/figures/strong_scaling_buran_distribution.pdf', bbox_inches='tight')