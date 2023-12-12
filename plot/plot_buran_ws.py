import numpy as np
import os
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 1})
matplotlib.pyplot.title(r'ABC123 vs $\mathrm{ABC123}^{123}$')
# CVD accewsible colors
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
# weak scaling data for fftw_mpi_omp
ws_fftw_mpi_omp_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/buran/weak_runtimes_fftw_mpi_omp.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_fftw_mpi_omp_matrix.shape[0]/n_loop)
ws_fftw_mpi_omp_averaged = np.zeros((n_entries, ws_fftw_mpi_omp_matrix.shape[1]))
for i in range (n_entries):
    ws_fftw_mpi_omp_averaged[i,:] = np.mean(ws_fftw_mpi_omp_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

###
# weak scaling data for fftw_mpi_total
ws_fftw_mpi_total_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/buran/weak_runtimes_fftw_mpi_total.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_fftw_mpi_total_matrix.shape[0]/n_loop)
ws_fftw_mpi_total_averaged = np.zeros((n_entries, ws_fftw_mpi_total_matrix.shape[1]))
for i in range (n_entries):
    ws_fftw_mpi_total_averaged[i,:] = np.mean(ws_fftw_mpi_total_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

###
# weak scaling data for hpx loop 24
ws_loop_24_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/buran/weak_runtimes_hpx_loop_24.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_loop_24_matrix.shape[0]/n_loop)
ws_loop_24_averaged = np.zeros((n_entries, ws_loop_24_matrix.shape[1]))
for i in range (n_entries):
    ws_loop_24_averaged[i,:] = np.mean(ws_loop_24_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)

###
# weak scaling data for hpx task agas_24
ws_task_agas_24_matrix = np.genfromtxt(os.path.abspath('./plot/data/weak_scaling/buran/weak_runtimes_hpx_task_agas_24.txt'), dtype='float', delimiter=';' , skip_header=1)
n_entries = int(ws_task_agas_24_matrix.shape[0]/n_loop)
ws_task_agas_24_averaged = np.zeros((n_entries, ws_task_agas_24_matrix.shape[1]))
for i in range (n_entries):
    ws_task_agas_24_averaged[i,:] = np.mean(ws_task_agas_24_matrix[i*n_loop:(i+1)*n_loop,:],axis=0)     


################################################################################
# weak SCALING RUNTIME
points = [1,4,16]
plt.figure(figsize=(10,3))
# MPI + OpenMP data
plt.plot(points, ws_fftw_mpi_omp_averaged[:,6], 'v-.', c=greyscale[0], linewidth=1, label='FFTW3 with MPI+OpenMP')
# MPI total data
plt.plot(points, ws_fftw_mpi_total_averaged[:,6], 'v-.', c=greyscale[3], linewidth=1, label='FFTW3 with MPI all ranks/node')
# HPX loop dist data
plt.plot(points, ws_loop_24_averaged[:,7], 's--', c=colors[3], linewidth=1, label='HPX loop dist')
# HPX task agas dist data
plt.plot(points, ws_task_agas_24_averaged[:,7], 's--', c=colors[5], linewidth=1, label='HPX task agas dist')

# plot parameters
plt.title('Weak Scaling runtime for buran cluster with 24 threads and with $2^{14}$ base dimension')
plt.legend(bbox_to_anchor=(1.0, 1), loc="upper left")
plt.xlabel('N nodes')
plt.xscale("log")
labels_x = ['1','4','16']
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/weak_scaling_buran_runtime.pdf', bbox_inches='tight')


################################################################################
################################################################################                                                
# Bar plot
plt.figure(figsize=(10,3.5))
    
# plot details
bar_width = 0.25
epsilon = .015
line_width= 0.5
opacity = 1.0
ticks_x= np.linspace(1,3,3)

plt.rc('hatch', color='k', linewidth=0.5)
# HPX loop distributed bars
ws_loop_dist_first_fft = ws_loop_24_averaged[:,8]
ws_loop_dist_first_trans = ws_loop_24_averaged[:,11]
ws_loop_dist_second_fft = ws_loop_24_averaged[:,12]
ws_loop_dist_second_trans = ws_loop_24_averaged[:,15]

ws_loop_dist_first_split = ws_loop_24_averaged[:,9]
ws_loop_dist_first_comm = ws_loop_24_averaged[:,10]
ws_loop_dist_second_split = ws_loop_24_averaged[:,13]
ws_loop_dist_second_comm = ws_loop_24_averaged[:,14]

ws_loop_dist_bar_positions = ticks_x

ws_loop_dist_bar_first_fft = plt.bar(ws_loop_dist_bar_positions, ws_loop_dist_first_fft, bar_width-epsilon,
                            color=colors[3],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='First FFT')
sum = ws_loop_dist_first_fft
ws_loop_dist_bar_second_fft = plt.bar(ws_loop_dist_bar_positions, ws_loop_dist_second_fft, bar_width-epsilon,
                            bottom=sum,
                            color=colors[2],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Second FFT')
sum+= ws_loop_dist_second_fft
ws_loop_dist_bar_first_trans = plt.bar(ws_loop_dist_bar_positions, ws_loop_dist_first_trans , bar_width-epsilon,
                            bottom=sum,
                            color=colors[5],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='First transpose')
sum+= ws_loop_dist_first_trans
ws_loop_dist_bar_second_trans = plt.bar(ws_loop_dist_bar_positions, ws_loop_dist_second_trans, bar_width-epsilon,
                            bottom=sum,
                            color=colors[6],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Second transpose')
sum+= ws_loop_dist_second_trans
ws_loop_dist_bar_both_split = plt.bar(ws_loop_dist_bar_positions, ws_loop_dist_first_split+ ws_loop_dist_second_split, bar_width-epsilon,
                            bottom=sum,
                            color=colors[8],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Rearrange')
sum+= ws_loop_dist_first_split+ ws_loop_dist_second_split
ws_loop_dist_bar_both_comm = plt.bar(ws_loop_dist_bar_positions, ws_loop_dist_first_comm+ ws_loop_dist_second_comm, bar_width-epsilon,
                            bottom=sum,
                            color=colors[9],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Communication')

ws_loop_dist_bars = [ws_loop_dist_bar_first_fft, ws_loop_dist_bar_second_fft, ws_loop_dist_bar_first_trans, ws_loop_dist_bar_second_trans, ws_loop_dist_bar_both_split, ws_loop_dist_bar_both_comm]
ws_loop_dist_legend = plt.legend(ws_loop_dist_bars, ['First FFT', 'Second FFT', 'First transpose', 'Second transpose', 'Rearrange', 'Communication'], title='HPX loop dist 24 threads', bbox_to_anchor=(1.0, 1), loc="upper left")
plt.gca().add_artist(ws_loop_dist_legend)

# plot parameters
plt.title('Weak Scaling distribution for buran cluster with $2^{14}$ base dimension')
plt.xlabel('N nodes')
#plt.xscale("log")
labels_x = ['1','4','16']
plt.xticks(ticks=ticks_x, labels= labels_x)
#plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/weak_scaling_buran_distribution.pdf', bbox_inches='tight')