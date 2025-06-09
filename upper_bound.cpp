#include <bits/stdc++.h>
#include "common.hpp"
#include "gen.hpp"


struct Coloring {
    Graph g;

    vector<array<fast_fp, 3>> sum_w_by_color;
    vector<int> coloring;
    fast_fp cur_error = 0;

    Coloring() {}

    Coloring(Graph g, vector<int> base_coloring) : g(g), coloring(base_coloring) {
        sum_w_by_color.resize(g.n);

        for (int i = 0; i < g.n; ++i) {
            for (auto e : g[i]) {
                sum_w_by_color[e.u][coloring[e.v]] += e.w;
                if (coloring[e.u] == coloring[e.v]) {
                    cur_error += e.w;
                }
            }
        }
        cur_error /= 2;
    }

    pair<int, int> gen_step() {
        int v = rng() % g.n;
        int c = rng() % 3;
        while (coloring[v] == c) {
            c = rng() % 3;
        }
        return {v, c};
    }

    fast_fp if_chagne(int v, int new_c) {
        return cur_error - sum_w_by_color[v][coloring[v]] + sum_w_by_color[v][new_c];
    };

    void change(int u, int new_c, fast_fp new_error) {
        cur_error = new_error;
        for (const auto& e : g[u]) {
            sum_w_by_color[e.v][coloring[u]] -= e.w;
            sum_w_by_color[e.v][new_c] += e.w;
        }
        coloring[u] = new_c;
    }

    int step(double temperature) {
        auto [v, c] = gen_step();
        fast_fp new_error = if_chagne(v, c);

        if (new_error < cur_error
            || (temperature != 0 && rand() < RAND_MAX * expf((cur_error - new_error) / temperature))) {
            change(v, c, new_error);

            return g[v].size();
        }
        return 0;
    }

    void reset() {
        sum_w_by_color.assign(g.n, array<fast_fp, 3>{0, 0, 0});
        cur_error = 0;

        for (int i = 0; i < g.n; ++i) {
            for (auto e : g[i]) {
                sum_w_by_color[e.u][coloring[e.v]] += e.w;
                if (coloring[e.u] == coloring[e.v]) {
                    cur_error += e.w;
                }
            }
        }
        cur_error /= 2;
    }
};

vector<int> baseline(Graph g) {
    vector<int> coloring(g.n);
    for (int i = 0; i < g.n; ++i) {
        coloring[i] = rng() % 3;
    }
    return coloring;
}

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
    vector<int> base_coloring = baseline(g);

    Coloring coloring(g, base_coloring);

    double start_tmp = (double)coloring.cur_error / n;
    double end_tmp = start_tmp / 300;
    double TL = 28 * 60; // sec

    double k = log(end_tmp / start_tmp) / TL;

    cerr << "start temperature = " << start_tmp << '\n';
    cerr << "start error = " << coloring.cur_error << '\n';

    auto start_time = fast_timer();
    auto last_time = fast_timer();

    long long op = 0;
    long long ticks = 0;
    long long total_steps = 0;
    long long times_upd_best = 0;

    vector<int> best_coloring = base_coloring;
    double best_error = coloring.cur_error;

    double log_freq = 20; // sec

    while (fast_timer() - start_time < TL) {
        ++total_steps;
        ++op;

        double tmp = start_tmp * expf(k * (fast_timer() - start_time));

        ticks += coloring.step(tmp);

        if (coloring.cur_error < best_error) {
            times_upd_best++;

            best_coloring = coloring.coloring;
            best_error = coloring.cur_error;
        }

        if (fast_timer() > last_time + log_freq) {
            last_time = fast_timer();

            cerr << "cur error = " << coloring.cur_error << " cur_t = " << tmp << "; ";
            cerr << "operations/sec = " << op / log_freq << ";  ";
            cerr << "ticks/sec = " << ticks / log_freq << '\n';
            op = 0;
            ticks = 0;

            coloring.reset();
        }
    }

    cerr << "end temperature = " << end_tmp << '\n';
    cerr << "total time: " << slow_timer() << '\n'; 
    cerr << "end errors = " << coloring.cur_error << '\n';
    cerr << "\n";

    cerr << "Total steps = " << total_steps << '\n';
    cerr << "times_upd_best = " << times_upd_best << '\n';

    evaluate(g, best_coloring, true);
}
