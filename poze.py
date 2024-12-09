
import matplotlib.pyplot as plt

# Data for the tests
# threads = [1, 2, 4, 8, 16]
# mic = [2, 1.9, 1.58, 1.83, 2.21]
# mediu = [8, 6.15, 4.7, 4.9, 5.67]
# mare = [17.6, 9.04, 5.68, 5.72, 8.78]

# test mare alg diferiti
# threads = [1, 2, 4, 8, 16]
# pthreads = [16.292, 9.309, 5.320, 3.297, 2.481]
# openmp = [16.292, 7.927, 4.407, 2.676, 2.195]
# mpi = [16.292, 7.915, 4.421, 2.684, 1.940]

# Data for the tests
threads = [1, 2, 4, 8, 16]
pthreads = [16.292, 7.915, 4.421, 2.684, 1.940]
openmp = [16.292, 7.927, 4.407, 2.676, 2.195]
mpi = [16.292, 9.04, 5.68, 5.72, 8.78]

# openmp
# threads = [1, 2, 4, 8, 16]
# mic = [2, 1.125, 0.647, 0.494, 0.396]
# mediu = [8, 4.254, 2.466, 1.554, 1.187]
# mare = [16.292, 7.927, 4.407, 2.676, 2.195]

# mpi
# threads = [1, 2, 4, 8, 16]
# mic = [2, 1.9, 1.58, 1.83, 2.21]
# mediu = [8, 6.15, 4.7, 4.9, 5.67]
# mare = [16.292, 9.04, 5.68, 5.72, 8.78]

#pthreads
# threads = [1, 2, 4, 8, 16]
# mic = [2, 1.106, 0.616, 0.546, 0.447]
# mediu = [8, 4.202, 2.398, 1.500, 1.195]
# mare = [16.292, 7.915, 4.421, 2.684, 1.940]

# Plot all three on a single graph
plt.figure(figsize=(8, 6))
plt.plot(threads, pthreads, marker='o', label='pthreads', color='blue')
plt.plot(threads, openmp, marker='o', label='openmp', color='green')
plt.plot(threads, mpi, marker='o', label='mpi', color='red')

# Add title, labels, grid, and legend
plt.title('8192x8192 test Execution time')
plt.xlabel('Number of Threads')
plt.ylabel('Execution Time (s)')
plt.grid(True)
plt.legend()
plt.tight_layout()

# Show the plot
plt.show()
