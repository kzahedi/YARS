#!/usr/bin/env python3
"""
Plot the 2D trajectory of the YARS robot center of mass.
Shows the actual path the robot takes in the x-y plane.
"""

import matplotlib.pyplot as plt
import numpy as np

# Extract the velocity data from the simulation output
# The output format is: x_velocity y_velocity z_velocity
velocity_data = """0.000 0.000 0.200
-0.000 -0.000 0.160
-0.000 -0.000 0.128
-0.000 -0.000 0.102
-0.000 -0.000 0.082
0.000 0.000 0.066
0.000 0.000 0.052
0.000 0.000 0.042
0.000 0.000 0.034
0.000 0.000 0.027
0.023 0.032 0.021
0.056 0.019 0.017
0.073 0.026 0.014
0.095 0.013 0.011
0.121 0.016 0.009
0.137 0.006 0.007
0.153 0.008 0.006
0.164 0.001 0.005
0.175 0.003 0.004
0.183 -0.001 0.003
0.191 0.002 0.002
0.197 -0.001 0.002
0.202 -0.000 0.001
0.205 -0.002 0.001
0.209 -0.000 0.001
0.212 -0.002 0.001
0.215 -0.001 0.001
0.217 -0.002 0.001
0.219 -0.001 0.000
0.220 -0.001 0.000
0.222 -0.000 0.000
0.223 -0.001 0.000
0.224 -0.000 0.000
0.224 -0.001 0.000
0.225 -0.001 0.000
0.225 -0.000 0.000
0.226 0.000 0.000
0.226 -0.000 0.000
0.226 0.000 0.000
0.226 -0.000 0.000
0.227 -0.000 0.000"""

# Parse the velocity data
lines = velocity_data.strip().split('\n')
velocities = []
for line in lines:
    parts = line.split()
    vx, vy, vz = float(parts[0]), float(parts[1]), float(parts[2])
    velocities.append((vx, vy, vz))

# Convert velocities to position by numerical integration
# Assuming timestep dt = 0.01 seconds (typical for physics simulation)
dt = 0.01
x_pos = [0.0]  # Start at origin
y_pos = [0.0]

for vx, vy, vz in velocities:
    x_pos.append(x_pos[-1] + vx * dt)
    y_pos.append(y_pos[-1] + vy * dt)

# Create time array for color coding
time = np.arange(len(x_pos)) * dt

# Create the 2D trajectory plot
plt.figure(figsize=(14, 10))

# Plot the trajectory with color gradient to show time progression
scatter = plt.scatter(x_pos, y_pos, c=time, cmap='plasma', s=30, alpha=0.8, edgecolors='black', linewidth=0.5)

# Also plot as a line to show continuity
plt.plot(x_pos, y_pos, 'k-', linewidth=1.5, alpha=0.6, label='Trajectory Path')

# Mark start and end points
plt.plot(x_pos[0], y_pos[0], 'go', markersize=12, label='Start Position', markeredgecolor='black', markeredgewidth=2)
plt.plot(x_pos[-1], y_pos[-1], 'ro', markersize=12, label='End Position', markeredgecolor='black', markeredgewidth=2)

# Add direction arrows at key points
n_arrows = 8
arrow_indices = np.linspace(5, len(x_pos)-5, n_arrows, dtype=int)
for i in arrow_indices:
    if i > 5 and i < len(x_pos)-5:
        dx = x_pos[i+3] - x_pos[i-3]
        dy = y_pos[i+3] - y_pos[i-3]
        if dx != 0 or dy != 0:  # Only draw if there's movement
            arrow_length = np.sqrt(dx**2 + dy**2)
            if arrow_length > 1e-6:  # Only draw significant arrows
                plt.arrow(x_pos[i], y_pos[i], dx*0.3, dy*0.3, 
                         head_width=0.0005, head_length=0.0008, 
                         fc='red', ec='red', alpha=0.7)

# Formatting
plt.xlabel('X Position (m)', fontsize=12)
plt.ylabel('Y Position (m)', fontsize=12)
plt.title('YARS Braitenberg Vehicle - 2D Trajectory of Robot Center\n(Modernized C++17 Version)', fontsize=14)
plt.grid(True, alpha=0.3)
plt.legend(fontsize=11)

# Add colorbar for time
cbar = plt.colorbar(scatter)
cbar.set_label('Time (seconds)', fontsize=12)

# Make axes equal for proper aspect ratio
plt.axis('equal')

# Add statistics box
total_distance = sum(np.sqrt((x_pos[i+1]-x_pos[i])**2 + (y_pos[i+1]-y_pos[i])**2) 
                     for i in range(len(x_pos)-1))
final_x, final_y = x_pos[-1], y_pos[-1]
max_x, max_y = max(x_pos), max(y_pos)
min_x, min_y = min(x_pos), min(y_pos)

stats_text = f'''Trajectory Statistics:
Start: (0.000, 0.000) m
End: ({final_x:.4f}, {final_y:.4f}) m
Total Distance: {total_distance:.4f} m
X Range: {max_x-min_x:.4f} m
Y Range: {max_y-min_y:.4f} m
Duration: {time[-1]:.2f} s'''

plt.text(0.02, 0.98, stats_text, transform=plt.gca().transAxes, 
         verticalalignment='top', fontsize=10,
         bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))

plt.tight_layout()
plt.savefig('/Volumes/Eregion/projects/yars/robot_2d_trajectory.png', dpi=300, bbox_inches='tight')
plt.show()

print(f"2D Trajectory Analysis:")
print(f"======================")
print(f"Start position: (0.000, 0.000) m")
print(f"Final position: ({final_x:.6f}, {final_y:.6f}) m")
print(f"Total distance traveled: {total_distance:.6f} m")
print(f"Straight-line distance: {np.sqrt(final_x**2 + final_y**2):.6f} m")
print(f"Path efficiency: {np.sqrt(final_x**2 + final_y**2)/total_distance*100:.1f}%")
print(f"Maximum X reached: {max_x:.6f} m")
print(f"Maximum Y reached: {max_y:.6f} m")
print(f"Y-axis drift range: {max_y-min_y:.6f} m")
print(f"Simulation duration: {time[-1]:.3f} seconds")
print(f"Average speed: {total_distance/time[-1]:.6f} m/s")
print(f"Plot saved as: robot_2d_trajectory.png")