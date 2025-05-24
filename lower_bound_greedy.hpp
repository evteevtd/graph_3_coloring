#include <bits/stdc++.h>
#include "common.hpp"
#include "gen.hpp"
#include "list_subgraphs.hpp"

pair<fast_fp, vector<int>> simple_baseline(const vector<SubGraph>& s, int max_edge) {
    vector<int> res(s.size());
    fast_fp sum = 0;

    vector<int> ind(s.size());
    iota(all(ind), 0);

    sort(all(ind), [&](int lhs, int rhs) {
        return s[lhs].min_cost > s[rhs].min_cost;
    });

    vector<int> etaken(max_edge);
    for (auto i : ind) {
        auto ss = s[i];
        bool ok = true;
        for (auto ei : ss.e) {
            ok &= !etaken[ei];
        }
        if (ok) {
            res[i] = 1;
            for (auto ei : ss.e) {
                etaken[ei] = 1;
            }
            sum += ss.min_cost;
        }
    }
    return {sum, res};
}

vector<fast_fp> iterate_priority(const vector<SubGraph>& s, int max_edge, vector<fast_fp> priority) {
    vector<int> taken;
    vector<int> edge_is_taken;

    fast_fp cur_sum = 0;

    taken.resize(s.size());
    edge_is_taken.resize(max_edge);

    vector<fast_fp> sum_prior_by_edge(max_edge);
    for (int i = 0; i < s.size(); ++i) {
        for (auto ei : s[i].e) {
            sum_prior_by_edge[ei] += priority[i];
        }
    }

    vector<fast_fp> approx_sum_neighbors(s.size());
    for (int i = 0; i < s.size(); ++i) {
        for (auto ei : s[i].e) {
            approx_sum_neighbors[i] += sum_prior_by_edge[ei] - priority[i];
        }
    }

    vector<fast_fp> new_priority(s.size());
    for (int i = 0; i < s.size(); ++i) {
        new_priority[i] = priority[i] / approx_sum_neighbors[i];
    }
    return new_priority;
}

struct TwoMax {
    fast_fp first_max = 0;
    int first_index = 0;
    fast_fp second_max = 0;

    void upd(fast_fp value, int index) {
        if (value > first_max) {
            second_max = first_max;
            first_max = value;
            first_index = index;
        } else if (value > second_max) {
            second_max = value;
        }
    }

    fast_fp get_other(int index) {
        if (index == first_index) return second_max;
        return first_max;
    }
};

vector<fast_fp> iterate_priority_by_max(const vector<SubGraph>& s, int max_edge, vector<fast_fp> priority) {
    vector<int> taken;
    vector<int> edge_is_taken;

    fast_fp cur_sum = 0;

    taken.resize(s.size());
    edge_is_taken.resize(max_edge);

    vector<TwoMax> max_prior_by_edge(max_edge);
    for (int i = 0; i < s.size(); ++i) {
        for (auto ei : s[i].e) {
            max_prior_by_edge[ei].upd(priority[i], i);
        }
    }

    vector<fast_fp> approx_sum_neighbors(s.size());
    for (int i = 0; i < s.size(); ++i) {
        for (auto ei : s[i].e) {
            approx_sum_neighbors[i] += max_prior_by_edge[ei].get_other(i);
        }
    }

    vector<fast_fp> new_priority(s.size());
    for (int i = 0; i < s.size(); ++i) {
        new_priority[i] = priority[i] / approx_sum_neighbors[i];
    }
    return new_priority;
}

vector<int> priority_sort(const vector<SubGraph>& s, int max_edge, int iterate = 1) {
    vector<int> taken;
    vector<int> edge_is_taken;

    fast_fp cur_sum = 0;

    taken.resize(s.size());
    edge_is_taken.resize(max_edge);

    vector<fast_fp> priority(s.size());
    for (int i = 0; i < s.size(); ++i) {
        priority[i] = s[i].min_cost;
        priority[i] /= pow((s[i].e.size() + 2), 0.5);
    }
    for (int it = 0; it < iterate; ++it) {
        priority = iterate_priority(s, max_edge, priority);
    }

    vector<int> ind(s.size());
    iota(all(ind), 0);

    sort(all(ind), [&](int lhs, int rhs) {
        return priority[lhs] > priority[rhs];
    });

    return ind;
}

vector<int> priority_sort_by_max(const vector<SubGraph>& s, int max_edge, int iterate = 1) {
    vector<int> taken;
    vector<int> edge_is_taken;

    fast_fp cur_sum = 0;

    taken.resize(s.size());
    edge_is_taken.resize(max_edge);

    vector<fast_fp> priority(s.size());
    for (int i = 0; i < s.size(); ++i) {
        priority[i] = s[i].min_cost;
    }
    for (int it = 0; it < iterate; ++it) {
        priority = iterate_priority_by_max(s, max_edge, priority);
    }

    vector<int> ind(s.size());
    iota(all(ind), 0);

    sort(all(ind), [&](int lhs, int rhs) {
        return priority[lhs] > priority[rhs];
    });

    return ind;
}

pair<fast_fp, vector<int>> greedy_static(const vector<SubGraph>& s, int max_edge) {
    vector<int> taken;
    vector<int> edge_is_taken;

    fast_fp cur_sum = 0;

    taken.resize(s.size());
    edge_is_taken.resize(max_edge);

    // greedy:
    auto ind = priority_sort(s, max_edge, 1);

    for (auto i : ind) {
        auto ss = s[i];
        bool ok = true;
        for (auto ei : ss.e) {
            ok &= !edge_is_taken[ei];
        }
        if (ok) {
            taken[i] = 1;
            for (auto ei : ss.e) {
                edge_is_taken[ei] = 1;
            }
            cur_sum += s[i].min_cost;
        }
    }
    return {cur_sum, taken};
}

pair<fast_fp, vector<int>> greedy_static_retry(const vector<SubGraph>& s, int max_edge) {
    vector<int> taken;
    vector<int> edge_is_taken_by;

    fast_fp cur_sum = 0;

    taken.resize(s.size());
    edge_is_taken_by.resize(max_edge, -1);

    auto ind = priority_sort(s, max_edge, 1);

    vector<int> used(s.size());
    int used_it = 1;

    for (int it = 0; it < 3; ++it) {
        for (auto i : ind) {
            if (taken[i]) continue;

            auto ss = s[i]; 
            fast_fp sum = 0;
            
            ++used_it;
            for (auto ei : ss.e) {
                if (edge_is_taken_by[ei] == -1 || used[edge_is_taken_by[ei]] == used_it)
                    continue;
                used[edge_is_taken_by[ei]] = used_it;
                sum += s[edge_is_taken_by[ei]].min_cost;
            }

            if (sum <= ss.min_cost) {
                // redo!
                ++used_it;
                for (auto ei : ss.e) {
                    if (edge_is_taken_by[ei] == -1 || used[edge_is_taken_by[ei]] == used_it)
                        continue;
                    used[edge_is_taken_by[ei]] = used_it;
                    int oth = edge_is_taken_by[ei];
                    for (int ei : s[oth].e) {
                        edge_is_taken_by[ei] = -1;
                    }
                    taken[oth] = 0;
                }
                cur_sum -= sum;

                for (auto ei : ss.e) {
                    edge_is_taken_by[ei] = i;
                }
                taken[i] = 1;
                cur_sum += ss.min_cost;
            }
        }
    }
    return {cur_sum, taken};
}


pair<fast_fp, vector<int>> greedy_static_by_max(const vector<SubGraph>& s, int max_edge) {
    vector<int> taken;
    vector<int> edge_is_taken;

    fast_fp cur_sum = 0;

    taken.resize(s.size());
    edge_is_taken.resize(max_edge);

    // greedy:
    auto ind = priority_sort_by_max(s, max_edge, 1);

    for (auto i : ind) {
        auto ss = s[i];
        bool ok = true;
        for (auto ei : ss.e) {
            ok &= !edge_is_taken[ei];
        }
        if (ok) {
            taken[i] = 1;
            for (auto ei : ss.e) {
                edge_is_taken[ei] = 1;
            }
            cur_sum += s[i].min_cost;
        }
    }
    return {cur_sum, taken};
}


pair<fast_fp, vector<int>> greedy_dynamic(const vector<SubGraph>& s, int max_edge) {
    vector<int> taken;
    vector<int> edge_is_taken;

    vector<int> ord = priority_sort(s, max_edge);
    vector<int> ord_big = ord;
    ord_big.resize(min<int>(ord_big.size(), 500000));

    vector<vector<int>> subgraph_by_edge(max_edge);
    for (int i : ord_big) {
        for (auto ei : s[i].e) {
            subgraph_by_edge[ei].push_back(i);
        }
    }

    fast_fp cur_sum = 0;

    taken.resize(s.size());
    edge_is_taken.resize(max_edge);

    // greedy:

    vector<fast_fp> sum_w_by_edge(max_edge);
    for (int i = 0; i < s.size(); ++i) {  // here all are counted
        for (auto ei : s[i].e) {
            sum_w_by_edge[ei] += s[i].min_cost;
        }
    }

    vector<fast_fp> approx_sum_neighbors(s.size());
    for (int i : ord_big) {
        for (auto ei : s[i].e) {
            approx_sum_neighbors[i] += sum_w_by_edge[ei] - s[i].min_cost;
        }
    }
    vector<fast_fp> priorities(s.size());
    for (int i : ord_big) {
        priorities[i] = approx_sum_neighbors[i] / s[i].min_cost;
    }
    vector<int> in_pool(s.size(), 1);

    set<pair<fast_fp, int>> prior;
    for (int i : ord_big) {
        prior.insert({priorities[i], i});
    }

    vector<int> used_e(max_edge);
    vector<int> used_s(s.size());
    int used_it = 0;

    vector<fast_fp> up_edges_by(max_edge, 0);

    while (!prior.empty()) {
        auto [score, ind] = *prior.begin();
        prior.erase(prior.begin());
        in_pool[ind] = false;
        
        // can take
        auto ss = s[ind];
        taken[ind] = 1;
        for (auto ei : ss.e) {
            assert(edge_is_taken[ei] == 0);
            edge_is_taken[ei] = 1;
        }
        cur_sum += ss.min_cost;

        // clean up
        vector<int> erase_subgraphs;
        for (auto ei : ss.e) {
            for (auto si : subgraph_by_edge[ei]) {
                if (!in_pool[si]) continue;
                in_pool[si] = false;
                erase_subgraphs.push_back(si);
            }
        }

        ++used_it;
        vector<int> upd_edges;
        for (int si : erase_subgraphs) {
            for (auto ei : s[si].e) {
                up_edges_by[ei] -= s[si].min_cost;
                if (used_e[ei] == used_it) continue;
                used_e[ei] = used_it;
                upd_edges.push_back(ei);
            }
        }

        for (int si : erase_subgraphs) {
            assert(prior.count({priorities[si], si}));
            prior.erase({priorities[si], si});
        }

        vector<int> upd_s;
        for (int ei : upd_edges) {
            for (int si : subgraph_by_edge[ei]) {
                if (!in_pool[si] || used_s[si] == used_it) continue;
                used_s[si] = used_it;
                upd_s.push_back(si);
            }
        }

        for (int si : upd_s) prior.erase({priorities[si], si});

        for (int ei : upd_edges) {
            for (int si : subgraph_by_edge[ei]) {
                approx_sum_neighbors[si] += up_edges_by[ei];
            }
        }

        for (auto ei : upd_edges) {
            up_edges_by[ei] = 0;
        }

        for (int si : upd_s) {
            priorities[si] = approx_sum_neighbors[si] / s[si].min_cost;;
        }
        for (int si : upd_s) prior.insert({priorities[si], si});
    }

    // tail
    for (int i = ord_big.size(); i < ord.size(); ++i) {
    // for (int i = 0; i < ord.size(); ++i) {
        int si = ord[i];
        auto ss = s[si];
        bool ok = true;
        for (auto ei : ss.e) {
            ok &= !edge_is_taken[ei];
        }
        if (ok) {
            taken[si] = 1;
            for (auto ei : ss.e) {
                edge_is_taken[ei] = 1;
            }
            cur_sum += s[si].min_cost;
        }
    }

    return {cur_sum, taken};
}


