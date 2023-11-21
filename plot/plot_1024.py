import numpy as np
import os
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 12})
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

################################################################################
################################################################################
# STRONG SCALING
# strong scaling data for hpx loop shared optimized
###
# strong scaling data for hpx loop shared
ss_loop_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_hpx_loop_shared.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_loop_shared_matrix.shape[0]/n_loop) - 1
ss_loop_shared_averaged = np.zeros((n_entries, ss_loop_shared_matrix.shape[1]))
for i in range (n_entries):
    ss_loop_shared_averaged[i,:] = np.mean(ss_loop_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for hpx loop scatter
ss_loop_scatter_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_hpx_loop_scatter.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_loop_scatter_matrix.shape[0]/n_loop) - 1
ss_loop_scatter_averaged = np.zeros((n_entries, ss_loop_scatter_matrix.shape[1]))
for i in range (n_entries):
    ss_loop_scatter_averaged[i,:] = np.mean(ss_loop_scatter_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for hpx task_agas_shared
ss_task_agas_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_hpx_task_agas_shared.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_task_agas_shared_matrix.shape[0]/n_loop) - 1
ss_task_agas_shared_averaged = np.zeros((n_entries, ss_task_agas_shared_matrix.shape[1]))
for i in range (n_entries):
    ss_task_agas_shared_averaged[i,:] = np.mean(ss_task_agas_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for hpx task_naive_shared
ss_task_naive_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_hpx_task_naive_shared.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_task_naive_shared_matrix.shape[0]/n_loop) - 1
ss_task_naive_shared_averaged = np.zeros((n_entries, ss_task_naive_shared_matrix.shape[1]))
for i in range (n_entries):
    ss_task_naive_shared_averaged[i,:] = np.mean(ss_task_naive_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for hpx loop shared
ss_task_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_hpx_task_shared.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_task_shared_matrix.shape[0]/n_loop) - 1
ss_task_shared_averaged = np.zeros((n_entries, ss_task_shared_matrix.shape[1]))
for i in range (n_entries):
    ss_task_shared_averaged[i,:] = np.mean(ss_task_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for hpx loop scatter
ss_task_scatter_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_hpx_task_scatter.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_task_scatter_matrix.shape[0]/n_loop) - 1
ss_task_scatter_averaged = np.zeros((n_entries, ss_task_scatter_matrix.shape[1]))
for i in range (n_entries):
    ss_task_scatter_averaged[i,:] = np.mean(ss_task_scatter_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for hpx loop task_sync_shared
ss_task_sync_shared_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_hpx_task_sync_shared.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_task_sync_shared_matrix.shape[0]/n_loop) - 1
ss_task_sync_shared_averaged = np.zeros((n_entries, ss_task_sync_shared_matrix.shape[1]))
for i in range (n_entries):
    ss_task_sync_shared_averaged[i,:] = np.mean(ss_task_sync_shared_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for fftw_omp
ss_fftw_omp_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_fftw_omp.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ss_fftw_omp_matrix.shape[0]/n_loop) - 1
ss_fftw_omp_averaged = np.zeros((n_entries, ss_fftw_omp_matrix.shape[1]))
for i in range (n_entries):
    ss_fftw_omp_averaged[i,:] = np.mean(ss_fftw_omp_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)
###
# strong scaling data for fftw_mpi
ss_fftw_mpi_matrix = np.genfromtxt(os.path.abspath('./plot/data/strong_scaling/1024/strong_runtimes_fftw_mpi.txt'), dtype='float', delimiter=';' , skip_header=1)
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
# OpenMP data
parallel_efficiency = 100 * ss_fftw_omp_averaged[0,6] / (ss_fftw_omp_averaged[:,6] * ss_fftw_omp_averaged[:,1])
plt.plot(points, parallel_efficiency, 'v-.', c=greyscale[0], linewidth=1, label='FFTW3 with OpenMP')
# MPI data
parallel_efficiency = 100 * ss_fftw_mpi_averaged[0,6] / (ss_fftw_mpi_averaged[:,6] * ss_fftw_mpi_averaged[:,0])
plt.plot(points, parallel_efficiency, 'v-.', c=greyscale[3], linewidth=1, label='FFTW3 with MPI')
# HPX loop shared data
parallel_efficiency = 100 * ss_loop_shared_averaged[0,7] / (ss_loop_shared_averaged[:,7] * ss_loop_shared_averaged[:,0])
plt.plot(points, parallel_efficiency, 'o-', c=colors[2], linewidth=1, label='HPX loop shared')
# HPX task sync shared data
parallel_efficiency = 100 * ss_task_sync_shared_averaged[0,6] / (ss_task_sync_shared_averaged[:,6] * ss_task_sync_shared_averaged[:,0])
plt.plot(points, parallel_efficiency, 'o-', c=colors[9], linewidth=1, label='HPX task sync shared')
# HPX task shared data
parallel_efficiency = 100 * ss_task_shared_averaged[0,6] / (ss_task_shared_averaged[:,6] * ss_task_shared_averaged[:,0])
plt.plot(points, parallel_efficiency, 'o-', c=colors[12], linewidth=1, label='HPX task shared')
# HPX task agas shared data
parallel_efficiency = 100 * ss_task_agas_shared_averaged[0,6] / (ss_task_agas_shared_averaged[:,6] * ss_task_agas_shared_averaged[:,0])
plt.plot(points, parallel_efficiency, 'o-', c=colors[4], linewidth=1, label='HPX task agas shared')
# HPX loop dist data
parallel_efficiency = 100 * ss_loop_scatter_averaged[0,7] / (ss_loop_scatter_averaged[:,7] * ss_loop_scatter_averaged[:,0])
plt.plot(points, parallel_efficiency, 's--', c=colors[3], linewidth=1, label='HPX loop dist')
# HPX task agas dist data
parallel_efficiency = 100 * ss_task_scatter_averaged[0,7] / (ss_task_scatter_averaged[:,7] * ss_task_scatter_averaged[:,0])
plt.plot(points, parallel_efficiency, 's--', c=colors[5], linewidth=1, label='HPX task agas dist')






# plot parameters
plt.title('Strong Scaling efficiency for shared-memory ipvs-epyc2 with $2^{10}$x$2^{10}$ matrix')
plt.legend(bbox_to_anchor=(1.0, 1), loc="upper left")
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.ylabel('Parallel efficiency in %')
ticks_y = np.linspace(0, 100, num=11, endpoint=True, dtype=int)
plt.yticks(ticks=ticks_y)
plt.savefig('plot/figures/strong_scaling_1024_efficiency.pdf', bbox_inches='tight')
################################################################################
# strong SCALING RUNTIME
plt.figure(figsize=(10,3))
# OpenMP data
plt.plot(points, ss_fftw_omp_averaged[:,6], 'v-.', c=greyscale[0], linewidth=1, label='FFTW3 with OpenMP')
# MPI data
plt.plot(points, ss_fftw_mpi_averaged[:,6], 'v-.', c=greyscale[3], linewidth=1, label='FFTW3 with MPI')
# HPX loop shared data
plt.plot(points, ss_loop_shared_averaged[:,7], 'o-', c=colors[2], linewidth=1, label='HPX loop shared')
# HPX task sync shared data
plt.plot(points, ss_task_sync_shared_averaged[:,6], 'o-', c=colors[9], linewidth=1, label='HPX task sync shared')
# HPX task shared data
plt.plot(points, ss_task_shared_averaged[:,6], 'o-', c=colors[12], linewidth=1, label='HPX task shared')
# HPX task agas shared data
plt.plot(points, ss_task_agas_shared_averaged[:,6], 'o-', c=colors[4], linewidth=1, label='HPX task agas shared')
# HPX task naive shared data
plt.plot(points, ss_task_naive_shared_averaged[:,6], 'o-', c=colors[13], linewidth=1, label='HPX task naive shared')
# HPX loop dist data
plt.plot(points, ss_loop_scatter_averaged[:,7], 's--', c=colors[3], linewidth=1, label='HPX loop dist')
# HPX task agas dist data
plt.plot(points, ss_task_scatter_averaged[:,7], 's--', c=colors[5], linewidth=1, label='HPX task agas dist')

# plot parameters
plt.title('Strong Scaling runtime for shared-memory ipvs-epyc2 with $2^{10}$x$2^{10}$ matrix')
plt.legend(bbox_to_anchor=(1.0, 1), loc="upper left")
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_1024_runtime.pdf', bbox_inches='tight')


################################################################################
# CORES PARALLEL SPEEDUP
# parallel efficiency
plt.figure(figsize=(10,3))
# line
plt.plot(points,points, 'k:', linewidth=1)
# OpenMP data
parallel_speedup = ss_fftw_omp_averaged[0,6] / (ss_fftw_omp_averaged[:,6])
plt.plot(points, parallel_speedup, 'v-.', c=greyscale[0], linewidth=1, label='FFTW3 with OpenMP')
# MPI data
parallel_speedup = ss_fftw_mpi_averaged[0,6] / (ss_fftw_mpi_averaged[:,6])
plt.plot(points, parallel_speedup, 'v-.', c=greyscale[3], linewidth=1, label='FFTW3 with MPI')
# HPX loop shared data
parallel_speedup = ss_loop_shared_averaged[0,7] / (ss_loop_shared_averaged[:,7])
plt.plot(points, parallel_speedup, 'o-', c=colors[2], linewidth=1, label='HPX loop shared')
# HPX task sync shared data
parallel_speedup = ss_task_sync_shared_averaged[0,6] / (ss_task_sync_shared_averaged[:,6])
plt.plot(points, parallel_speedup, 'o-', c=colors[9], linewidth=1, label='HPX task sync shared')
# HPX task shared data
parallel_speedup = ss_task_shared_averaged[0,6] / (ss_task_shared_averaged[:,6])
plt.plot(points, parallel_speedup, 'o-', c=colors[12], linewidth=1, label='HPX task shared')
# HPX task agas shared data
parallel_speedup = ss_task_agas_shared_averaged[0,6] / (ss_task_agas_shared_averaged[:,6])
plt.plot(points, parallel_speedup, 'o-', c=colors[4], linewidth=1, label='HPX task agas shared')
# HPX loop dist data
parallel_speedup = ss_loop_scatter_averaged[0,7] / (ss_loop_scatter_averaged[:,7])
plt.plot(points, parallel_speedup, 's--', c=colors[3], linewidth=1, label='HPX loop dist')
# HPX task agas dist data
parallel_speedup = ss_task_scatter_averaged[0,7] / (ss_task_scatter_averaged[:,7])
plt.plot(points, parallel_speedup, 's--', c=colors[5], linewidth=1, label='HPX task agas dist')

# plot parameters
plt.title('Parallel speedup for shared-memory ipvs-epyc2 with $2^{10}$x$2^{10}$ matrix')
plt.legend(bbox_to_anchor=(1.0, 1), loc="upper left")
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.ylabel('Parallel speedup')
plt.yscale("log")
ticks_y = np.logspace(0, 2, num=3, endpoint=True, base=10.0, dtype=int)
plt.yticks(ticks=ticks_y, labels=ticks_y)
plt.savefig('plot/figures/strong_scaling_1024_speedup.pdf', bbox_inches='tight')
################################################################################
################################################################################                                                
# Bar plot
plt.figure(figsize=(10,3.5))
    
# plot details
bar_width = 0.25
epsilon = .015
line_width= 0.5
opacity = 1.0
ticks_x= np.linspace(1,8,8)

plt.rc('hatch', color='k', linewidth=0.5)
# HPX loop bars
ss_loop_first_fft = ss_loop_shared_averaged[:,8]
ss_loop_first_trans = ss_loop_shared_averaged[:,9]
ss_loop_second_fft = ss_loop_shared_averaged[:,10]
ss_loop_second_trans = ss_loop_shared_averaged[:,11]

ss_loop_bar_positions = ticks_x

ss_loop_bar_first_fft = plt.bar(ss_loop_bar_positions, ss_loop_first_fft, bar_width-epsilon,
                            color=colors[3],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='')
ss_loop_bar_second_fft = plt.bar(ss_loop_bar_positions, ss_loop_second_fft, bar_width-epsilon,
                            bottom=ss_loop_first_fft,
                            color=colors[2],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='')
ss_loop_bar_first_trans = plt.bar(ss_loop_bar_positions, ss_loop_first_trans , bar_width-epsilon,
                            bottom=ss_loop_first_fft+ss_loop_second_fft,
                            color=colors[5],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='')
ss_loop_bar_second_trans = plt.bar(ss_loop_bar_positions, ss_loop_second_trans, bar_width-epsilon,
                            bottom=ss_loop_first_fft+ss_loop_first_trans+ss_loop_second_fft,
                            color=colors[6],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='')

ss_loop_bars = [ss_loop_bar_first_fft, ss_loop_bar_second_fft, ss_loop_bar_first_trans, ss_loop_bar_second_trans]
ss_loop_legend = plt.legend(ss_loop_bars, ['First FFT', 'Second FFT', 'First transpose', 'Second transpose'], title='HPX loop shared', bbox_to_anchor=(0.75, 1.0), loc="upper right")
plt.gca().add_artist(ss_loop_legend)

# HPX task async bars
ss_task_sync_first_fft = ss_task_sync_shared_averaged[:,7]
ss_task_sync_first_trans = ss_task_sync_shared_averaged[:,8]
ss_task_sync_second_fft = ss_task_sync_shared_averaged[:,9]
ss_task_sync_second_trans = ss_task_sync_shared_averaged[:,10]

ss_task_sync_bar_positions = ss_loop_bar_positions - bar_width

ss_task_sync_bar_first_fft = plt.bar(ss_task_sync_bar_positions, ss_task_sync_first_fft, bar_width-epsilon,
                            color=colors[3],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='///',
                            alpha=opacity,
                            label='First FFT')
ss_task_sync_bar_second_fft = plt.bar(ss_task_sync_bar_positions, ss_task_sync_second_fft, bar_width-epsilon,
                            bottom=ss_task_sync_first_fft,
                            color=colors[2],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='///',
                            alpha=opacity,
                            label='Second FFT')
ss_task_sync_bar_first_trans = plt.bar(ss_task_sync_bar_positions, ss_task_sync_first_trans , bar_width-epsilon,
                            bottom=ss_task_sync_first_fft+ss_task_sync_second_fft,
                            color=colors[5],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='///',
                            alpha=opacity,
                            label='First transpose')
ss_task_sync_bar_second_trans = plt.bar(ss_task_sync_bar_positions, ss_task_sync_second_trans, bar_width-epsilon,
                            bottom=ss_task_sync_first_fft+ss_task_sync_first_trans+ss_task_sync_second_fft,
                            color=colors[6],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='///',
                            alpha=opacity,
                            label='Second transpose')

ss_task_sync_bars = [ss_task_sync_bar_first_fft, ss_task_sync_bar_second_fft, ss_task_sync_bar_first_trans, ss_task_sync_bar_second_trans]
ss_task_sync_legend = plt.legend(ss_task_sync_bars, ['First FFT', 'Second FFT', 'First transpose', 'Second transpose'], title='HPX task sync shared', bbox_to_anchor=(1.0, 1.0), loc="upper right")
plt.gca().add_artist(ss_task_sync_legend)

# HPX loop distributed bars
ss_loop_dist_first_fft = ss_loop_scatter_averaged[:,8]
ss_loop_dist_first_trans = ss_loop_scatter_averaged[:,11]
ss_loop_dist_second_fft = ss_loop_scatter_averaged[:,12]
ss_loop_dist_second_trans = ss_loop_scatter_averaged[:,15]

ss_loop_dist_first_split = ss_loop_scatter_averaged[:,9]
ss_loop_dist_first_comm = ss_loop_scatter_averaged[:,10]
ss_loop_dist_second_split = ss_loop_scatter_averaged[:,13]
ss_loop_dist_second_comm = ss_loop_scatter_averaged[:,14]

ss_loop_dist_bar_positions = ss_loop_bar_positions + bar_width

ss_loop_dist_bar_first_fft = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_fft, bar_width-epsilon,
                            color=colors[3],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='First FFT')
sum = ss_loop_dist_first_fft
ss_loop_dist_bar_second_fft = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_second_fft, bar_width-epsilon,
                            bottom=sum,
                            color=colors[2],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Second FFT')
sum+= ss_loop_dist_second_fft
ss_loop_dist_bar_first_trans = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_trans , bar_width-epsilon,
                            bottom=sum,
                            color=colors[5],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='First transpose')
sum+= ss_loop_dist_first_trans
ss_loop_dist_bar_second_trans = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_second_trans, bar_width-epsilon,
                            bottom=sum,
                            color=colors[6],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Second transpose')
sum+= ss_loop_dist_second_trans
ss_loop_dist_bar_both_split = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_split+ ss_loop_dist_second_split, bar_width-epsilon,
                            bottom=sum,
                            color=colors[8],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Data preparation')
sum+= ss_loop_dist_first_split+ ss_loop_dist_second_split
ss_loop_dist_bar_both_comm = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_comm+ ss_loop_dist_second_comm, bar_width-epsilon,
                            bottom=sum,
                            color=colors[9],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Communication (very small)')

ss_loop_dist_bars = [ss_loop_dist_bar_first_fft, ss_loop_dist_bar_second_fft, ss_loop_dist_bar_first_trans, ss_loop_dist_bar_second_trans, ss_loop_dist_bar_both_split, ss_loop_dist_bar_both_comm]
ss_loop_dist_legend = plt.legend(ss_loop_dist_bars, ['First FFT', 'Second FFT', 'First transpose', 'Second transpose', 'Data preparation', 'Communication'], title='HPX loop dist', bbox_to_anchor=(0.5, 1.0), loc="upper right")
plt.gca().add_artist(ss_loop_dist_legend)

# plot parameters
plt.title('Strong Scaling distribution for shared-memory ipvs-epyc2 with $2^{10}$x$2^{10}$ matrix')
plt.xlabel('N cores')
#plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=ticks_x, labels= labels_x)
#plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_1024_distribution.pdf', bbox_inches='tight')