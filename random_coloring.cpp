#include <bits/stdc++.h>
#include "common.hpp"
#include "gen.hpp"


signed main(int argc, char* argv[]) {
    Graph g;

    if (argc == 1) {
        int n = 20000;
        int max_neighbors = 60;
        fast_fp p_connection = 0.5;

        rng.seed(43);
        g = gen(n, max_neighbors, p_connection);
    } else {
        g.read(argv[1]);
    }

    int n = g.n;
    
    vector<int> coloring(n);
    for (int i = 0; i < n; ++i) {
        coloring[i] = rng() % 3;
    }

    evaluate(g, coloring, true);
}
