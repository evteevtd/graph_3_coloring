#pragma once

#include <bits/stdc++.h>

#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;

using namespace std;

using ll = long long;
using ld = long double;
using pii = pair<int, int>;
using fast_fp = float;

#define all(a) (a).begin(), (a).end()

mt19937 rng;

fast_fp uniform() {
    return (fast_fp)rng() / rng.max();
}

const fast_fp k_pi = 2 * atan2(1, 0);

fast_fp normal() {
   return sqrt(-2 * log(uniform())) * cos(2 * k_pi * uniform());
}

fast_fp lognormal() {
    return exp(normal());
}


struct Point {
    fast_fp x = -1;
    fast_fp y = -1;
    Point() {};
    Point(fast_fp x, fast_fp y) : x(x), y(y) {}
    fast_fp len2() {
        return x * x + y + y;
    }
    fast_fp len() {
        return sqrt(len2());
    }
};

Point operator-(Point p1, Point p2) {
    p1.x -= p2.x;
    p1.y -= p2.y;
    return p1;
}

fast_fp dist2(Point p1, Point p2) {
    return (p1 - p2).len2();
}

fast_fp dist(Point p1, Point p2) {
    return (p1 - p2).len();
}

struct Edge {
    int u = -1;
    int v = -1;
    fast_fp w = 0;
    // int index = -1;
    Edge() {}
    Edge(int u, int v, fast_fp w) : u(u), v(v), w(w) {}
    Edge(int u, int v) : u(u), v(v) {
        w = lognormal();
    }
};

struct chash {
    size_t operator()(pair<int, int> x) const { return (size_t)x.first * 19311273 + (size_t)x.second * 12094721; }
};

template <typename V>
using unmap_pii = gp_hash_table<pair<int, int>, V, chash>;

struct Graph {
    int n = 0;
    vector<Point> vertex_info;
    vector<vector<Edge>> edges;

    vector<Edge> edge_list;

    Graph() {}
    Graph(int n) : n(n), vertex_info(n), edges(n) {}

    vector<Edge>& operator[](int u) {
        return edges[u];
    }
    const vector<Edge>& operator[](int u) const {
        return edges[u];
    }

    void from_edge_list() {
        edges.resize(n);
        for (int i = 0; i < edge_list.size(); ++i) { 
            auto e = edge_list[i];
            edges[e.u].push_back(e);
            swap(e.u, e.v);
            edges[e.u].push_back(e);
        }
    }

    void read(string filename) {
        ifstream myfile;
        myfile.open(filename);
        myfile >> n;
        int u, v;
        fast_fp f;
        while (myfile >> u) {
            myfile >> v >> f;
            edge_list.push_back(Edge{u, v, f});
        }
        vertex_info.resize(n);
        from_edge_list();
    }
    void write() {}
};

const fast_fp inf = 1e9 + 7;
fast_fp evaluate(const Graph& g, vector<int> coloring, bool print = false) {
    if (g.n != coloring.size()) {
        std::cerr << "evaluate " << " graph size (" << g.n << ") and coloring size (" << coloring.size() << ") do not match\n";
        return inf;
    }

    int mx_clr = -100;
    int mn_clr = 100;
    for (int i = 0; i < g.n; ++i) {
        mx_clr = max(mx_clr, coloring[i]);
        mn_clr = min(mn_clr, coloring[i]);
    }
    if (mx_clr - mn_clr + 1 > 3) {
        std::cerr << "evaluate " << mx_clr - mn_clr + 1 << " > 3 colors are used \n";
        return inf;
    }

    fast_fp score = 0;
    for (int i = 0; i < g.n; ++i) {
        for (auto e : g[i]) {
            if (coloring[e.u] == coloring[e.v]) {
                score += e.w;
            }
        }
    }

    // each edge counted twice
    score /= 2;

    if (print) {
        std::cout << "score: " << score << '\n';
        std::cerr << "score: " << score << '\n';
    
        // for (int c : coloring) {
        //     std::cout << c << ' ';
        // }
    }

    return score;
}

struct SubGraph {
    vector<int> e;  // edge indexes
    fast_fp min_cost;
    fast_fp sum_c;
};



double slow_timer() {
    return (double)clock() / CLOCKS_PER_SEC;
}

double fast_timer() {
    static int cnt = 0;
    static double last_time = 0;

    cnt++;

    if (cnt == 5000) {
        cnt = 0;
        last_time = slow_timer();
    }

    return last_time;
}





