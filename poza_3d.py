import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation

# Given three points A, B, C
# small test
# A = np.array([2, 8, 1.746])
# B = np.array([4, 4, 1.310])
# C = np.array([8, 2, 1.580])

# medium test
# A = np.array([2, 8, 4.301])
# B = np.array([4, 4, 2.718])
# C = np.array([8, 2, 3.533])

# big test
A = np.array([2, 8, 6.601])
B = np.array([4, 4, 4.146])
C = np.array([8, 2, 4.496])

# Compute vectors
AB = B - A
AC = C - A

# Compute the normal vector (plane normal)
normal = np.cross(AB, AC)
nx, ny, nz = normal

# Create the mesh for the plane
x_range = np.linspace(min(A[0], B[0], C[0]) - 2, max(A[0], B[0], C[0]) + 2, 10)
y_range = np.linspace(min(A[1], B[1], C[1]) - 2, max(A[1], B[1], C[1]) + 2, 10)
X, Y = np.meshgrid(x_range, y_range)

if nz != 0:
    Z = ( -nx * (X - A[0]) - ny * (Y - A[1]) ) / nz + A[2]
else:
    # If nz = 0, for simplicity, assume a constant Z
    Z = np.full_like(X, A[2])

# Create figure and 3D axes
fig = plt.figure(figsize=(8, 6))
ax = fig.add_subplot(111, projection='3d')

# Plot the points
ax.scatter(A[0], A[1], A[2], color='r', s=50, label='(2, 8)')
ax.scatter(B[0], B[1], B[2], color='g', s=50, label='(4, 4)')
ax.scatter(C[0], C[1], C[2], color='b', s=50, label='(8, 2)')

# Add Z-value text near each point
ax.text(A[0], A[1], A[2], f"Time={A[2]}", color='r', fontsize=10)
ax.text(B[0], B[1], B[2], f"Time={B[2]}", color='g', fontsize=10)
ax.text(C[0], C[1], C[2], f"Time={C[2]}", color='b', fontsize=10)

# Plot the plane
plane_surface = ax.plot_surface(X, Y, Z, alpha=0.5, color='cyan')

ax.set_xlabel('X=threads openmp')
ax.set_ylabel('Y=threads MPI')
ax.set_zlabel('Z=Execution Time')
ax.legend()

# Initial view
ax.view_init(elev=20, azim=0)

# Update function for the animation
def update(frame):
    # Rotate the azimuth angle for each frame
    ax.view_init(elev=20, azim=frame)
    return [plane_surface]

# Create the animation
anim = FuncAnimation(fig, update, frames=360, interval=50, blit=False)

plt.title('Execution Time openmp + MPI Big test (8192x8192)')
plt.show()

# Save as GIF (requires imagemagick)
anim.save('Execution_time_openmp_MPI_mare.gif', writer='imagemagick', fps=20)

