#pragma once

#include "common.hpp"

Graph gen(int n, int max_neighbors, fast_fp p_connection) {
    Graph g(n);
    for (int i = 0; i < n; ++i) {
        g.vertex_info[i] = Point(uniform(), uniform());
    }

    int b_cnt = sqrt(n);
    vector<vector<vector<int>>> table(b_cnt, vector<vector<int>>(b_cnt));
    for (int i = 0; i < n; ++i) {
        table[g.vertex_info[i].x * b_cnt][g.vertex_info[i].y * b_cnt].push_back(i);
    }

    vector<pair<fast_fp, pair<int, int>>> dir;
    for (int i = -b_cnt; i <= b_cnt; ++i) {
        for (int j = -b_cnt; j <= b_cnt; ++j) {
            fast_fp x = (fast_fp)i / b_cnt;
            fast_fp y = (fast_fp)j / b_cnt;
            fast_fp d = sqrt(x * x + y * y);
            d = max<fast_fp>(0, d - sqrt(2) / b_cnt);
            d = d * d;
            dir.push_back({d, {i, j}});
        }
    }
    
    sort(all(dir));

    vector<Edge> edges;

    for (int i = 0; i < n; ++i) {
        int mx = g.vertex_info[i].x * b_cnt;
        int my = g.vertex_info[i].y * b_cnt;

        vector<pair<fast_fp, int>> all_dist;

        fast_fp stop_if = 1e9;
        for (auto dd : dir) {
            if (dd.first > stop_if) {
                break;    
            }
            auto d = dd.second;

            int x1 = mx + d.first;
            int y1 = my + d.second;
            if (x1 < 0 || x1 >= b_cnt || y1 < 0 || y1 >= b_cnt) continue;

            int was_size = all_dist.size();
            for (int j : table[x1][y1]) {
                all_dist.push_back({dist2(g.vertex_info[i], g.vertex_info[j]), j});
            }

            if (all_dist.size() > max_neighbors && was_size <= max_neighbors) {
                nth_element(all_dist.begin(), all_dist.begin() + max_neighbors + 1, all_dist.end());
                stop_if = all_dist[max_neighbors].first;
            }
        }

        nth_element(all_dist.begin(), all_dist.begin() + max_neighbors + 1, all_dist.end());

        for (int j = 0; j <= max_neighbors; ++j) {
            auto elem = all_dist[j];
            if (i == elem.second) {
                continue;
            }
            if (uniform() > p_connection) {
                continue;
            }
            edges.emplace_back(i, elem.second);
        }
    }

    // compress same edges
    for (auto& e : edges) {
        if (e.u > e.v) swap(e.u, e.v);
    }
    sort(all(edges), [&](const Edge& lhs, const Edge& rhs) {
        return tie(lhs.u, lhs.v) < tie(rhs.u, rhs.v);
    });

    int at = 0;
    for (int j = 1; j < edges.size(); ++j) {
        if (edges[j].v != edges[at].v) {
            ++at;
            edges[at] = edges[j];
        } else {
            edges[at].w += edges[j].w;
        }
    }
    edges.resize(at);

    for (int i = 0; i < edges.size(); ++i) { 
        auto e = edges[i];
        g[e.u].push_back(e);
        swap(e.u, e.v);
        g[e.u].push_back(e);
    }
    g.edge_list = edges;

    return g;
}