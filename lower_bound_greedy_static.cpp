#include <bits/stdc++.h>
#include "common.hpp"
#include "gen.hpp"
#include "list_subgraphs.hpp"
#include "lower_bound_greedy.hpp"

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

    auto [subgraphs, emax] = list_subgraphs(g);
    auto [sum, vec] = greedy_static(subgraphs, emax);

    cerr << "time = " << slow_timer() << '\n';

    cout << "lower_bound = " << sum << '\n';
}
