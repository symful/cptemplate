#include <bits/stdc++.h>
using namespace std;

int n, m;
vector<string> grid;

int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};

void dfs(int r, int c, vector<vector<bool>>& vis) {
    vis[r][c] = true;

    for (int i = 0; i < 4; i++) {
        int nr = r + dr[i];
        int nc = c + dc[i];

        if (nr < 0 || nr >= n || nc < 0 || nc >= m)
            continue;

        if (grid[nr][nc] == '#')
            continue;

        if (vis[nr][nc])
            continue;

        dfs(nr, nc, vis);
    }
}

int main() {
    cin >> n >> m;

    grid.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> grid[i];
    }

    vector<vector<bool>> vis(n, vector<bool>(m, false));

    dfs(0, 0, vis);

    return 0;
}
