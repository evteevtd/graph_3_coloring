#pragma once

#include <bits/stdc++.h>
#include "common.hpp"

unmap_pii<pair<int, fast_fp>> get_edge_indexer(Graph g) {
    unmap_pii<pair<int, fast_fp>> res;
    for (int i = 0; i < g.edge_list.size(); ++i) {
        auto e = g.edge_list[i];
        assert(res.find({e.u, e.v}) == res.end());
        res[{e.u, e.v}] = {i, e.w};
        res[{e.v, e.u}] = {i, e.w};
    }
    return res;
}

vector<SubGraph> list_quads(Graph graph, auto& edge_indexer) {
    int n = graph.n;

    // vector<fast_fp> e_costs;
    // for (auto e : graph.edge_list) {
    //     e_costs.push_back(e.w);
    // }
    // sort(all(e_costs));
    // fast_fp cutoff = e_costs[e_costs.size() / 2];

    vector<vector<int>> g(n);
    for (int i = 0; i < n; ++i) {
        for (auto e : graph[i]) {
            // if (e.w >= cutoff)
            g[i].push_back(e.v);
        }
    }
    
    // leave only forward edges

    vector<int> ord(n);
    iota(all(ord), 0);
    sort(all(ord), [&](int i, int j) {
        return g[i].size() < g[j].size();
    });

    vector<int> pos(n);
    for (int i = 0; i < n; ++i) {
        pos[ord[i]] = i;
    }

    for (int i = 0; i < n; ++i) {
        auto it = remove_if(all(g[i]), [&](int j) {
            return pos[i] > pos[j];
        });
        g[i].resize(it - g[i].begin());
    }

    vector<SubGraph> res;

    vector<int> count(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j : g[i]) {
            count[j]++;
        }

        for (int j : g[i]) {
            for (int k : g[j]) {
                count[k]++;
            }
            
            for (int k : g[j]) {
                if (count[k] < 2) continue;
                for (int l : g[k]) {
                    if (count[l] == 2) {
                        // i, j, k, l
                        SubGraph nsb;
                        nsb.min_cost = 1e9;
                        array<int, 4> v = {i, j, k, l};
                        for (int i1 = 0; i1 < 4; ++i1) {
                            for (int j1 = i1 + 1; j1 < 4; ++j1) {
                                auto info = edge_indexer[{v[i1], v[j1]}];
                                nsb.e.push_back(info.first);
                                nsb.min_cost = min(nsb.min_cost, info.second);
                            }
                        }
                        res.push_back(nsb);
                    }
                }
            }
            
            for (int k : g[j]) {
                count[k]--;
            }
        }

        for (int j : g[i]) {
            count[j]--;
        }
    }

    fast_fp sum = 0;
    for (auto s : res) {
        sum += s.min_cost;
    }

    return res;
}

SubGraph from_vert(vector<int> vert, const Graph& graph, auto& edge_indexer) {
    SubGraph res;
    vector<vector<fast_fp>> c(vert.size(), vector<fast_fp>(vert.size(), 0));
    res.e.reserve(vert.size() * (vert.size() - 1));

    for (int i = 0; i < vert.size(); ++i) {
        for (int j = i + 1; j < vert.size(); ++j) {
            if (edge_indexer.find({vert[i], vert[j]}) == edge_indexer.end()) continue;
            auto [id, cost] = edge_indexer[{vert[i], vert[j]}];
            res.e.push_back(id);
            c[i][j] = cost;
            c[j][i] = cost;
        }
    }
    res.e.shrink_to_fit();

    fast_fp best = inf;
    vector<int> coloring(vert.size());

    auto calc = [&](int u, int max_c, fast_fp cur, auto&& rec) -> void {
        if (u == vert.size()) {
            best = min(best, cur);
            return;
        }
        for (int clr = 0; clr <= max_c; ++clr) {
            coloring[u] = clr;
            fast_fp cur_copy = cur;
            for (int v = 0; v < u; ++v) {
                if (coloring[v] == clr) {
                    cur_copy += c[u][v];
                }
            }
            rec(u + 1, min(2, max(max_c, clr + 1)), cur_copy, rec);
        }
    };
    calc(0, 0, 0, calc);

    res.min_cost = best;

    return res;
}

vector<SubGraph> list_cliques(Graph graph, auto& edge_indexer, int sz) {
    int n = graph.n;

    // vector<fast_fp> e_costs;
    // for (auto e : graph.edge_list) {
    //     e_costs.push_back(e.w);
    // }
    // sort(all(e_costs));
    // fast_fp cutoff = e_costs[e_costs.size() * 3 / 4];


    vector<vector<int>> g(n);
    for (int i = 0; i < n; ++i) {
        for (auto e : graph[i]) {
            // if (e.w >= cutoff)
                g[i].push_back(e.v);
        }
    }
    // leave only forward edges
    vector<int> ord(n);
    iota(all(ord), 0);
    sort(all(ord), [&](int i, int j) {
        return g[i].size() < g[j].size();
    });

    vector<int> pos(n);
    for (int i = 0; i < n; ++i) {
        pos[ord[i]] = i;
    }

    for (int i = 0; i < n; ++i) {
        auto it = remove_if(all(g[i]), [&](int j) {
            return pos[i] > pos[j];
        });
        g[i].resize(it - g[i].begin());
    }

    vector<SubGraph> res;
    vector<int> count(n, 0);

    vector<int> st;

    auto rec = [&](int u, auto&& rec) -> void {
        st.push_back(u);
        if (st.size() == sz) {
            res.push_back(from_vert(st, graph, edge_indexer));
        } else {
            for (int i : g[u]) {
                count[i]++;
            }

            for (int i : g[u]) {
                if (count[i] < st.size()) continue;
                rec(i, rec);
            }

            for (int i : g[u]) {
                count[i]--;
            }
        }
        st.pop_back();
    };

    for (int u = 0; u < n; ++u) {
        rec(u, rec);
    }

    return res;
}


vector<SubGraph> list_almost_cliques(Graph graph, auto& edge_indexer, int sz) {
    int n = graph.n;

    set<vector<int>> res_set;

    for (int it = 0; it < 3; ++it) {

        vector<vector<int>> g(n);
        for (int i = 0; i < n; ++i) {
            for (auto e : graph[i]) {
                g[i].push_back(e.v);
            }
        }
        // leave only forward edges
        vector<int> ord(n);
        iota(all(ord), 0);
        shuffle(all(ord), rng);
        // sort(all(ord), [&](int i, int j) {
        //     return g[i].size() < g[j].size();
        // });

        vector<int> pos(n);
        for (int i = 0; i < n; ++i) {
            pos[ord[i]] = i;
        }

        for (int i = 0; i < n; ++i) {
            auto it = remove_if(all(g[i]), [&](int j) {
                return pos[i] > pos[j];
            });
            g[i].resize(it - g[i].begin());
        }

        // vector<SubGraph> res;
        vector<int> count(n, 0);

        vector<int> st;

        auto rec = [&](int u, auto&& rec) -> void {
            st.push_back(u);
            if (st.size() == sz) {
                // res.push_back(from_vert(st, graph, edge_indexer));
                auto st_copy = st;
                sort(all(st_copy));
                res_set.insert(st_copy);
            } else {
                for (int i : g[u]) {
                    count[i]++;
                }

                for (int i : g[u]) {
                    if (count[i] < min<int>(st.size(), sz - 1)) {
                        continue;
                    }
                    rec(i, rec);
                }

                for (int i : g[u]) {
                    count[i]--;
                }
            }
            st.pop_back();
        };

        for (int u = 0; u < n; ++u) {
            rec(u, rec);
        }
    }

    vector<SubGraph> res;

    for (auto v : res_set) {
        res.push_back(from_vert(v, graph, edge_indexer));
    }

    return res;
}

vector<SubGraph> unite(vector<vector<SubGraph>> v) {
    vector<SubGraph> res = v[0];
    for (int i = 1 ; i < v.size(); ++i) {
        res.insert(res.end(), all(v[i]));
    }
    return res;
}

pair<vector<SubGraph>, int> list_subgraphs(Graph graph) {
    auto edge_indexer = get_edge_indexer(graph);

    // auto res4 = list_quads(graph, edge_indexer);
    // auto res5_w1 = list_almost_cliques(graph, edge_indexer, 5);
    auto res4 = list_cliques(graph, edge_indexer, 4);
    auto res5 = list_cliques(graph, edge_indexer, 5);
    auto res6 = list_cliques(graph, edge_indexer, 6);
    // auto res7 = list_cliques(graph, edge_indexer, 7);

    auto res = unite({res4, res5, res6});//, res6, res7});

    cerr << "size = " << res.size() << '\n';
    return {res, edge_indexer.size()};
}