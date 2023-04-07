import subprocess
import numpy as np
from sklearn.cluster import KMeans

# generate random numbers file
numbers = np.random.randint(4, 33)
subprocess.run(["dd", "if=/dev/random", f"bs=1", f"count={numbers}", "of=numbers"])

# run run.sh and get output
mpirun_output = subprocess.run(["bash", "run.sh"], capture_output=True, text=True).stdout.strip().split("\n")


# process mpirun output and print it in desired format
for line in mpirun_output:
    tokens = line.strip().split()
    centroid = tokens[0][1:6]
    points = " ".join(tokens[2:])
    print(f"[{centroid}] <- {points}")

# compare mpirun output with sklearn KMeans
data = np.fromfile("numbers", dtype=np.uint8)
subprocess.run(["rm", "numbers"])
init_centroids = data[:4].reshape(-1, 1)
kmeans = KMeans(n_clusters=4,init=init_centroids).fit(data.reshape(-1, 1))



# print sklearn KMeans center and points
for i in range(4):
    print(f"[{kmeans.cluster_centers_[i][0]}]")