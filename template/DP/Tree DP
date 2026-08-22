#include <bits/stdc++.h>
using namespace std;

int n;
vector<vector<int>> adj;

vector<vector<int>> dp;

void dfs(int cur, int par) {
    // base case
    dp[cur][0] = 0;
    dp[cur][1] = 1;

    for (auto next : adj[cur]) {
        if (next == par)
            continue;

        dfs(next, cur);

        // cur tidak dipilih
        dp[cur][0] += max(dp[next][0], dp[next][1]);

        // cur dipilih
        dp[cur][1] += dp[next][0];
    }
}

int main() {
    cin >> n;

    adj.resize(n + 1);
    dp.assign(n + 1, vector<int>(2, 0));

    for (int i = 0; i < n - 1; i++) {
        int u, v;
        cin >> u >> v;

        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    dfs(1, 0);

    cout << max(dp[1][0], dp[1][1]) << endl;

    return 0;
}
