#!/usr/bin/env python3
"""
Plot the 2D trajectory of the YARS robot using actual CSV data.
Shows the path the robot takes in the x-y plane using real simulation output.
"""

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Read the CSV file
csv_file = '/Volumes/Eregion/projects/yars/braitenberg-2025-08-23-00-37-19.csv'
df = pd.read_csv(csv_file, comment='#')

# Extract main body x,y coordinates (first two columns)
x_pos = df.iloc[:, 0].values  # main body x
y_pos = df.iloc[:, 1].values  # main body y

# Create time array (assuming each row is one simulation step)
time_steps = np.arange(len(x_pos))
# Assuming physics timestep of 0.01 seconds
dt = 0.01
time = time_steps * dt

# Create the 2D trajectory plot
plt.figure(figsize=(14, 10))

# Plot the trajectory with color gradient to show time progression
scatter = plt.scatter(x_pos, y_pos, c=time, cmap='viridis', s=25, alpha=0.8, edgecolors='black', linewidth=0.3)

# Also plot as a line to show continuity
plt.plot(x_pos, y_pos, 'k-', linewidth=1.5, alpha=0.6, label='Trajectory Path')

# Mark start and end points
plt.plot(x_pos[0], y_pos[0], 'go', markersize=12, label='Start Position', markeredgecolor='black', markeredgewidth=2)
plt.plot(x_pos[-1], y_pos[-1], 'ro', markersize=12, label='End Position', markeredgecolor='black', markeredgewidth=2)

# Add direction arrows at key points
n_arrows = 10
if len(x_pos) > 20:
    arrow_indices = np.linspace(10, len(x_pos)-10, n_arrows, dtype=int)
    for i in arrow_indices:
        if i > 5 and i < len(x_pos)-5:
            dx = x_pos[i+5] - x_pos[i-5]
            dy = y_pos[i+5] - y_pos[i-5]
            if abs(dx) > 1e-6 or abs(dy) > 1e-6:  # Only draw if there's movement
                arrow_length = np.sqrt(dx**2 + dy**2)
                if arrow_length > 1e-4:  # Only draw significant arrows
                    scale = 0.2
                    plt.arrow(x_pos[i], y_pos[i], dx*scale, dy*scale, 
                             head_width=0.002, head_length=0.003, 
                             fc='red', ec='red', alpha=0.7, linewidth=1.5)

# Formatting
plt.xlabel('X Position (m)', fontsize=14)
plt.ylabel('Y Position (m)', fontsize=14)
plt.title('YARS Braitenberg Vehicle - Robot Center Trajectory\n(Real CSV Data from Modernized C++17 Version)', fontsize=16)
plt.grid(True, alpha=0.3)
plt.legend(fontsize=12)

# Add colorbar for time
cbar = plt.colorbar(scatter)
cbar.set_label('Time (seconds)', fontsize=12)

# Calculate statistics
total_distance = sum(np.sqrt((x_pos[i+1]-x_pos[i])**2 + (y_pos[i+1]-y_pos[i])**2) 
                     for i in range(len(x_pos)-1))
final_x, final_y = x_pos[-1], y_pos[-1]
start_x, start_y = x_pos[0], y_pos[0]
max_x, max_y = max(x_pos), max(y_pos)
min_x, min_y = min(x_pos), min(y_pos)

# Add statistics box
stats_text = f'''Trajectory Statistics:
Start: ({start_x:.3f}, {start_y:.3f}) m
End: ({final_x:.3f}, {final_y:.3f}) m
Total Distance: {total_distance:.3f} m
Net Displacement: {np.sqrt((final_x-start_x)**2 + (final_y-start_y)**2):.3f} m
X Range: {max_x-min_x:.3f} m
Y Range: {max_y-min_y:.3f} m
Duration: {time[-1]:.2f} s
Data Points: {len(x_pos)}'''

plt.text(0.02, 0.98, stats_text, transform=plt.gca().transAxes, 
         verticalalignment='top', fontsize=11,
         bbox=dict(boxstyle='round,pad=0.5', facecolor='lightblue', alpha=0.8))

# Set equal aspect ratio for accurate representation
plt.axis('equal')

plt.tight_layout()
plt.savefig('/Volumes/Eregion/projects/yars/robot_csv_trajectory.png', dpi=300, bbox_inches='tight')
plt.show()

# Print detailed analysis
print(f"YARS Robot Trajectory Analysis (Real CSV Data):")
print(f"==============================================")
print(f"Simulation details:")
print(f"  Data points: {len(x_pos)}")
print(f"  Duration: {time[-1]:.3f} seconds")
print(f"  Timestep: {dt:.3f} seconds")
print(f"")
print(f"Position analysis:")
print(f"  Start position: ({start_x:.6f}, {start_y:.6f}) m")
print(f"  Final position: ({final_x:.6f}, {final_y:.6f}) m")
print(f"  Net displacement: {np.sqrt((final_x-start_x)**2 + (final_y-start_y)**2):.6f} m")
print(f"")
print(f"Movement statistics:")
print(f"  Total distance: {total_distance:.6f} m")
print(f"  Average speed: {total_distance/time[-1]:.6f} m/s")
print(f"  X displacement: {final_x-start_x:.6f} m")
print(f"  Y displacement: {final_y-start_y:.6f} m")
print(f"  X range: {max_x-min_x:.6f} m")
print(f"  Y range: {max_y-min_y:.6f} m")
print(f"")
print(f"Efficiency:")
straight_line_distance = np.sqrt((final_x-start_x)**2 + (final_y-start_y)**2)
if total_distance > 0:
    efficiency = straight_line_distance / total_distance * 100
    print(f"  Path efficiency: {efficiency:.1f}%")
print(f"")
print(f"Plot saved as: robot_csv_trajectory.png")