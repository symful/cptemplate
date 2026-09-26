#include <bits/stdc++.h>
using namespace std;

// ============================================================
// MEX
// ============================================================
// mex of a set of nonnegative integers = smallest nonnegative
// integer not present in the set.
// ============================================================
int mex(const set<int>& seen) {
    int g = 0;
    while (seen.count(g)) ++g;
    return g;
}

// ============================================================
// GENERIC GRUNDY WITH MEMOIZATION
// ============================================================
// State must be hashable / comparable.
// next_states(s) returns all states reachable in one move.
// ============================================================
template <typename State, typename NextFn>
struct Grundy {
    map<State, int> memo;
    NextFn next_states;

    Grundy(NextFn fn) : next_states(fn) {}

    int G(const State& s) {
        auto it = memo.find(s);
        if (it != memo.end()) return it->second;

        set<int> seen;
        for (const State& ns : next_states(s)) {
            seen.insert(G(ns));
        }
        int g = mex(seen);
        memo[s] = g;
        return g;
    }
};

// ============================================================
// EXAMPLE 1: SUBTRACTION GAME
// ============================================================
// One pile. Remove a number from allowed set S.
// State: int n.
// ============================================================
vector<int> subtraction_grundy(int N, const vector<int>& S) {
    vector<int> G(N + 1, 0);
    for (int n = 1; n <= N; ++n) {
        set<int> seen;
        for (int s : S) {
            if (s <= n) seen.insert(G[n - s]);
        }
        G[n] = mex(seen);
    }
    return G;
}

// ============================================================
// EXAMPLE 2: KAYLES
// ============================================================
// Row of n pins. Remove one pin, or two adjacent pins.
// Row splits into two independent rows.
// State: int n.
// ============================================================
vector<int> kayles_grundy(int N) {
    vector<int> G(N + 1, 0);
    for (int n = 1; n <= N; ++n) {
        set<int> seen;
        // remove one pin at position i (1-indexed)
        for (int i = 1; i <= n; ++i) {
            seen.insert(G[i - 1] ^ G[n - i]);
        }
        // remove two adjacent pins at i and i+1
        for (int i = 1; i <= n - 1; ++i) {
            seen.insert(G[i - 1] ^ G[n - i - 1]);
        }
        G[n] = mex(seen);
    }
    return G;
}

// ============================================================
// EXAMPLE 3: WYTHOFF GAME
// ============================================================
// Two piles (a, b).
// Move: remove any positive from one pile,
//       or remove same positive from both piles.
// State: pair<int,int>.
// ============================================================
vector<vector<int>> wythoff_grundy(int N) {
    vector<vector<int>> G(N + 1, vector<int>(N + 1, 0));
    for (int a = 0; a <= N; ++a) {
        for (int b = 0; b <= N; ++b) {
            if (a == 0 && b == 0) {
                G[a][b] = 0;
                continue;
            }
            set<int> seen;
            for (int k = 1; k <= a; ++k) seen.insert(G[a - k][b]);
            for (int k = 1; k <= b; ++k) seen.insert(G[a][b - k]);
            for (int k = 1; k <= min(a, b); ++k) seen.insert(G[a - k][b - k]);
            G[a][b] = mex(seen);
        }
    }
    return G;
}

// ============================================================
// EXAMPLE 4: WHITE-BLACK TREE
// ============================================================
// Tree with colors. Move: choose white node, turn it black,
// then optionally flip any descendants.
// Grundy = XOR of 2^(height-1) for all white nodes.
// Since powers of two, track parity per height.
// ============================================================
string white_black_tree(int N, const vector<int>& parent, const vector<int>& color) {
    vector<int> height(N + 1, 1);
    for (int i = N; i >= 2; --i) {
        int p = parent[i];
        height[p] = max(height[p], height[i] + 1);
    }

    vector<bool> parity(N + 1, false);
    for (int i = 1; i <= N; ++i) {
        if (color[i] == 1) {
            parity[height[i]] = !parity[height[i]];
        }
    }

    for (int h = 1; h <= N; ++h) {
        if (parity[h]) return "First";
    }
    return "Second";
}

// ============================================================
// EXAMPLE 5: INDEPENDENT SUM
// ============================================================
// If the game is a sum of independent parts, compute G
// for each part and XOR them.
// ============================================================
int independent_sum(const vector<int>& part_grundy) {
    int x = 0;
    for (int g : part_grundy) x ^= g;
    return x;
}

// ============================================================
// MAIN
// ============================================================
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Example: subtraction game with S = {1, 2}
    {
        int N = 10;
        vector<int> S = {1, 2};
        vector<int> G = subtraction_grundy(N, S);
        cout << "Subtraction {1,2}:\n";
        for (int n = 0; n <= N; ++n) {
            cout << "G(" << n << ") = " << G[n] << "\n";
        }
        cout << "\n";
    }

    // Example: Kayles
    {
        int N = 10;
        vector<int> G = kayles_grundy(N);
        cout << "Kayles:\n";
        for (int n = 0; n <= N; ++n) {
            cout << "G(" << n << ") = " << G[n] << "\n";
        }
        cout << "\n";
    }

    // Example: Wythoff
    {
        int N = 5;
        auto G = wythoff_grundy(N);
        cout << "Wythoff (a,b) for a,b <= " << N << ":\n";
        for (int a = 0; a <= N; ++a) {
            for (int b = 0; b <= N; ++b) {
                cout << G[a][b] << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    // Example: White-Black Tree
    {
        int N = 7;
        vector<int> parent = {0, 0, 1, 1, 2, 2, 3, 3};
        vector<int> color  = {0, 0, 1, 1, 0, 0, 0, 1};
        cout << "White-Black Tree: "
             << white_black_tree(N, parent, color) << "\n";
    }

    return 0;
}
