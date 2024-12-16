import matplotlib.pyplot as plt

image_sizes = [2048, 4096, 8192]
execution_times = [2.121, 7.631, 16.292]
labels = ['test_mic(2048)', 'test_mediu(4096)', 'test_mare(8192)']

plt.figure(figsize=(10, 6))
plt.plot(image_sizes, execution_times, 'o-', color='blue', label='Execution Time')

for i, label in enumerate(labels):
    plt.annotate(f"{label} {execution_times[i]}s", 
                 (image_sizes[i], execution_times[i]), 
                 textcoords="offset points", 
                 xytext=(10, 10), 
                 ha='center')

plt.title("Execution Time vs. Image Size")
plt.xlabel("Image Size (pixels)")
plt.ylabel("Execution Time (seconds)")
plt.legend()

plt.grid(True)

plt.show()
