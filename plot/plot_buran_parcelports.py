import numpy as np
import matplotlib
import matplotlib.pyplot as plt

from plot_general import plot_object
from plot_general import colors_16 as colors
from plot_general import plot_bar
from plot_general import extract_fft_sections

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
ss_fftw_mpi_threads = plot_object('./plot/data/strong_scaling/buran_parcelports/strong_runtimes_fftw_mpi_threads.txt', n_loop)

ss_loop_mpi = plot_object('./plot/data/strong_scaling/buran_parcelports/strong_runtimes_loop_mpi.txt', n_loop)
ss_loop_lci = plot_object('./plot/data/strong_scaling/buran_parcelports/strong_runtimes_loop_lci.txt', n_loop)

ss_loop_shmem = plot_object('./plot/data/strong_scaling/buran_parcelports/strong_runtimes_loop_shmem.txt', 10)
ss_loop_shmem_all = plot_object('./plot/data/strong_scaling/buran_parcelports/strong_runtimes_loop_shmem_all.txt', 10)

################################################################################
# Strong scaling runtime on buran for scatter collective 
plt.figure(figsize=(7,6))
# ideal scaling
points = np.array([1,2,4,8,16])
ideal = 2 * ss_fftw_mpi_threads.median[1,6]/ points
plt.plot(points, ideal, '--', c=colors[0], linewidth=1.5)

# error bars
plt.errorbar(points, ss_fftw_mpi_threads.median[:,6], yerr = ss_fftw_mpi_threads.min_max_error(6), fmt='v-', c=colors[13], linewidth=2, label='FFTW3 reference')
plt.errorbar(points, ss_loop_mpi.median[:,7], yerr = ss_loop_mpi.min_max_error(7), fmt='s-', c=colors[3], linewidth=2, label='MPI scatter')
plt.errorbar(points, ss_loop_lci.median[:,7], yerr = ss_loop_lci.min_max_error(7), fmt='s-', c=colors[2], linewidth=2, label='LCI scatter')
plt.errorbar(points, ss_loop_shmem.median[:,7], yerr = ss_loop_shmem.min_max_error(7), fmt='s-', c=colors[4], linewidth=2, label='SHMEM scatter')
#plt.errorbar(points, ss_loop_shmem_all.median[:,7], yerr = ss_loop_shmem_all.min_max_error(7), fmt='s-', c=colors[5], linewidth=2, label='SHMEM all_to_all')

# plot parameters
#plt.legend(bbox_to_anchor=(0, 0), loc="lower left")
plt.legend(bbox_to_anchor=(1, 0), loc="lower left")
plt.xlabel('N nodes')
plt.xscale("log")
labels_x = ['1','2','4','8','16']
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.yticks(ticks=[0.1, 1.0, 10.0,100.0])
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_buran_parcelport_scatter_runtime.pdf', bbox_inches='tight')

################################################################################
# Strong scaling runtime on buran for all_to_all collective 
plt.figure(figsize=(7,6))

#TODO

plt.xlabel('N nodes')
plt.xscale("log")
labels_x = ['1','2','4','8','16']
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.yticks(ticks=[0.1, 1.0, 10.0,100.0])
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_buran_parcelport_all_to_all_runtime.pdf', bbox_inches='tight')


################################################################################
# Strong scaling runtime on medusa for scatter collective 
plt.figure(figsize=(7,6))

#TODO

plt.xlabel('N nodes')
plt.xscale("log")
labels_x = ['1','2','4','8','16']
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.yticks(ticks=[0.1, 1.0, 10.0,100.0])
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_medusa_parcelport_scatter_runtime.pdf', bbox_inches='tight')

################################################################################
# Strong scaling runtime on buran for all_to_all collective 
plt.figure(figsize=(7,6))

#TODO

plt.xlabel('N nodes')
plt.xscale("log")
labels_x = ['1','2','4','8','16']
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.yticks(ticks=[0.1, 1.0, 10.0,100.0])
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_medusa_parcelport_all_to_all_runtime.pdf', bbox_inches='tight')







################################################################################
################################################################################                                                
# Bar plot buran
plt.figure(figsize=(12,10))   
# plot details
bar_width = 0.25
ticks_x= np.linspace(1,5,5)
section_colors = [colors[3], colors[5], colors[8], colors[9]]
section_labels = ['FFT', 'Transpose', 'Rearrange', 'Communication']

# MPI bar
mpi_sections = extract_fft_sections(ss_loop_mpi.median)
mpi_bar_positions = ticks_x
mpi_hatch = '\\'
# plot bar
mpi_bar = plot_bar(mpi_bar_positions, mpi_sections, section_colors, section_labels, mpi_hatch, bar_width)
# create legend
mpi_legend = plt.legend(mpi_bar, section_labels, title='MPI', bbox_to_anchor=(.5, 1.0), loc="upper left")
plt.gca().add_artist(mpi_legend)

# LCI bar
lci_sections = extract_fft_sections(ss_loop_lci.median)
lci_bar_positions = ticks_x + bar_width
lci_hatch = ''
# plot bar
lci_bar = plot_bar(lci_bar_positions, lci_sections, section_colors, section_labels, lci_hatch, bar_width)
# create legend
lci_legend = plt.legend(lci_bar, section_labels, title='LCI', bbox_to_anchor=(.8, 1.0), loc="upper left")
plt.gca().add_artist(lci_legend)

# SHMEM bar
shmem_sections = extract_fft_sections(ss_loop_shmem.median)
shmem_bar_positions = ticks_x - bar_width
shmem_hatch = '///'
# plot bar
shmem_bar = plot_bar(shmem_bar_positions, shmem_sections, section_colors, section_labels, shmem_hatch, bar_width)
# create legend
shmem_legend = plt.legend(shmem_bar, section_labels, title='LCI', bbox_to_anchor=(.2, 1.0), loc="upper left")
plt.gca().add_artist(shmem_legend)

# plot parameters
plt.title('Strong Scaling distribution for buran cluster with with 24 threads and $2^{14}$x$2^{14}$ matrix')
plt.xlabel('N nodes')
#plt.xscale("log")
labels_x = ['1','2','4','8','16']
plt.xticks(ticks=ticks_x, labels= labels_x)
#plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/strong_scaling_buran_distribution.pdf', bbox_inches='tight')



################################################################################                                                
# Bar plot medusa

#TODO