#include <bits/stdc++.h>

using namespace std;

class Edge {
    public:
        int to;
        int weight;

        Edge(int t, int w) : to(t), weight(w) {}
};

// Cari jarak terpendek untuk berjalan dari titik `src` ke semua titik.
vector<int> dijkstra_all(int n, const vector<vector<Edge>>& adj, int src) {
    vector<int> dist(n, INT_MAX);

    priority_queue<pair<int, int>,
                        vector<pair<int, int>>,
                        greater<pair<int, int>>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // Apabila hanya ingin mencari jarak titik `src` ke titik `target` (spesifik) bisa menggunakan
        // if (u == target) return d;

        if (d > dist[u]) continue;

        for (const Edge& edge : adj[u]) {
            int v = edge.to;
            int weight = edge.weight;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }

    return dist;
}

// Cari jarak terpendek untuk berjalan dari titik `src` ke semua titik.
int dijkstra_spec(int n, const vector<vector<Edge>>& adj, int src, int target) {
    vector<int> dist(n, INT_MAX);

    priority_queue<pair<int, int>,
                        vector<pair<int, int>>,
                        greater<pair<int, int>>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // Premature return karena target sudah ditemukan
        if (u == target) return d;

        if (d > dist[u]) continue;

        for (const Edge& edge : adj[u]) {
            int v = edge.to;
            int weight = edge.weight;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }

    // Target tidak ditemukan maka hasilnya -1
    return dist[target] == INT_MAX ? -1 : dist[target];
}

int main() {
    {
        int n = 4;
        vector<vector<Edge>> adj(n);

        // Koneksi node 0 ke node 1 dengan weight 1
        adj[0].emplace_back(1, 1);
        // Koneksi node 0 ke node 2 dengan weight 4
        adj[0].emplace_back(2, 4);
        // Koneksi node 1 ke node 2 dengan weight 2
        adj[1].emplace_back(2, 2);
        // dst
        adj[1].emplace_back(3, 5);
        adj[2].emplace_back(3, 1);

        // Mencari semua jarak terpendek dari titik 0 ke semua titik

        int source = 0;
        vector<int> shortest_paths = dijkstra_all(n, adj, source);

        cout << "Jarak terpendek dari titik " << source << ":\n";
        for (int i = 0; i < n; ++i) {
            cout << "- ke titik " << i << " adalah sepanjang " << shortest_paths[i] << "\n";
        }

        int target = 3;
        int shortest_path = dijkstra_spec(n, adj, source, target);
        cout << "Jarak terpendek dari titik " << source << " ke titik " << target << " adalah " << shortest_path << "\n";
    }

    return 0;
}
