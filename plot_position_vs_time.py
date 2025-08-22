#!/usr/bin/env python3
"""
Plot the x,y position of the YARS robot over time from simulation output.
Shows how the robot's coordinates change during the simulation.
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

# Create time array
time = np.arange(len(x_pos)) * dt

# Create the position vs time plots
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 10))

# X position vs time
ax1.plot(time, x_pos, 'b-', linewidth=2, label='X Position')
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('X Position (m)')
ax1.set_title('YARS Robot X Position Over Time\n(Modernized C++17 Version)')
ax1.grid(True, alpha=0.3)
ax1.legend()

# Y position vs time
ax2.plot(time, y_pos, 'r-', linewidth=2, label='Y Position')
ax2.set_xlabel('Time (s)')
ax2.set_ylabel('Y Position (m)')
ax2.set_title('YARS Robot Y Position Over Time')
ax2.grid(True, alpha=0.3)
ax2.legend()

# Combined X,Y positions vs time
ax3.plot(time, x_pos, 'b-', linewidth=2, label='X Position')
ax3.plot(time, y_pos, 'r-', linewidth=2, label='Y Position')
ax3.set_xlabel('Time (s)')
ax3.set_ylabel('Position (m)')
ax3.set_title('YARS Robot X,Y Positions Over Time (Combined)')
ax3.grid(True, alpha=0.3)
ax3.legend()

plt.tight_layout()
plt.savefig('/Volumes/Eregion/projects/yars/robot_position_vs_time.png', dpi=300, bbox_inches='tight')
plt.show()

# Print some analysis
print(f"Position vs Time Analysis:")
print(f"=========================")
print(f"Simulation duration: {time[-1]:.3f} seconds")
print(f"Final X position: {x_pos[-1]:.6f} m")
print(f"Final Y position: {y_pos[-1]:.6f} m")
print(f"Maximum X position: {max(x_pos):.6f} m")
print(f"Maximum Y position: {max(y_pos):.6f} m")
print(f"Minimum Y position: {min(y_pos):.6f} m")
print(f"Y position range: {max(y_pos) - min(y_pos):.6f} m")

# Calculate when robot reaches 90% of final X position
final_x = x_pos[-1]
target_x = 0.9 * final_x
settling_time = None
for i, x in enumerate(x_pos):
    if x >= target_x:
        settling_time = time[i]
        break

if settling_time:
    print(f"Time to reach 90% of final X position: {settling_time:.3f} seconds")

print(f"Plot saved as: robot_position_vs_time.png")