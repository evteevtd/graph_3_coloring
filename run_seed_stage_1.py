import sys
import os
import networkx as nx
import numpy as np
from sklearn.neighbors import NearestNeighbors
from scipy.sparse import lil_matrix

argc = len(sys.argv)

if argc == 1:
    print(f"using default seed = 42")
    seed = 42
else:
    seed = int(sys.argv[1])


n_nodes = 20000
max_neighbors = 60
p_connection = 0.5
rs = np.random.RandomState(seed)

# Set positions
xy = rs.rand(n_nodes, 2)

# Fill the adjacency matrix
nbrs = NearestNeighbors(n_neighbors=max_neighbors+1).fit(xy)
_, indices = nbrs.kneighbors(xy)

adj = lil_matrix((n_nodes, n_nodes))
for i, neighbors in enumerate(indices):
    for j in neighbors[1:]:
        if p_connection > rs.rand():
            adj[i, j] = np.exp(rs.randn())
adj = adj + adj.T

# Create graph
G = nx.from_scipy_sparse_array(adj)

n_colors = 3
colors = rs.randint(0, n_colors, n_nodes)

sum_conflict = 0
for u, v, data in G.edges(data=True):
    if colors[u] == colors[v]:
        sum_conflict += data['weight']

# Write graph data to a file

graph_name = f"graph_{seed}"
filename = f"data/{graph_name}"

with open(filename, 'w') as ouf:
    ouf.write(f"{n_nodes}\n")
    for u, v, data in G.edges(data=True):
        ouf.write(f"{u} {v} {data['weight']}\n")

# Run upper/lower bounds

def compile(name):
	stat = os.system(f"g++ {name}.cpp -o build/{name} -O3 -ffast-math 2> /dev/null")

print("running upper_bound")

# upper_bound = "upper_bound_with_retry"
upper_bound = "upper_bound"

compile(upper_bound)

os.system(f"./build/{upper_bound} {filename} > results/{graph_name}_{upper_bound}_result")

print("running lower_bound")

lower_bound = "lower_bound_greedy_retry"
compile(lower_bound)

# os.system(f"./build/{lower_bound} {filename} > results/{graph_name}_{lower_bound}_result")

print(f"seed = {seed}")

with open(f"results/{graph_name}_{upper_bound}_result", 'r') as ub_res:
    print(f"upped_bound_result: {ub_res.readline()}")

with open(f"results/{graph_name}_{lower_bound}_result", 'r') as lb_res:
    print(f"lower_bound_result: {lb_res.readline()}")


