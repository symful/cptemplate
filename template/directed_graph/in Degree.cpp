vector<int> indegree(N + 1, 0);

for (int i = 0; i < M; i++) {
    int u, v;
    cin >> u >> v;

    adj[u].push_back(v);
    indegree[v]++;
}
