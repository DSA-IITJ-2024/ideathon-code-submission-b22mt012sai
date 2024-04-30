import matplotlib.pyplot as plt
import numpy as np

# Read the cluster data from the text file
filename = "output.txt"
clusters = {}
with open(filename, "r") as file:
    for line in file:
        if line.startswith("cluster"):
            cluster_info = line.split(":")
            cluster_num = int(cluster_info[0].split()[1])
            cluster_points = cluster_info[1].strip().split()
            clusters[cluster_num] = []
            for point in cluster_points:
                point = point.strip('[]')
                point_info = point.split('[')
                x, y = map(int, point_info[1].split(','))
                clusters[cluster_num].append((x, y))

# Plot each cluster
colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k']
for cluster_num, points in clusters.items():
    points = np.array(points)
    plt.scatter(points[:,0], points[:,1], color=colors[cluster_num % len(colors)], label=f'Cluster {cluster_num}')

# Add labels and legend
plt.title('Cluster Values')
plt.xlabel('X')
plt.ylabel('Y')
plt.legend()

# Show plot
plt.show()
