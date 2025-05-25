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

    enum class StepType {
        clrChange,
        clrSwap,
    };

    struct Step {
        int v = 0;
        int c = 0;
        Edge e;
        StepType type = StepType::clrChange;
    };

    Step gen_step() {
        Step res;
        if (rng() % 3) {
            res.type = StepType::clrChange;
            res.v = rng() % g.n;
            res.c = rng() % 3;
            while (coloring[res.v] == res.c) {
                res.c = rng() % 3;
            }
        } else {
            res.type = StepType::clrSwap;
            int u = rng() % g.n;
            res.e = g[u][rng() % g[u].size()];
            if (coloring[res.e.u] == coloring[res.e.v]) {
                res.e.w = 0;
            }
        }
        return res;
    }

    fast_fp if_chagne(Step s) {
        if (s.type == StepType::clrChange) {
            return cur_error - sum_w_by_color[s.v][coloring[s.v]] + sum_w_by_color[s.v][s.c];
        } else {
            return cur_error
                 - sum_w_by_color[s.e.u][coloring[s.e.u]] + sum_w_by_color[s.e.u][coloring[s.e.v]]
                 - sum_w_by_color[s.e.v][coloring[s.e.v]] + sum_w_by_color[s.e.v][coloring[s.e.u]]
                 - 2 * s.e.w;
        }
    };

    int change(Step s, fast_fp new_error) {
        cur_error = new_error;
        if (s.type == StepType::clrChange) {
            for (const auto& e : g[s.v]) {
                sum_w_by_color[e.v][coloring[s.v]] -= e.w;
                sum_w_by_color[e.v][s.c] += e.w;
            }
            coloring[s.v] = s.c;
            return g[s.v].size();
        } else {
            for (const auto& e : g[s.e.u]) {
                sum_w_by_color[e.v][coloring[s.e.u]] -= e.w;
                sum_w_by_color[e.v][coloring[s.e.v]] += e.w;
            }
            for (const auto& e : g[s.e.v]) {
                sum_w_by_color[e.v][coloring[s.e.v]] -= e.w;
                sum_w_by_color[e.v][coloring[s.e.u]] += e.w;
            }
            swap(coloring[s.e.u], coloring[s.e.v]);
            return g[s.e.u].size() + g[s.e.v].size();
        }
    }

    int step(double temperature) {
        Step s = gen_step();
        fast_fp new_error = if_chagne(s);

        if (new_error < cur_error
            || (temperature != 0 && rand() < RAND_MAX * expf((cur_error - new_error) / temperature))) {
            return change(s, new_error);
        }
        return 0;
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
    double TL = 100; // sec

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

    rng();

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

        if (fast_timer() > last_time + 1) {
            last_time = fast_timer();

            cerr << "cur error = " << coloring.cur_error << " cur_t = " << tmp << "; ";
            cerr << "operations/sec = " << op << ";  ";
            cerr << "ticks/sec = " << ticks << '\n';
            op = 0;
            ticks = 0;

            // recount error?
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
