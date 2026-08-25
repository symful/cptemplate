#include <bits/stdc++.h>
using namespace std;

int main() {
    int N, M;
    cin >> N >> M;

    vector<vector<int>> adj(N + 1); //vector<vector<pair<int,int>>> adj(N + 1);    untuk weighted

    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v; //u v w untuk weighted

        adj[u].push_back(v);
        adj[v].push_back(u); // hapus kalau directed

        //    adj[u].push_back({v, w}); untuk weighted
        //    adj[v].push_back({u, w});
        
    }

    return 0;
}


/*
cara akses
for (auto [next, weight] : adj[u]) {
    cout << next << " " << weight << '\n';
}
*/

