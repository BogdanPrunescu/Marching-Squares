import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from scipy.interpolate import griddata
import matplotlib.animation as animation

# Define the updated 16 points and their z-values big test
# points = np.array([
#     (1, 1), (2, 1), (1, 2), (1, 4), (4, 1), (1, 8), (8, 1), (1, 16), (16, 1),
#     (2, 8), (4, 4), (8, 2), (2, 2), (2, 4), (4, 2)
# ])
# z_values = np.array([
#     16.292, 9.04, 7.927, 4.407, 5.68, 2.676, 5.72, 2.195, 8.78,
#     4.496, 4.146, 6.601, 5.477, 5.634, 4.097
# ])

# Define the updated 16 points and their z-values medium test
# points = np.array([
#     (1, 1), (2, 1), (1, 2), (1, 4), (4, 1), (1, 8), (8, 1), (1, 16), (16, 1),
#     (2, 2), (2, 4), (4, 2), (2, 8), (4, 4), (8, 2)
# ])
# z_values = np.array([
#     7.631, 6.15, 4.254, 2.466, 4.7, 1.554, 4.9, 1.187, 5.67,
#     3.403, 3.497, 2.856, 3.551, 2.148, 2.572
# ])

# Define the updated 16 points and their z-values small test
points = np.array([
    (1, 1), (2, 1), (1, 2), (1, 4), (4, 1), (1, 8), (8, 1), (1, 16), (16, 1),
    (2, 2), (2, 4), (4, 2), (2, 8), (4, 4), (8, 2)
])
z_values = np.array([
    2.121, 1.9, 1.125, 0.647, 1.58, 0.494, 1.83, 0.396, 2.21,
    1.091, 1.152, 1.023, 1.172, 0.827, 0.956
])


# Apply logarithmic scaling to x and y coordinates to uniform bar distances
log_x = np.log2(points[:, 0])
log_y = np.log2(points[:, 1])
log_points = np.column_stack((log_x, log_y))

# Create a dense grid for interpolation
x_dense = np.linspace(log_x.min(), log_x.max(), 100)
y_dense = np.linspace(log_y.min(), log_y.max(), 100)
x_dense, y_dense = np.meshgrid(x_dense, y_dense)
z_dense = griddata(log_points, z_values, (x_dense, y_dense), method='cubic')

# Handle NaN values by setting them to 0
z_dense = np.nan_to_num(z_dense, nan=0.0)

# Create a 3D plot
fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

# Normalize the colors based on the interpolated z values
norm = plt.Normalize(z_values.min(), z_values.max())

# Plot bars for the original points with larger sizes, positioned in the middle
bar_width = 0.2  # Adjusted bar width for uniform spacing on the logarithmic scale
bars = []
for (x, y), z in zip(log_points, z_values):
    bar = ax.bar3d(x - bar_width / 2, y - bar_width / 2, 0, bar_width, bar_width, z, color=cm.turbo(norm(z)), alpha=0.9)
    bars.append(bar)
    ax.text(x, y, z + 0.1, f'{z:.2f}', color='black', ha='center', fontsize=8)

# Set ticks and labels only for the given points, convert back to original scale
x_ticks = np.log2(sorted(set(points[:, 0])))
y_ticks = np.log2(sorted(set(points[:, 1])))
ax.set_xticks(x_ticks)
ax.set_yticks(y_ticks)
ax.set_xticklabels([f'{int(2**tick)}' for tick in x_ticks])
ax.set_yticklabels([f'{int(2**tick)}' for tick in y_ticks])

# Add labels and title
ax.set_title('Exection Time openmp + MPI Small test (2048x2048)')
ax.set_xlabel('X : MPI')
ax.set_ylabel('Y : openMP')
ax.set_zlabel('Z : Execution Time')

# Add a color bar
sm = cm.ScalarMappable(cmap='turbo', norm=norm)
sm.set_array(z_values)
fig.colorbar(sm, ax=ax, shrink=0.5, aspect=10, label='Z Value')

plt.show()
