import numpy as np
import matplotlib
import matplotlib.pyplot as plt

from plot_general import plot_object
from plot_general import colors_16 as colors

# Configure font and fontsize
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 16})
matplotlib.pyplot.title(r'ABC123 vs $\mathrm{ABC123}^{123}$')
matplotlib.rcParams.update({'errorbar.capsize': 5})

# set number of runs
n_loop = 50
# create print objects
ss_fftw_mpi_omp = plot_object('./plot/data/strong_scaling/buran_median/strong_runtimes_fftw_mpi_omp_48.txt', n_loop)
ss_fftw_mpi_threads = plot_object('./plot/data/strong_scaling/buran_median/strong_runtimes_fftw_mpi_threads_48.txt', n_loop)

ss_loop_mpi = plot_object('./plot/data/strong_scaling/buran_median/strong_runtimes_hpx_loop_48.txt', n_loop)
ss_loop_lci = plot_object('./plot/data/strong_scaling/buran_median/strong_runtimes_hpx_loop_48_lci.txt', n_loop)

ss_task_agas_mpi = plot_object('./plot/data/strong_scaling/buran_median/strong_runtimes_hpx_task_agas_48.txt', n_loop)
ss_task_agas_lci = plot_object('./plot/data/strong_scaling/buran_median/strong_runtimes_hpx_task_agas_48_lci.txt', n_loop)


################################################################################
# strong SCALING RUNTIME
points = np.array([1,2,4,8,16])
plt.figure(figsize=(7,6))
# ideal scaling
ideal = 2 * ss_loop_lci.median[1,7] / points
plt.plot(points, ideal, '--', c=colors[0], linewidth=1.5)

# error bars
plt.errorbar(points, ss_fftw_mpi_omp.median[:,6], yerr = ss_fftw_mpi_omp.min_max_error(6), fmt='v-', c=colors[12], linewidth=2, label='FFTW3 with MPI+OpenMP')
plt.errorbar(points, ss_fftw_mpi_threads.median[:,6], yerr = ss_fftw_mpi_threads.min_max_error(6), fmt='v-', c=colors[13], linewidth=2, label='FFTW3 with MPI+pthreads')

plt.errorbar(points, ss_task_agas_mpi.median[:,7], yerr = ss_task_agas_mpi.min_max_error(7), fmt='o-', c =colors[5], linewidth=2, label='HPX future agas with MPI')
plt.errorbar(points, ss_task_agas_lci.median[:,7], yerr = ss_task_agas_lci.min_max_error(7), fmt='o-', c=colors[4], linewidth=2, label='HPX future agas with LCI')

plt.errorbar(points, ss_loop_mpi.median[:,7], yerr = ss_loop_mpi.min_max_error(7), fmt='s-', c=colors[3], linewidth=2, label='HPX for_loop with MPI')
plt.errorbar(points, ss_loop_lci.median[:,7], yerr = ss_loop_lci.min_max_error(7), fmt='s-', c=colors[2], linewidth=2, label='HPX for_loop with LCI')

# plot parameters
#plt.title('Strong Scaling runtime for buran cluster with 48 threads and with $2^{14}$x$2^{14}$ matrix')
plt.legend(bbox_to_anchor=(0, 0), loc="lower left")
#plt.legend(bbox_to_anchor=(0.1, 0), loc="lower left")
#plt.legend(bbox_to_anchor=(1, 0), loc="lower left")
plt.xlabel('N nodes')
plt.xscale("log")
labels_x = ['1','2','4','8','16']
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.yticks(ticks=[0.01,0.1, 1.0, 10.0])
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_buran_runtime.pdf', bbox_inches='tight')


################################################################################
################################################################################                                                
# Bar plot
plt.figure(figsize=(12,10))
    
# plot details
bar_width = 0.25
epsilon = .015
line_width= 0.5
opacity = 1.0
ticks_x= np.linspace(1,5,5)

plt.rc('hatch', color='k', linewidth=0.5)
# HPX loop distributed bars
ss_loop_dist_first_fft = ss_loop_mpi.median[:,8]
ss_loop_dist_first_trans = ss_loop_mpi.median[:,11]
ss_loop_dist_second_fft = ss_loop_mpi.median[:,12]
ss_loop_dist_second_trans = ss_loop_mpi.median[:,15]

ss_loop_dist_first_split = ss_loop_mpi.median[:,9]
ss_loop_dist_first_comm = ss_loop_mpi.median[:,10]
ss_loop_dist_second_split = ss_loop_mpi.median[:,13]
ss_loop_dist_second_comm = ss_loop_mpi.median[:,14]

ss_loop_dist_bar_positions = ticks_x 

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
                            label='Rearrange')
sum+= ss_loop_dist_first_split+ ss_loop_dist_second_split
ss_loop_dist_bar_both_comm = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_first_comm+ ss_loop_dist_second_comm, bar_width-epsilon,
                            bottom=sum,
                            color=colors[9],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='\\',
                            alpha=opacity,
                            label='Communication')

ss_loop_dist_bars = [ss_loop_dist_bar_first_fft, ss_loop_dist_bar_second_fft, ss_loop_dist_bar_first_trans, ss_loop_dist_bar_second_trans, ss_loop_dist_bar_both_split, ss_loop_dist_bar_both_comm]
ss_loop_dist_legend = plt.legend(ss_loop_dist_bars, ['First FFT', 'Second FFT', 'First transpose', 'Second transpose', 'Rearrange', 'Communication'], title='HPX loop dist', bbox_to_anchor=(.5, 1.0), loc="upper left")
plt.gca().add_artist(ss_loop_dist_legend)


# HPX loop lci distributed bars
ss_loop_lci_dist_first_fft = ss_loop_lci.median[:,8]
ss_loop_lci_dist_first_trans = ss_loop_lci.median[:,11]
ss_loop_lci_dist_second_fft = ss_loop_lci.median[:,12]
ss_loop_lci_dist_second_trans = ss_loop_lci.median[:,15]

ss_loop_lci_dist_first_split = ss_loop_lci.median[:,9]
ss_loop_lci_dist_first_comm = ss_loop_lci.median[:,10]
ss_loop_lci_dist_second_split = ss_loop_lci.median[:,13]
ss_loop_lci_dist_second_comm = ss_loop_lci.median[:,14]

ss_loop_lci_dist_bar_positions = ticks_x  + bar_width

ss_loop_lci_dist_bar_first_fft = plt.bar(ss_loop_lci_dist_bar_positions, ss_loop_lci_dist_first_fft, bar_width-epsilon,
                            color=colors[3],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='',
                            alpha=opacity,
                            label='First FFT')
sum = ss_loop_lci_dist_first_fft
ss_loop_lci_dist_bar_second_fft = plt.bar(ss_loop_lci_dist_bar_positions, ss_loop_lci_dist_second_fft, bar_width-epsilon,
                            bottom=sum,
                            color=colors[2],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='',
                            alpha=opacity,
                            label='Second FFT')
sum+= ss_loop_lci_dist_second_fft
ss_loop_lci_dist_bar_first_trans = plt.bar(ss_loop_lci_dist_bar_positions, ss_loop_lci_dist_first_trans , bar_width-epsilon,
                            bottom=sum,
                            color=colors[5],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='',
                            alpha=opacity,
                            label='First transpose')
sum+= ss_loop_lci_dist_first_trans
ss_loop_lci_dist_bar_second_trans = plt.bar(ss_loop_lci_dist_bar_positions, ss_loop_lci_dist_second_trans, bar_width-epsilon,
                            bottom=sum,
                            color=colors[6],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='',
                            alpha=opacity,
                            label='Second transpose')
sum+= ss_loop_lci_dist_second_trans
ss_loop_lci_dist_bar_both_split = plt.bar(ss_loop_lci_dist_bar_positions, ss_loop_lci_dist_first_split+ ss_loop_lci_dist_second_split, bar_width-epsilon,
                            bottom=sum,
                            color=colors[8],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='',
                            alpha=opacity,
                            label='Rearrange')
sum+= ss_loop_lci_dist_first_split+ ss_loop_lci_dist_second_split
ss_loop_lci_dist_bar_both_comm = plt.bar(ss_loop_lci_dist_bar_positions, ss_loop_lci_dist_first_comm+ ss_loop_lci_dist_second_comm, bar_width-epsilon,
                            bottom=sum,
                            color=colors[9],
                            edgecolor='black',
                            linewidth=line_width,
                            hatch='',
                            alpha=opacity,
                            label='Communication')

ss_loop_lci_dist_bars = [ss_loop_lci_dist_bar_first_fft, ss_loop_lci_dist_bar_second_fft, ss_loop_lci_dist_bar_first_trans, ss_loop_lci_dist_bar_second_trans, ss_loop_lci_dist_bar_both_split, ss_loop_lci_dist_bar_both_comm]
ss_loop_lci_dist_legend = plt.legend(ss_loop_lci_dist_bars, ['First FFT', 'Second FFT', 'First transpose', 'Second transpose', 'Rearrange', 'Communication'], title='HPX loop lci dist', bbox_to_anchor=(.8, 1.0), loc="upper left")
plt.gca().add_artist(ss_loop_lci_dist_legend)

# plot parameters
plt.title('Strong Scaling distribution for buran cluster with with 24 threads and $2^{14}$x$2^{14}$ matrix')
plt.xlabel('N nodes')
#plt.xscale("log")
labels_x = ['1','2','4','8','16']
plt.xticks(ticks=ticks_x, labels= labels_x)
#plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_buran_distribution.pdf', bbox_inches='tight')