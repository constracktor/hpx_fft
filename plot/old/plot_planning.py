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
################################################################################
# READ PLAN FILES
# get header and loop number for averaging
plan_header = np.genfromtxt(os.path.abspath('./plot/data/plan/plan_times_estimate.txt'), dtype='unicode', delimiter=';' , max_rows=1)

# read estimate file
plan_estimate_matrix = np.genfromtxt(os.path.abspath('./plot/data/plan/plan_times_estimate.txt'), dtype='float', delimiter=';' , skip_header=1) / 1e3
# read patient_file
plan_patient_matrix = np.genfromtxt(os.path.abspath('./plot/data/plan/plan_times_patient.txt'), dtype='float', delimiter=';' , skip_header=1) / 1e3

################################################################################
# READ RUNTIME FILES
# get header and loop number for averaging
runtime_header = np.genfromtxt(os.path.abspath('./plot/data/plan/runtimes_estimate.txt'), dtype='unicode', delimiter=';' , max_rows=1)
#n_loop = int(runtime_header[2])
#print(n_loop)

# read estimate file
runtime_estimate_matrix = np.genfromtxt(os.path.abspath('./plot/data/plan/runtimes_estimate.txt'), dtype='float', delimiter=';' , skip_header=1) / 1e3
# read patient file
runtime_patient_matrix = np.genfromtxt(os.path.abspath('./plot/data/plan/runtimes_patient.txt'), dtype='float', delimiter=';' , skip_header=1) / 1e3

points = np.logspace(1,
               runtime_estimate_matrix.shape[0],
               num = runtime_estimate_matrix.shape[0],
               endpoint = True,
               base = 2,
               dtype = int)

            
scale = np.logspace(-5,
               7,
               num = 7,
               endpoint = True,
               base = 10,
               dtype = float)

print(scale)



################################################################################
# TOTAL RUNTIME ESTIMATE
#points = runtime_estimate_matrix[:,0]
total_fftw = runtime_estimate_matrix[:,3]
total_stride = runtime_estimate_matrix[:,4]
total_trans = runtime_estimate_matrix[:,7]
total_loop = runtime_estimate_matrix[:,11]

plt.figure(figsize=(10,5))
plt.plot(points, total_fftw, 'o-', c=colors[0], linewidth=1, label='E: FFTW 2D')
plt.plot(points, total_stride, 'o-', c=colors[4], linewidth=1, label='E: Strided')
plt.plot(points, total_trans, 'o-', c=colors[2], linewidth=1, label='E: Transpose')
plt.plot(points, total_loop, 'o-', c=colors[1], linewidth=1, label='E: Loop')

total_fftw = runtime_patient_matrix[:,3]
total_stride = runtime_patient_matrix[:,4]
total_trans = runtime_patient_matrix[:,7]
total_loop = runtime_patient_matrix[:,11]

plt.plot(points, total_fftw, 's--', c=colors[0], linewidth=1, label='P: FFTW 2D')
plt.plot(points, total_stride, 's--', c=colors[4], linewidth=1, label='P: Strided')
plt.plot(points, total_trans, 's--', c=colors[2], linewidth=1, label='P: Transpose')
plt.plot(points, total_loop, 's--', c=colors[1], linewidth=1, label='P: Loop')

plt.title('Total runtime estimated vs patient planning')
plt.legend(loc='upper left', ncol=2)
plt.xlabel('N with N x N Matrix')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.yticks(ticks=scale)
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/total_runtime_estimate.pdf', format='pdf', bbox_inches='tight')

################################################################################
# RUNTIME STRIDE VS TRANS ESTIMATE
#points = runtime_estimate_matrix[:,0]
total_stride = runtime_estimate_matrix[:,4]
r2c_stride = runtime_estimate_matrix[:,5]
c2c_stride = runtime_estimate_matrix[:,6]

total_trans = runtime_estimate_matrix[:,7]
r2c_trans = runtime_estimate_matrix[:,8]
local_trans = runtime_estimate_matrix[:,9]
c2c_trans = runtime_estimate_matrix[:,10]

plt.figure(figsize=(10,5))
plt.plot(points, total_stride, 'o-', c=colors[0], linewidth=1, label='Stride: Total')
plt.plot(points, total_trans, 's--', c=colors[0], linewidth=1, label='Trans: Total')

plt.plot(points, r2c_stride, 'o-', c=colors[4], linewidth=1, label='Stride: r2c')
plt.plot(points, r2c_trans, 's--', c=colors[4], linewidth=1, label='Trans: r2c')

plt.plot(points, c2c_stride, 'o-', c=colors[2], linewidth=1, label='Stride: strided c2c')
plt.plot(points, c2c_trans, 's--', c=colors[2], linewidth=1, label='Trans: c2c')

plt.plot(points, local_trans, 's--', c=colors[1], linewidth=1, label='Trans: local')

plt.title('Runtime comparison strided vs transpose for estimated planning')
plt.legend(loc='upper left')
plt.xlabel('N with N x N Matrix')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Runtime in s')
plt.savefig('plot/figures/stride_trans_runtime_estimate.pdf', format='pdf', bbox_inches='tight')

################################################################################
# TOTAL RUNTIME PATIENT
#points = runtime_patient_matrix[:,0]
total_fftw = runtime_patient_matrix[:,3]
total_stride = runtime_patient_matrix[:,4]
total_trans = runtime_patient_matrix[:,7]
total_loop = runtime_patient_matrix[:,11]

plt.figure(figsize=(10,5))
plt.plot(points, total_fftw, 'o-', c=colors[0], linewidth=1, label='FFTW 2D')
plt.plot(points, total_stride, 'o-', c=colors[4], linewidth=1, label='Strided')
plt.plot(points, total_trans, 'o-', c=colors[2], linewidth=1, label='Transpose')
plt.plot(points, total_loop, 'o-', c=colors[1], linewidth=1, label='Loop')
plt.title('Total runtime for patient planning')
plt.legend(loc='upper left')
plt.xlabel('N with N x N Matrix')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Time in s')
plt.savefig('plot/figures/total_runtime_patient.pdf', format='pdf', bbox_inches='tight')

################################################################################
# RUNTIME STRIDE VS TRANSPOSE PATIENT
#points = runtime_patient_matrix[:,0]
total_stride = runtime_patient_matrix[:,4]
r2c_stride = runtime_patient_matrix[:,5]
c2c_stride = runtime_patient_matrix[:,6]

total_trans = runtime_patient_matrix[:,7]
r2c_trans = runtime_patient_matrix[:,8]
local_trans = runtime_patient_matrix[:,9]
c2c_trans = runtime_patient_matrix[:,10]

plt.figure(figsize=(10,5))
plt.plot(points, total_stride, 'o-', c=colors[0], linewidth=1, label='Stride: Total')
plt.plot(points, total_trans, 's--', c=colors[0], linewidth=1, label='Trans: Total')

plt.plot(points, r2c_stride, 'o-', c=colors[4], linewidth=1, label='Stride: r2c')
plt.plot(points, r2c_trans, 's--', c=colors[4], linewidth=1, label='Trans: r2c')

plt.plot(points, c2c_stride, 'o-', c=colors[2], linewidth=1, label='Stride: strided c2c')
plt.plot(points, c2c_trans, 's--', c=colors[2], linewidth=1, label='Trans: c2c')

plt.plot(points, local_trans, 's--', c=colors[1], linewidth=1, label='Trans: local')

plt.title('Runtime comparison strided vs transpose for patient planning')
plt.legend(loc='upper left')
plt.xlabel('N with N x N Matrix')
plt.xscale("log")
labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.ylabel('Time in s')
plt.savefig('plot/figures/stride_trans_runtime_patient.pdf', format='pdf', bbox_inches='tight')

################################################################################
# PLAN TIME ESTIMATE
#points = plan_estimate_matrix[:,0]
plan_fftw = plan_estimate_matrix[:,3]
plan_stride = plan_estimate_matrix[:,4] + plan_estimate_matrix[:,5]
plan_trans = plan_estimate_matrix[:,6] + plan_estimate_matrix[:,7]
plan_loop = plan_estimate_matrix[:,8] + plan_estimate_matrix[:,9]

plt.figure(figsize=(10,5))
plt.plot(points, plan_fftw, 'o-', c=colors[0], linewidth=1, label='E: FFTW 2D')
plt.plot(points, plan_stride, 'o-', c=colors[4], linewidth=1, label='E: Strided')
plt.plot(points, plan_trans, 'o-', c=colors[2], linewidth=1, label='E: Transpose')
plt.plot(points, plan_loop, 'o-', c=colors[1], linewidth=1, label='E: Loop')
# PLAN TIME PATIENT
#points = plan_patient_matrix[:,0]
plan_fftw = plan_patient_matrix[:,3]
plan_stride = plan_patient_matrix[:,4] + plan_patient_matrix[:,5]
plan_trans = plan_patient_matrix[:,6] + plan_patient_matrix[:,7]
plan_loop = plan_patient_matrix[:,8] + plan_patient_matrix[:,9]

plt.plot(points, plan_fftw, 's--', c=colors[0], linewidth=1, label='P: FFTW 2D')
plt.plot(points, plan_stride, 's--', c=colors[4], linewidth=1, label='P: Strided')
plt.plot(points, plan_trans, 's--', c=colors[2], linewidth=1, label='P: Transpose')
plt.plot(points, plan_loop, 's--', c=colors[1], linewidth=1, label='P: Loop')
plt.title('Planning time estimated vs patient')
plt.legend(loc='upper left', ncol=2)
plt.xlabel('N with N x N Matrix')
plt.xscale("log")
#labels_x = points.astype(int).astype(str)
plt.xticks(ticks=points, labels= labels_x)
plt.yscale("log")
plt.yticks(ticks=scale)
plt.ylabel('Time in s')
plt.savefig('plot/figures/plan_time.pdf', format='pdf', bbox_inches='tight')


# ################################################################################
# # HPX TILE SCALING
# # plot HPX Runtime distribution tiles_right
# points = hpx_tiles_right_128_averaged[:,0]
# plt.figure(figsize=(10,5))
# plt.plot(points, hpx_tiles_right_16_averaged[:,4], 'o-', c=colors[1], linewidth=1, label='System 1, 16 Cores')
# plt.plot(points, hpx_tiles_right_128_averaged[:,4], 'o-', c=colors[0], linewidth=1, label='System 1, 128 Cores')
# plt.plot(points, hpx_tiles_right_18_cpu_averaged[:,4], 's--', c=colors[2], linewidth=1, label='System 2, 18 Cores')
# plt.plot(points, hpx_tiles_right_18_gpu_averaged[:,4], 's--', c=colors[4], linewidth=1, label='System 2, 18 Cores + GPU')
# #plt.title('Tile scaling HPX implementation for different tile sizes')
# plt.legend(loc='lower left')
# plt.xlabel('Tile size and tiles per dimension ')
# plt.xscale("log")
# labels_x = (20000 / points).astype(int).astype(str)
# for i in range(0,labels_x.size):
#     labels_x[i] = labels_x[i] + "\n T = " + points[i].astype(int).astype(str)
# plt.xticks(ticks=points, labels=labels_x)
# plt.yscale("log")
# plt.ylabel('Runtime in s')
# plt.savefig('figures/tiles_right_scaling_hpx.pdf', bbox_inches='tight')

# ################################################################################
# # CORES RUNTIME DISTRIBUTION
# # PETSc data
# points = petsc_cores_averaged[:,0]
# prediction = petsc_cores_averaged[:,7]
# solve = petsc_cores_averaged[:,6]
# assembly = petsc_cores_averaged[:,5]
# plt.figure(figsize=(10,5))
# plt.plot(points, petsc_cores_averaged[:,4], 's--', c=colors[0], linewidth=1, label='PETSc Total')
# plt.plot(points, solve, 's--', c=colors[4], linewidth=1, label='PETSc Cholesky')
# plt.plot(points, assembly, 's--', c=colors[2], linewidth=1, label='PETSc Assembly')
# plt.plot(points, prediction, 's--', c=colors[1], linewidth=1, label='PETSc Prediction')
# # HPX data
# points = hpx_cores_averaged[:,0]
# prediction = hpx_cores_averaged[:,9]
# solve = hpx_cores_averaged[:,8]
# choleksy = hpx_cores_averaged[:,7] + solve
# assembly = hpx_cores_averaged[:,6]
# total = hpx_cores_averaged[:,5]
# plt.plot(points, total, 'o-', c=colors[0], linewidth=1, label='HPX Total')
# plt.plot(points, choleksy, 'o-', c=colors[4], linewidth=1, label='HPX Cholesky')
# plt.plot(points, assembly, 'o-', c=colors[2], linewidth=1, label='HPX Assembly')
# plt.plot(points, prediction, 'o-', c=colors[1], linewidth=1, label='HPX Prediction')
# #plt.title('Runtime distribution of HPX and PETSc for different number of cores')
# plt.legend(loc='lower left')
# plt.xlabel('N cores')
# plt.xscale("log")
# labels_x = points.astype(int).astype(str)
# plt.xticks(ticks=points, labels= labels_x)
# plt.yscale("log")
# plt.ylabel('Runtime in s')
# plt.savefig('figures/cores_distribution.pdf', format='pdf', bbox_inches='tight')

# ################################################################################
# # CORES PARALLEL EFFICIENCY
# # plot PETSc and HPX data scaling parallel efficiency
# plt.figure(figsize=(10,3))
# # line
# plt.plot(points, 100 *np.ones(points.size), 'k:', linewidth=1)
# # PETSc data
# points = petsc_cores_averaged[:,0]
# parallel_efficieny = 100 * petsc_cores_averaged[0,4] / (petsc_cores_averaged[:,4] * petsc_cores_averaged[:,0])
# plt.plot(points, parallel_efficieny, 's--', c=greyscale[0], linewidth=1, label='PETSc FP64')
# # HPX data
# points = hpx_cores_averaged[:,0]
# parallel_efficieny = 100 * hpx_cores_averaged[0,5] / (hpx_cores_averaged[:,5] * hpx_cores_averaged[:,0])
# plt.plot(points, parallel_efficieny, 'o-', c=greyscale[2], linewidth=1, label='HPX FP64')
# points = hpx_cores_sp_averaged[:,0]
# parallel_efficieny = 100 * hpx_cores_sp_averaged[0,5] / (hpx_cores_sp_averaged[:,5] * hpx_cores_sp_averaged[:,0])
# plt.plot(points, parallel_efficieny, 'o-', c=greyscale[4], linewidth=1, label='HPX FP32')
# #plt.title('Parallel efficiency of HPX and PETSc for different number of cores')
# plt.legend(loc='lower left')
# plt.xlabel('N cores')
# plt.xscale("log")
# labels_x = points.astype(int).astype(str)
# plt.xticks(ticks=points, labels= labels_x)
# plt.ylabel('Parallel efficiency in %')
# ticks_y = np.linspace(50, 100, num=6, endpoint=True, dtype=int)
# plt.yticks(ticks=ticks_y)
# plt.savefig('figures/cores_efficiency.pdf', bbox_inches='tight')
# print(hpx_cores_sp_averaged[:,5] / hpx_cores_averaged[:,5])
# ################################################################################
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

# ################################################################################
# # BLAS COMPARISON
# # plot PETSc and HPX blas scaling
# plt.figure(figsize=(10,5))
# plt.plot(petsc_blas_matrix[:,0], petsc_blas_matrix[:,1], 's--', c=colors[4], linewidth=1, label='POTRF PETSc with fblaslapack')
# plt.plot(petsc_blas_matrix[:,0], petsc_blas_matrix[:,2], 's--', c=colors[2], linewidth=1, label='TRSM PETSc with fblaslapack')
# plt.plot(petsc_blas_matrix[:,0], petsc_blas_matrix[:,3], 's--', c=colors[1], linewidth=1, label='GEMM PETSc with fblaslapack')
# plt.plot(hpx_blas_matrix[:,0], hpx_blas_matrix[:,1], 'o-', c=colors[4], linewidth=1, label='POTRF uBLAS')
# plt.plot(hpx_blas_matrix[:,0], hpx_blas_matrix[:,2], 'o-', c=colors[2], linewidth=1, label='TRSM uBLAS')
# plt.plot(hpx_blas_matrix[:,0], hpx_blas_matrix[:,3], 'o-', c=colors[1], linewidth=1, label='GEMM uBLAS')
# #plt.title('Comparison of uBLAS and PETSc with fblaslapack for different training set sizes')
# plt.legend()
# plt.xlabel('N matrix dimension')
# plt.xticks(petsc_blas_matrix[:,0])
# plt.ylabel('Time in s')
# plt.xscale("log")
# plt.yscale("log")
# plt.savefig('figures/blas_hpx_petsc_comparison.pdf', bbox_inches='tight')

print('All figures generated.')