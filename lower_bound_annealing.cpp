#include <bits/stdc++.h>
#include "common.hpp"
#include "gen.hpp"
#include "list_subgraphs.hpp"
#include "lower_bound_greedy.hpp"


struct IndSet {
    vector<SubGraph> s;
    vector<int> taken;
    vector<int> edge_is_taken_by;
    vector<vector<int>> subgraphs_by_edge;
    vector<fast_fp> sum_taken_neighbors;

    fast_fp cur_sum = 0;

    vector<uint64_t> used;
    vector<uint64_t> used2;
    uint64_t used_it = 1;
    uint64_t used_it2 = 1;

    IndSet() {}

    IndSet(vector<SubGraph> s, int max_edge, vector<int> baseline) : s(s), taken(baseline) {
        subgraphs_by_edge.resize(max_edge);
        for (int i = 0; i < s.size(); ++i) {
            for (auto ei : s[i].e) {
                subgraphs_by_edge[ei].push_back(i);
            }
        }
        used.resize(s.size());
        used2.resize(s.size());
        sum_taken_neighbors.resize(s.size(), 0);

        edge_is_taken_by.resize(max_edge, -1);

        for (int i = 0; i < s.size(); ++i) {
            if (!taken[i]) continue;
            cur_sum += s[i].min_cost;
            for (auto ei : s[i].e) {
                edge_is_taken_by[ei] = i;
            }
            switch_taken(i);
        }
    }

    int gen_step() {
        int res = rng() % s.size();
        while (taken[res]) {
            res = rng() % s.size();
        };
        return res;
    }

    fast_fp if_chagne(int i) {
        // auto ss = s[i];
        // fast_fp sum = 0;
        
        // ++used_it;
        // for (auto ei : ss.e) {
        //     if (edge_is_taken_by[ei] == -1 || used[edge_is_taken_by[ei]] == used_it)
        //         continue;
        //     used[edge_is_taken_by[ei]] = used_it;
        //     sum += s[edge_is_taken_by[ei]].min_cost;
        // }
        // return cur_sum + ss.min_cost - sum;

        return cur_sum + s[i].min_cost - sum_taken_neighbors[i];
    };

    void switch_taken(int i) {
        int coef = taken[i] ? 1 : -1;
        ++used_it2;

        used2[i] = used_it2;
        for (auto ei : s[i].e) {
            for (int j : subgraphs_by_edge[ei]) {
                if (used2[j] == used_it2) continue;
                used2[j] = used_it2;
                sum_taken_neighbors[j] += coef * s[i].min_cost;
            }
        }
    }

    int change(int i, fast_fp new_sum) {
        cur_sum = new_sum;
        auto ss = s[i];
        ++used_it;

        int op_cnt = 0;
        for (auto ei : ss.e) {
            if (edge_is_taken_by[ei] == -1 || used[edge_is_taken_by[ei]] == used_it)
                continue;
            used[edge_is_taken_by[ei]] = used_it;
            int oth = edge_is_taken_by[ei];
            for (int ei : s[oth].e) {
                ++op_cnt;
                edge_is_taken_by[ei] = -1;
            }
            taken[oth] = 0;
            switch_taken(oth);
        }
        for (auto ei : ss.e) {
            edge_is_taken_by[ei] = i;
        }
        taken[i] = 1;
        switch_taken(i);
        return op_cnt;
    }

    int step(double temperature) {
        auto i = gen_step();
        fast_fp new_sum = if_chagne(i);

        if (new_sum > cur_sum
            || (temperature != 0 && rand() < RAND_MAX * expf((new_sum - cur_sum) / temperature))) {
            return change(i, new_sum);
        }
        return 0;
    }

    void reset() {
        sum_taken_neighbors.assign(s.size(), 0);
        cur_sum = 0;
        for (int i = 0; i < s.size(); ++i) {
            if (!taken[i]) continue;
            cur_sum += s[i].min_cost;
            switch_taken(i);
        }
    }
};

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
    auto [score, taken] = greedy_static(subgraphs, emax);

    auto indset = IndSet(subgraphs, emax, taken);

    double start_tmp = (double)indset.cur_sum / accumulate(all(taken), 0) / 3;
    double end_tmp = start_tmp / 40;
    double TL = 60 * 15; // sec

    double k = log(end_tmp / start_tmp) / TL;

    cerr << "start temperature = " << start_tmp << '\n';
    cerr << "start sum = " << indset.cur_sum << '\n';

    auto start_time = fast_timer();
    auto last_time = fast_timer();

    long long op = 0;
    long long ticks = 0;
    long long total_steps = 0;
    long long times_upd_best = 0;

    // vector<int> best_taken = taken;
    double best_sum = indset.cur_sum;

    double log_freq = 60; // sec

    while (fast_timer() - start_time < TL) {
        ++total_steps;
        ++op;

        double tmp = start_tmp * expf(k * (fast_timer() - start_time));

        ticks += indset.step(tmp);

        if (indset.cur_sum > best_sum) {
            times_upd_best++;

            // best_taken = indset.taken;
            // recalc?
            best_sum = indset.cur_sum;
        }

        if (fast_timer() > last_time + log_freq) {
            last_time = fast_timer();

            cerr << "cur sum = " << indset.cur_sum << " cur_t = " << tmp << "; ";
            cerr << "operations/sec = " << op  / log_freq << ";  ";
            cerr << "ticks/sec = " << ticks / log_freq << '\n';
            op = 0;
            ticks = 0;

            indset.reset();
        }
    }

    cerr << "end temperature = " << end_tmp << '\n';
    cerr << "total time: " << slow_timer() << '\n'; 

    cerr << "Total steps = " << total_steps << '\n';
    cerr << "times_upd_best = " << times_upd_best << '\n';

    cerr << "time = " << slow_timer() << '\n';

    // recalc?
    cout << "lower_bound = " << best_sum << '\n';
}
