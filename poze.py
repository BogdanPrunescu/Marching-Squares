import matplotlib.pyplot as plt

# Data for the tests
threads = [1, 2, 4, 8, 16]
mic = [2, 1.03, 0.59, 0.42, 0.35]
mediu = [8, 3.64, 2.12, 1.40, 1.12]
mare = [17.6, 9.3, 5.32, 3.29, 2.48]

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
