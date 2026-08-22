#include <bits/stdc++.h>
using namespace std;

int n, m;
vector<string> grid;

int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};

void bfs(int sr, int sc) {
    queue<pair<int, int>> q;
    vector<vector<bool>> vis(n, vector<bool>(m, false));

    q.push({sr, sc});
    vis[sr][sc] = true;

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr < 0 || nr >= n || nc < 0 || nc >= m)
                continue;

            if (grid[nr][nc] == '#')
                continue;

            if (vis[nr][nc])
                continue;

            vis[nr][nc] = true;
            q.push({nr, nc});
        }
    }
}

int main() {
    cin >> n >> m;

    grid.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> grid[i];
    }

    bfs(0, 0);

    return 0;
}
