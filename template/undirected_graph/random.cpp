#include <bits/stdc++.h>
using namespace std;

/*
============================================================
TREE & GRAPH TEMPLATE
============================================================

Isi:
1. DFS Tree
2. Rerooting DP
3. DSU with Parity
4. LCA (Binary Lifting)
5. XOR Path

============================================================
*/


/*
============================================================
1. DFS TREE
============================================================

Dipakai untuk:
- traversal tree
- menghitung subtree
- mencari depth
- tree DP

Pola utama:

void dfs(int cur, int par) {
    for (auto next : adj[cur]) {
        if (next == par) continue;

        dfs(next, cur);
    }
}
*/

int n;
vector<vector<int>> adj;
vector<bool> vis;

void dfs(int cur, int par) {

    vis[cur] = true;

    // proses cur di sini


    for (auto next : adj[cur]) {

        if (next == par)
            continue;

        if (vis[next])
            continue;

        dfs(next, cur);
    }
}


/*
============================================================
2. REROOTING DP
============================================================

Pola umum:

- dfs1() : hitung informasi subtree
- dfs2() : sebarkan informasi dari parent ke child

Contoh:
menghitung jumlah ukuran subtree.

dp[cur] = informasi dari subtree cur

ans[cur] = jawaban ketika root berada di cur
*/

vector<int> dp;
vector<int> ans;

void dfs1(int cur, int par) {

    dp[cur] = 1;

    for (auto next : adj[cur]) {

        if (next == par)
            continue;

        dfs1(next, cur);

        dp[cur] += dp[next];
    }
}

void dfs2(int cur, int par) {

    // ans[cur] sudah diketahui

    for (auto next : adj[cur]) {

        if (next == par)
            continue;

        /*
        --------------------------------
        REROOTING TRANSITION
        --------------------------------

        sebelum:
            cur -> next

        sesudah:
            next -> cur

        Sesuaikan rumus dengan soal.
        */

        ans[next] = ans[cur];

        dfs2(next, cur);
    }
}


/*
============================================================
3. DSU WITH PARITY
============================================================

Dipakai ketika hubungan antar node memiliki parity:

parity = 0 -> hubungan sama
parity = 1 -> hubungan berbeda

Contoh:
x dan y harus berbeda:
union(x, y, 1)

x dan y harus sama:
union(x, y, 0)

parent[x] = parent DSU
sz[x]     = ukuran component
parity[x] = parity x terhadap parent[x]
*/

vector<int> parent;
vector<int> sz;
vector<int> parity;

pair<int, int> find(int cur) {

    if (parent[cur] == cur)
        return {cur, 0};

    auto [root, p] = find(parent[cur]);

    parity[cur] ^= p;

    parent[cur] = root;

    return {parent[cur], parity[cur]};
}

/*
relation:

return:
0 -> sama
1 -> berbeda
*/

bool unite(int u, int v, int relation) {

    auto [rootU, parityU] = find(u);
    auto [rootV, parityV] = find(v);

    // sudah satu component
    if (rootU == rootV) {

        // cek apakah constraint konsisten
        return ((parityU ^ parityV) == relation);
    }

    // union by size
    if (sz[rootU] < sz[rootV]) {
        swap(rootU, rootV);
        swap(parityU, parityV);
    }

    parent[rootV] = rootU;
    sz[rootU] += sz[rootV];

    /*
    parity[rootV] harus dibuat sehingga:

    parityU ^ parity[rootV] ^ parityV
        = relation

    sehingga:

    parity[rootV]
        = parityU ^ parityV ^ relation
    */

    parity[rootV] = parityU ^ parityV ^ relation;

    return true;
}


/*
============================================================
4. LCA - LOWEST COMMON ANCESTOR
============================================================

Binary Lifting.

up[k][cur]
=
ancestor cur sejauh 2^k level ke atas.

Contoh:

up[0][cur] = parent cur
up[1][cur] = ancestor 2 langkah
up[2][cur] = ancestor 4 langkah
up[3][cur] = ancestor 8 langkah

LCA complexity:

Preprocessing : O(N log N)
Query         : O(log N)
*/

const int LOG = 20;

vector<vector<int>> up;
vector<int> depth;

void dfs_lca(int cur, int par) {

    up[0][cur] = par;

    for (int k = 1; k < LOG; k++) {

        up[k][cur] = up[k - 1][
            up[k - 1][cur]
        ];
    }

    for (auto next : adj[cur]) {

        if (next == par)
            continue;

        depth[next] = depth[cur] + 1;

        dfs_lca(next, cur);
    }
}

int jump(int cur, int distance) {

    for (int k = 0; k < LOG; k++) {

        if (distance & (1 << k)) {

            cur = up[k][cur];
        }
    }

    return cur;
}

int lca(int u, int v) {

    // pastikan depth u >= depth v
    if (depth[u] < depth[v])
        swap(u, v);

    // samakan depth
    u = jump(u, depth[u] - depth[v]);

    if (u == v)
        return u;

    // naik dari terbesar
    for (int k = LOG - 1; k >= 0; k--) {

        if (up[k][u] != up[k][v]) {

            u = up[k][u];
            v = up[k][v];
        }
    }

    return up[0][u];
}


/*
============================================================
5. XOR PATH
============================================================

Untuk tree dengan edge memiliki weight:

u --w--> v

Kita bisa menyimpan:

xr[cur] =
XOR semua edge dari root sampai cur.

Maka XOR path u -> v:

xr[u] ^ xr[v]

Karena bagian root -> LCA akan terhapus.

Contoh:

root
 |
 5
 |
 A
 |
 7
 |
 B

xr[A] = 5
xr[B] = 5 ^ 7

XOR path A -> B:

xr[A] ^ xr[B]
= 5 ^ (5 ^ 7)
= 7
*/


vector<int> xr;

void dfs_xor(int cur, int par) {

    for (auto [next, weight] : weighted_adj[cur]) {

        if (next == par)
            continue;

        xr[next] = xr[cur] ^ weight;

        dfs_xor(next, cur);
    }
}


/*
Weighted adjacency list untuk XOR Path:

vector<vector<pair<int,int>>> weighted_adj;

Format:

weighted_adj[u].push_back({v, weight});
weighted_adj[v].push_back({u, weight});
*/


vector<vector<pair<int, int>>> weighted_adj;


/*
============================================================
MAIN
============================================================

Bagian main di bawah ini hanya contoh initialization.

Hapus bagian yang tidak diperlukan ketika mengerjakan soal.
*/

int main() {

    /*
    ========================================================
    INPUT TREE
    ========================================================
    */

    cin >> n;

    adj.resize(n + 1);

    for (int i = 0; i < n - 1; i++) {

        int u, v;
        cin >> u >> v;

        adj[u].push_back(v);
        adj[v].push_back(u);
    }


    /*
    ========================================================
    DFS TREE
    ========================================================
    */

    vis.assign(n + 1, false);

    dfs(1, 0);


    /*
    ========================================================
    REROOTING
    ========================================================
    */

    dp.assign(n + 1, 0);
    ans.assign(n + 1, 0);

    dfs1(1, 0);

    // Tentukan ans[1] sesuai kebutuhan soal

    dfs2(1, 0);


    /*
    ========================================================
    DSU WITH PARITY
    ========================================================
    */

    parent.resize(n + 1);
    sz.assign(n + 1, 1);
    parity.assign(n + 1, 0);

    for (int i = 1; i <= n; i++) {
        parent[i] = i;
    }

    /*
    Contoh:

    unite(1, 2, 0);
        -> 1 dan 2 harus sama

    unite(2, 3, 1);
        -> 2 dan 3 harus berbeda
    */


    /*
    ========================================================
    LCA
    ========================================================
    */

    up.assign(LOG, vector<int>(n + 1, 0));
    depth.assign(n + 1, 0);

    dfs_lca(1, 1);

    /*
    Contoh:

    int u, v;
    cin >> u >> v;

    cout << lca(u, v) << '\n';
    */


    /*
    ========================================================
    XOR PATH
    ========================================================
    */

    weighted_adj.resize(n + 1);
    xr.assign(n + 1, 0);

    /*
    Untuk soal sebenarnya,
    input edge weighted harus dimasukkan ke:

    int u, v, weight;

    weighted_adj[u].push_back({v, weight});
    weighted_adj[v].push_back({u, weight});

    kemudian:

    dfs_xor(1, 0);

    XOR path u -> v:

    int result = xr[u] ^ xr[v];
    */


    return 0;
}
