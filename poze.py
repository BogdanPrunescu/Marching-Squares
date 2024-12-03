import matplotlib.pyplot as plt

# Data for the tests
threads = [1, 2, 4, 8, 16]
mic = [2, 1.9, 1.58, 1.83, 2.21]
mediu = [8, 6.15, 4.7, 4.9, 5.67]
mare = [17.6, 9.04, 5.68, 5.72, 8.78]

# Plot all three on a single graph
plt.figure(figsize=(8, 6))
plt.plot(threads, mic, marker='o', label='mic', color='blue')
plt.plot(threads, mediu, marker='o', label='mediu', color='green')
plt.plot(threads, mare, marker='o', label='mare', color='red')

# Add title, labels, grid, and legend
plt.title('Execution Time vs Number of Threads')
plt.xlabel('Number of Threads')
plt.ylabel('Execution Time (s)')
plt.grid(True)
plt.legend()
plt.tight_layout()

# Show the plot
plt.show()

# -- show plot for openmp times --

import matplotlib.pyplot as plt

# Data for the tests
threads = [1, 2, 4, 8, 16]
mic = [2.121, 1.106, 0.616, 0.546, 0.447]
mediu = [7.631, 4.202, 2.398, 1.500, 1.195]
mare = [16.292, 7.915, 4.421, 2.684, 1.940]

# Plot all three on a single graph
plt.figure(figsize=(8, 6))
plt.plot(threads, mic, marker='o', label='mic', color='blue')
plt.plot(threads, mediu, marker='o', label='mediu', color='green')
plt.plot(threads, mare, marker='o', label='mare', color='red')

# Add title, labels, grid, and legend
plt.title('Execution Time vs Number of Threads openmp')
plt.xlabel('Number of Threads')
plt.ylabel('Execution Time (s)')
plt.grid(True)
plt.legend()
plt.tight_layout()

# Show the plot
plt.show()
