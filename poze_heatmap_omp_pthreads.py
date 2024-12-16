import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from scipy.interpolate import griddata
import matplotlib.animation as animation

# big test
points = np.array([
    (1, 1), (1, 2), (1, 4), (1, 8), (1, 16), 
    (2, 1), (4, 1), (8, 1), (16, 1), 
    (2, 2), (2, 4), (2, 8), (4, 2), (4, 4), (8, 2)
])
z_values = np.array([
    16.292, 7.916, 4.421, 2.684, 1.940, 
    9.04, 5.68, 5.72, 8.78, 
    8.379, 8.521, 8.256, 5.611, 4.786, 5.517
])

# medium test
points = np.array([
    (1, 1), (1, 2), (1, 4), (1, 8), (1, 16), 
    (2, 1), (4, 1), (8, 1), (16, 1), 
    (2, 2), (2, 4), (2, 8), (4, 2), (4, 4), (8, 2)
])
z_values = np.array([
    7.631, 4.202, 2.398, 1.5, 1.195, 
    6.15, 4.7, 4.9, 5.67, 
    4.222, 3.557, 3.761, 3.055, 2.719, 3.518
])

# small test
# points = np.array([
#     (1, 1), (1, 2), (1, 4), (1, 8), (1, 16), 
#     (2, 1), (4, 1), (8, 1), (16, 1), 
#     (2, 2), (2, 4), (2, 8), (4, 2), (4, 4), (8, 2)
# ])
# z_values = np.array([
#     2.121, 1.106, 0.616, 0.546, 0.447, 
#     1.9, 1.58, 1.83, 2.21, 
#     1.424, 1.516, 1.644, 1.796, 1.369, 1.799
# ])


log_x = np.log2(points[:, 0])
log_y = np.log2(points[:, 1])
log_points = np.column_stack((log_x, log_y))

x_dense = np.linspace(log_x.min(), log_x.max(), 100)
y_dense = np.linspace(log_y.min(), log_y.max(), 100)
x_dense, y_dense = np.meshgrid(x_dense, y_dense)
z_dense = griddata(log_points, z_values, (x_dense, y_dense), method='cubic')

z_dense = np.nan_to_num(z_dense, nan=0.0)

fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

norm = plt.Normalize(z_values.min(), z_values.max())

bar_width = 0.2
bars = []
for (x, y), z in zip(log_points, z_values):
    bar = ax.bar3d(x - bar_width / 2, y - bar_width / 2, 0, bar_width, bar_width, z, color=cm.turbo(norm(z)), alpha=0.9)
    bars.append(bar)
    ax.text(x, y, z + 0.1, f'{z:.2f}', color='black', ha='center', fontsize=8)

x_ticks = np.log2(sorted(set(points[:, 0])))
y_ticks = np.log2(sorted(set(points[:, 1])))
ax.set_xticks(x_ticks)
ax.set_yticks(y_ticks)
ax.set_xticklabels([f'{int(2**tick)}' for tick in x_ticks])
ax.set_yticklabels([f'{int(2**tick)}' for tick in y_ticks])

ax.set_title('Exection Time openmp + Phtreads Medium test (4096x4096)')
ax.set_xlabel('X : MPI')
ax.set_ylabel('Y : Pthreads')
ax.set_zlabel('Z : Execution Time')

sm = cm.ScalarMappable(cmap='turbo', norm=norm)
sm.set_array(z_values)
fig.colorbar(sm, ax=ax, shrink=0.5, aspect=10, label='Z Value')

plt.show()
