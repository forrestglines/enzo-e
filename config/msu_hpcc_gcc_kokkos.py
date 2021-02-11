import os

print("##############################################################################")
print("MSU's HPCC machine for for GCC (incl. CUDA) builds (last verified 2021-02-11)")
print("Load the following modules:")
print("$ module load -* gcccuda/2020a OpenMPI git Python HDF5 Boost libpng zlib/1.2.9")
print("##############################################################################")

is_arch_valid = 1

flags_arch = '-O3 -Wall -g -ffast-math -funroll-loops'
flags_link = "-L" + os.getenv('EBROOTZLIB') + "/lib"


charm_path = os.environ['CHARM_HOME']
charm_inc = charm_path + "/include"

kokkos_path = os.getenv('KOKKOS_HOME')
kokkos_bin = kokkos_path + "/bin"
kokkos_inc = kokkos_path + "/include"
kokkos_lib = kokkos_path + "/lib64"
flags_arch  += " -I"+kokkos_inc + " -I" + charm_inc
flags_link  += " -L"+kokkos_lib + " -Wl,-rpath=" + kokkos_lib + " -lkokkoscore"

cxx = kokkos_bin + "/nvcc_wrapper"
cc   = 'gcc'
f90  = 'gfortran'

#optional fortran flag
flags_arch_fortran = '-ffixed-line-length-132'

flags_prec_single = ''
flags_prec_double = '-fdefault-real-8 -fdefault-double-8'


libpath_fortran = os.getenv('EBROOTGCCCORE') + '/lib64'
libs_fortran    = ['gfortran']

home = os.environ['HOME']

hdf5_path    = os.environ['EBROOTHDF5']
hdf5_inc = hdf5_path + '/include'
hdf5_lib = hdf5_path + '/lib'

boost_path = os.environ['EBROOTBOOST']
boost_inc = boost_path + '/include'
boost_lib = boost_path + '/lib'


png_path     = os.getenv('EBROOTLIBPNG')
#grackle_path = home + '/local'


