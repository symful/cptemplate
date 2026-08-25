#include <bits/stdc++.h>
using namespace std;

int main() {
    int N, M;
    cin >> N >> M;

    vector<vector<int>> adj(N + 1);

    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v;

        adj[u].push_back(v);
        adj[v].push_back(u); // hapus kalau directed
    }

    return 0;
}
