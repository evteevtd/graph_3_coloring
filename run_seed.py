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

os.system(f"python3 graph_gen.py {seed}")
graph_name = f"graph_{seed}"
filename = f"data/{graph_name}"

# Run upper/lower bounds

def compile(name):
	stat = os.system(f"g++ {name}.cpp -std=c++20 -o build/{name} -O3 -ffast-math")

print("running upper_bound")

# upper_bound = "upper_bound_with_retry"
upper_bound = "upper_bound"

compile(upper_bound)

os.system(f"./build/{upper_bound} {filename} {29 * 60} > results/{graph_name}_{upper_bound}_result")

print("running lower_bound")

# lower_bound = "lower_bound_greedy_retry"
lower_bound = "lower_bound_annealing"
compile(lower_bound)

os.system(f"./build/{lower_bound} {filename} {29 * 60} > results/{graph_name}_{lower_bound}_result")

print(f"seed = {seed}")

with open(f"results/{graph_name}_{upper_bound}_result", 'r') as ub_res:
    print(f"upped_bound_result: {ub_res.readline()}")

with open(f"results/{graph_name}_{lower_bound}_result", 'r') as lb_res:
    print(f"lower_bound_result: {lb_res.readline()}")


