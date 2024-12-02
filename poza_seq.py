import matplotlib.pyplot as plt

# Data
image_sizes = [2048, 4096, 8192]  # Image sizes in pixels
execution_times = [2.121, 7.631, 16.292]  # Execution times in seconds
labels = ['test_mic(2048)', 'test_mediu(4096)', 'test_mare(8192)']

# Plot
plt.figure(figsize=(10, 6))
plt.plot(image_sizes, execution_times, 'o-', color='blue', label='Execution Time')

# Add annotations for each point
for i, label in enumerate(labels):
    plt.annotate(f"{label} {execution_times[i]}s", 
                 (image_sizes[i], execution_times[i]), 
                 textcoords="offset points", 
                 xytext=(10, 10), 
                 ha='center')

# Title and labels
plt.title("Execution Time vs. Image Size")
plt.xlabel("Image Size (pixels)")
plt.ylabel("Execution Time (seconds)")
plt.legend()

# Grid
plt.grid(True)

# Show the plot
plt.show()
