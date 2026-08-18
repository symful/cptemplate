#include <bits/stdc++.h>

using namespace std;

class Edge {
public:
    int src;
    int dest;

    Edge(int s, int d) : src(s), dest(d) {}
};

vector<int> bfs_all(int n, const vector<Edge>& edges, int source) {
    vector<vector<int>> adj(n);
    for (const auto& edge : edges) {
        adj[edge.src].push_back(edge.dest);
        adj[edge.dest].push_back(edge.src);
    }
    vector<int> order;
    vector<bool> visited(n, false);
    for (int i = source; i < n; ++i) {
        if (!visited[i]) {
            queue<int> q;
            q.push(i);
            visited[i] = true;
            while (!q.empty()) {
                int curr = q.front();
                q.pop();
                order.push_back(curr);
                for (int neighbor : adj[curr]) {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        q.push(neighbor);
                    }
                }
            }
        }
    }
    for (int i = 0; i < source; ++i) {
        if (!visited[i]) {
            queue<int> q;
            q.push(i);
            visited[i] = true;
            while (!q.empty()) {
                int curr = q.front();
                q.pop();
                order.push_back(curr);
                for (int neighbor : adj[curr]) {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        q.push(neighbor);
                    }
                }
            }
        }
    }
    return order;
}

vector<int> bfs_specific(int n, const vector<Edge>& edges, int start, int target) {
    vector<vector<int>> adj(n);
    for (const auto& edge : edges) {
        adj[edge.src].push_back(edge.dest);
        adj[edge.dest].push_back(edge.src);
    }
    vector<int> order;
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(start);
    visited[start] = true;
    while (!q.empty()) {
        int curr = q.front();

        if (target == curr) break;

        q.pop();
        order.push_back(curr);
        for (int neighbor : adj[curr]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    return order;
}

void dfs_all_util(int node, const vector<vector<int>>& adj, vector<bool>& visited, vector<int>& order) {
    visited[node] = true;
    order.push_back(node);
    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            dfs_all_util(neighbor, adj, visited, order);
        }
    }
}

vector<int> dfs_all(int n, const vector<Edge>& edges, int source) {
    vector<vector<int>> adj(n);
    for (const auto& edge : edges) {
        adj[edge.src].push_back(edge.dest);
        adj[edge.dest].push_back(edge.src);
    }
    vector<int> order;
    vector<bool> visited(n, false);
    for (int i = source; i < n; ++i) {
        if (!visited[i]) {
            dfs_all_util(i, adj, visited, order);
        }
    }
    for (int i = 0; i < source; ++i) {
        if (!visited[i]) {
            dfs_all_util(i, adj, visited, order);
        }
    }
    return order;
}

void dfs_specific_util(int node, const vector<vector<int>>& adj, vector<bool>& visited, vector<int>& order, int target) {
    if (visited[target]) return;
    visited[node] = true;
    order.push_back(node);
    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            dfs_specific_util(neighbor, adj, visited, order, target);
        }
    }
}

vector<int> dfs_specific(int n, const vector<Edge>& edges, int start, int target) {
    vector<vector<int>> adj(n);
    for (const auto& edge : edges) {
        adj[edge.src].push_back(edge.dest);
        adj[edge.dest].push_back(edge.src);
    }
    vector<int> order;
    vector<bool> visited(n, false);
    dfs_specific_util(start, adj, visited, order, target);
    return order;
}

int main() {
    int n = 6;
    vector<Edge> edges;

    // Koneksi titik 0 ke titik 1
    edges.emplace_back(0, 1);
    // Koneksi titik 0 ke titik 2
    edges.emplace_back(0, 2);
    // dst
    edges.emplace_back(1, 3);
    edges.emplace_back(1, 4);
    edges.emplace_back(2, 4);
    edges.emplace_back(5, 5);

    int source = 0;
    int target = 3;

    cout << "BFS semua titik dari titik " << source << ": ";
    vector<int> res_bfs_all = bfs_all(n, edges, source);
    for (int x : res_bfs_all) cout << x << " ";
    cout << "\n";

    cout << "BFS dari titik " << source << " ke titik " << target << ": ";
    vector<int> res_bfs_spec = bfs_specific(n, edges, source, target);
    for (int x : res_bfs_spec) cout << x << " ";
    cout << "\n";

    cout << "DFS semua titik dari titik " << source << ": ";
    vector<int> res_dfs_all = dfs_all(n, edges, source);
    for (int x : res_dfs_all) cout << x << " ";
    cout << "\n";

    cout << "DFS dari titik " << source << " ke titik " << target << ": ";
    vector<int> res_dfs_spec = dfs_specific(n, edges, source, target);
    for (int x : res_dfs_spec) cout << x << " ";
    cout << "\n";

    return 0;
}
