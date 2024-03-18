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

# get header
n_loop = 50
################################################################################
# FFTW_ESTIMATE
ss_loop_shared = plot_object('./plot/data/strong_scaling/sven/runtimes_hpx_loop_shared.txt', n_loop)

ss_future_sync_shared = plot_object('./plot/data/strong_scaling/sven/runtimes_hpx_future_sync_shared.txt', 1)
ss_future_naive_shared = plot_object('./plot/data/strong_scaling/sven/runtimes_hpx_future_naive_shared.txt', 1)
ss_future_shared = plot_object('./plot/data/strong_scaling/sven/runtimes_hpx_future_opt_shared.txt', 1)

ss_fftw_threads = plot_object('./plot/data/strong_scaling/sven/runtimes_mpi_threads.txt', 1)
ss_fftw_omp = plot_object('./plot/data/strong_scaling/sven/runtimes_mpi_omp.txt', 1)
#ss_fftw_mpi = plot_object('./plot/data/strong_scaling/sven/runtimes_fftw_mpi.txt', n_loop)
ss_fftw_hpx = plot_object('./plot/data/strong_scaling/sven/runtimes_shared_hpx.txt', n_loop)


##########################################
# FFTW_MEASURE
# ss_loop_shared_measure = plot_object('./plot/data/strong_scaling/16384/strong_runtimes_hpx_loop_shared_measure.txt', n_loop)

# ss_fftw_threads_measure = plot_object('./plot/data/strong_scaling/16384/strong_runtimes_fftw_threads_measure.txt', n_loop)
# ss_fftw_omp_measure = plot_object('./plot/data/strong_scaling/16384/strong_runtimes_fftw_omp_measure.txt', n_loop)
# ss_fftw_mpi_measure = plot_object('./plot/data/strong_scaling/16384/strong_runtimes_fftw_mpi_measure.txt', n_loop)
# ss_fftw_hpx_measure = plot_object('./plot/data/strong_scaling/16384/strong_runtimes_fftw_hpx_measure.txt', n_loop)


################################################################################
# HPX ESTIMATE
plt.figure(figsize=(7,6))


points = np.array([1,2,4,8,16,32,64])

# errorbars
plt.errorbar(points, 
             ss_future_naive_shared.median[:,6], 
             yerr = ss_future_naive_shared.min_max_error(6), 
             fmt='o-',
             c=colors[13], 
             linewidth=2,
             label='HPX future naive')

plt.errorbar(points, 
             ss_future_shared.median[:,6], 
             yerr = ss_future_shared.min_max_error(6), 
             fmt='o-',
             c=colors[12], 
             linewidth=2,
             label='HPX future opt')

plt.errorbar(points, 
             ss_future_sync_shared.median[:,6], 
             yerr = ss_future_sync_shared.min_max_error(6), 
             fmt='o-',
             c=colors[9], 
             linewidth=2,
             label='HPX future sync')

plt.errorbar(points, 
             ss_loop_shared.median[:,7], 
             yerr = ss_loop_shared.min_max_error(7), 
             fmt='s-',
             c=colors[2], 
             linewidth=2,
             label='HPX for_loop')

# plot parameters
#plt.title('Strong Scaling runtime for shared-memory ipvs-epyc2 with $2^{14}$x$2^{14}$ matrix')
plt.legend(bbox_to_anchor=(0, 0), loc="lower left")
#plt.legend(bbox_to_anchor=(1, 0), loc="lower left")
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
#plt.yticks(ticks=[0.01, 0.1, 1.0])
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_sven_hpx_runtime.pdf', bbox_inches='tight')

################################################################################
# FFTW ESTIMATE
plt.figure(figsize=(7,6))

# plt.errorbar(points, 
#              ss_fftw_mpi.median[:,6], 
#              yerr = ss_fftw_mpi.min_max_error(6), 
#              fmt='v--', 
#              c=colors[0], 
#              linewidth=2, 
#              label='FFTW3 with MPI')
plt.errorbar(points, 
             ss_fftw_threads.median[:,6], 
             yerr = ss_fftw_threads.min_max_error(6), 
             fmt='v-',
             c=colors[13], 
             linewidth=2,
             label='FFTW3 with pthreads')
plt.errorbar(points, 
             ss_fftw_omp.median[:,6], 
             yerr = ss_fftw_omp.min_max_error(6), 
             fmt='v-',
             c=colors[12], 
             linewidth=2,
            label='FFTW3 with OpenMP')
plt.errorbar(points, 
             ss_fftw_hpx.median[:,5], 
             yerr = ss_fftw_hpx.min_max_error(5), 
             fmt='v-',
             c=colors[9], 
             linewidth=2,
             label='FFTW3 with HPX')


plt.errorbar(points, 
             ss_loop_shared.median[:,7], 
             yerr = ss_loop_shared.min_max_error(7), 
             fmt='s-',
             c=colors[2], 
             linewidth=2,
             label='HPX for_loop')

# plot parameters
#plt.title('Strong Scaling runtime for shared-memory ipvs-epyc2 with $2^{14}$x$2^{14}$ matrix')
plt.legend(bbox_to_anchor=(0, 1.0), loc="upper left")
plt.xlabel('N cores')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_sven_fftw3_runtime.pdf', bbox_inches='tight')

# ################################################################################
# # FFTW MEASURE
# plt.figure(figsize=(7,6))

# plt.errorbar(points, 
#              ss_fftw_mpi_measure.median[:,6], 
#              yerr = ss_fftw_mpi_measure.min_max_error(6), 
#              fmt='v--', 
#              c=colors[0], 
#              linewidth=2, 
#              label='FFTW3 with MPI')

# plt.errorbar(points, 
#              ss_fftw_hpx_measure.median[:,5], 
#              yerr = ss_fftw_hpx_measure.min_max_error(5), 
#              fmt='v-',
#              c=colors[9], 
#              linewidth=2,
#              label='FFTW3 with HPX')

# plt.errorbar(points, 
#              ss_fftw_threads_measure.median[:,6], 
#              yerr = ss_fftw_threads_measure.min_max_error(6), 
#              fmt='v-',
#              c=colors[13], 
#              linewidth=2,
#              label='FFTW3 with pthreads')

# plt.errorbar(points, 
#              ss_fftw_omp_measure.median[:,6], 
#              yerr = ss_fftw_omp_measure.min_max_error(6), 
#              fmt='v-',
#              c=colors[12], 
#              linewidth=2,
#              label='FFTW3 with OpenMP')


# plt.errorbar(points, 
#              ss_loop_shared_measure.median[:,7], 
#              yerr = ss_loop_shared_measure.min_max_error(7), 
#              fmt='s-',
#              c=colors[2], 
#              linewidth=2,
#              label='HPX for_loop')


# # plot parameters
# #plt.title('Strong Scaling runtime for shared-memory ipvs-epyc2 with $2^{14}$x$2^{14}$ matrix')
# #plt.legend(bbox_to_anchor=(1, 0), loc="lower left")
# plt.legend(bbox_to_anchor=(1.0, 1.0), loc="upper right")
# plt.xlabel('N cores')
# plt.xscale("log")
# labels_x = points.astype(int).astype(str)
# plt.xticks(ticks=points, labels= labels_x)
# plt.yscale("log")
# plt.yticks(ticks=[0.1, 1.0, 10.0])
# plt.ylabel('Runtime in s')
# plt.savefig('plot/figures/strong_scaling_16384_fftw3_measure_runtime.pdf', bbox_inches='tight')

# ################################################################################
# # FFTW MEASURE planning time
# plt.figure(figsize=(7,6))

# plt.errorbar(points, 
#              ss_fftw_mpi_measure.median[:,5], 
#              yerr = ss_fftw_mpi_measure.min_max_error(5), 
#              fmt='v--', 
#              c=colors[0], 
#              linewidth=2, 
#              label='FFTW3 with MPI')

# plt.errorbar(points, 
#              ss_fftw_hpx_measure.median[:,4], 
#              yerr = ss_fftw_hpx_measure.min_max_error(4), 
#              fmt='v-',
#              c=colors[9], 
#              linewidth=2,
#              label='FFTW3 with HPX')

# plt.errorbar(points, 
#              ss_fftw_threads_measure.median[:,5], 
#              yerr = ss_fftw_threads_measure.min_max_error(5), 
#              fmt='v-',
#              c=colors[13], 
#              linewidth=2,
#              label='FFTW3 with pthreads')

# plt.errorbar(points, 
#              ss_fftw_omp_measure.median[:,5], 
#              yerr = ss_fftw_omp_measure.min_max_error(5), 
#              fmt='v-',
#              c=colors[12], 
#              linewidth=2,
#              label='FFTW3 with OpenMP')


# plt.errorbar(points, 
#              ss_loop_shared_measure.median[:,-3], 
#              yerr = ss_loop_shared_measure.min_max_error(-3), 
#              fmt='s-',
#              c=colors[2], 
#              linewidth=2,
#              label='HPX for_loop')

# # plot parameters
# #plt.title('Strong Scaling runtime for shared-memory ipvs-epyc2 with $2^{14}$x$2^{14}$ matrix')
# plt.legend(bbox_to_anchor=(0, 0), loc="lower left")
# #plt.legend(bbox_to_anchor=(1, 0), loc="lower left")
# plt.xlabel('N cores')
# plt.xscale("log")
# labels_x = points.astype(int).astype(str)
# plt.xticks(ticks=points, labels= labels_x)
# plt.yscale("log")
# plt.ylabel('Runtime in s')
# plt.savefig('plot/figures/strong_scaling_16384_fftw3_measure_plantime.pdf', bbox_inches='tight')


# ################################################################################                                                
# # Bar plot added up
# plt.figure(figsize=(7,6))
    
# # plot details
# bar_width = 0.25
# epsilon = .05
# line_width= 0.5
# opacity = 1.0
# ticks_x= np.linspace(1,8,8)

# plt.rc('hatch', color='k', linewidth=0.5)
# # HPX loop bars
# ss_loop_fft = ss_loop_shared.median[:,8] + ss_loop_shared.median[:,10]
# ss_loop_trans = ss_loop_shared.median[:,9] + ss_loop_shared.median[:,11]

# ss_loop_bar_positions = ticks_x

# ss_loop_bar_fft = plt.bar(ss_loop_bar_positions, ss_loop_fft, bar_width-epsilon,
#                             color=colors[2],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='')
# ss_loop_bar_trans = plt.bar(ss_loop_bar_positions, ss_loop_trans , bar_width-epsilon,
#                             bottom=ss_loop_fft,
#                             color=colors[6],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='')

# ss_loop_bars = [ss_loop_bar_fft, ss_loop_bar_trans]
# ss_loop_legend = plt.legend(ss_loop_bars, ['FFT', 'Transpose'], title='HPX for_loop', bbox_to_anchor=(1.0, .628), loc="upper right")
# plt.gca().add_artist(ss_loop_legend)

# # HPX future async bars
# ss_future_sync_fft = ss_future_sync_shared.median[:,7] + ss_future_sync_shared.median[:,9]
# ss_future_sync_trans = ss_future_sync_shared.median[:,8] + ss_future_sync_shared.median[:,10]

# ss_future_sync_bar_positions = ss_loop_bar_positions - bar_width

# ss_future_sync_bar_fft = plt.bar(ss_future_sync_bar_positions, ss_future_sync_fft, bar_width-epsilon,
#                             color=colors[2],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='\\')

# ss_future_sync_bar_trans = plt.bar(ss_future_sync_bar_positions, ss_future_sync_trans , bar_width-epsilon,
#                             bottom=ss_future_sync_fft,
#                             color=colors[6],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='\\')

# ss_future_sync_bars = [ss_future_sync_bar_fft, ss_future_sync_bar_trans]
# ss_future_sync_legend = plt.legend(ss_future_sync_bars, ['FFT', 'Transpose'], title='HPX future sync', bbox_to_anchor=(1.0, .39), loc="upper right")
# plt.gca().add_artist(ss_future_sync_legend)

# # HPX loop distributed bars
# ss_loop_dist_fft = ss_loop_scatter.median[:,8] + ss_loop_scatter.median[:,12]
# ss_loop_dist_trans = ss_loop_scatter.median[:,11] + ss_loop_scatter.median[:,15]

# ss_loop_dist_split = ss_loop_scatter.median[:,9] + ss_loop_scatter.median[:,13]
# ss_loop_dist_comm = ss_loop_scatter.median[:,10] + ss_loop_scatter.median[:,14]

# ss_loop_dist_bar_positions = ss_loop_bar_positions + bar_width

# ss_loop_dist_bar_fft = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_fft, bar_width-epsilon,
#                             color=colors[2],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='//')
# sum = ss_loop_dist_fft
# ss_loop_dist_bar_trans = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_trans , bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[6],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='//')
# sum+= ss_loop_dist_trans
# ss_loop_dist_bar_split = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_split, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[8],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='//',
#                             alpha=opacity,
#                             label='Rearrange')
# sum+= ss_loop_dist_split
# ss_loop_dist_bar_comm = plt.bar(ss_loop_dist_bar_positions, ss_loop_dist_comm, bar_width-epsilon,
#                             bottom=sum,
#                             color=colors[9],
#                             edgecolor='black',
#                             linewidth=line_width,
#                             hatch='//',
#                             alpha=opacity,
#                             label='Communication (very small)')

# ss_loop_dist_bars = [ss_loop_dist_bar_fft, ss_loop_dist_bar_trans, ss_loop_dist_bar_split, ss_loop_dist_bar_comm]
# ss_loop_dist_legend = plt.legend(ss_loop_dist_bars, ['FFT', 'Transpose','Rearrange', 'Communicate'], title='HPX for_loop dist', bbox_to_anchor=(1.0, 1.0), loc="upper right")
# plt.gca().add_artist(ss_loop_dist_legend)

# # plot parameters
# #plt.title('Strong Scaling distribution for shared-memory ipvs-epyc2 with $2^{14}$x$2^{14}$ matrix')
# plt.xlabel('N cores')
# #plt.xscale("log")
# labels_x = points.astype(int).astype(str)
# plt.xticks(ticks=ticks_x, labels= labels_x)
# #plt.yscale("log")
# plt.ylabel('Runtime in s')
# plt.savefig('plot/figures/strong_scaling_16384_distribution.pdf', bbox_inches='tight')
