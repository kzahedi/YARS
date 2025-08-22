#!/usr/bin/env python3
"""
Plot the trajectory of the YARS robot from simulation output.
Extracts x,y coordinates from the velocity output and plots the path.
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

# Create the trajectory plot
plt.figure(figsize=(12, 8))
plt.plot(x_pos, y_pos, 'b-', linewidth=2, label='Robot Trajectory')
plt.plot(x_pos[0], y_pos[0], 'go', markersize=10, label='Start Position')
plt.plot(x_pos[-1], y_pos[-1], 'ro', markersize=10, label='End Position')

# Add arrows to show direction
n_arrows = 5
arrow_indices = np.linspace(10, len(x_pos)-10, n_arrows, dtype=int)
for i in arrow_indices:
    dx = x_pos[i+5] - x_pos[i-5]
    dy = y_pos[i+5] - y_pos[i-5]
    plt.arrow(x_pos[i], y_pos[i], dx*0.1, dy*0.1, 
              head_width=0.0001, head_length=0.0001, fc='red', ec='red')

plt.xlabel('X Position (m)')
plt.ylabel('Y Position (m)')
plt.title('YARS Braitenberg Vehicle Trajectory\n(Modernized C++17 Version)')
plt.grid(True, alpha=0.3)
plt.legend()
plt.axis('equal')

# Add some statistics
total_distance = sum(np.sqrt((x_pos[i+1]-x_pos[i])**2 + (y_pos[i+1]-y_pos[i])**2) 
                     for i in range(len(x_pos)-1))
final_x, final_y = x_pos[-1], y_pos[-1]

plt.text(0.02, 0.98, f'Final Position: ({final_x:.4f}, {final_y:.4f}) m\nTotal Distance: {total_distance:.4f} m', 
         transform=plt.gca().transAxes, verticalalignment='top',
         bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))

plt.tight_layout()
plt.savefig('/Volumes/Eregion/projects/yars/robot_trajectory.png', dpi=300, bbox_inches='tight')
plt.show()

print(f"Trajectory plotted successfully!")
print(f"Final position: ({final_x:.6f}, {final_y:.6f}) m")
print(f"Total distance traveled: {total_distance:.6f} m")
print(f"Plot saved as: robot_trajectory.png")