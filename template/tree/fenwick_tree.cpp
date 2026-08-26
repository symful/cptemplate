#include <bits/stdc++.h>
using namespace std;

class FenwickTree {
private:
    vector<long long> bit;
    int n;
    long long MOD = 1e9 + 7; // Useful for modular operations

    // =================== CONFIG (The Cheat Sheet) ===================

    // --- 1. IDENTITY ---
    // SUM / XOR / MOD_SUM: 0
    // PROD / MOD_PROD:     1
    long long identity() { return 0; }

    // --- 2. COMBINE ---
    // SUM:      return a + b;
    // XOR:      return a ^ b;
    // PROD:     return a * b;
    // MOD_SUM:  return (a + b) % MOD;
    // MOD_PROD: return (a * b) % MOD;
    long long combine(long long a, long long b) {
        return a + b;
    }

    // --- 3. INVERSE (Crucial for Range Queries) ---
    // SUM:      return -a;
    // XOR:      return a; (XOR is its own inverse)
    // MOD_SUM:  return (MOD - a) % MOD;
    // MOD_PROD: return modInverse(a); (Requires binary exponentiation: binpow(a, MOD-2))
    long long inverse(long long a) {
        return -a;
    }
    // ================================================================

    void build(vector<int>& arr) {
        for (int i = 1; i <= n; i++) {
            bit[i] = combine(bit[i], arr[i - 1]);
            int p = i + (i & -i);
            if (p <= n) bit[p] = combine(bit[p], bit[i]);
        }
    }

    long long prefix(int pos) {
        long long res = identity();
        for (int idx = pos + 1; idx > 0; idx -= (idx & -idx)) {
            res = combine(res, bit[idx]);
        }
        return res;
    }

    long long get(int pos) {
        return combine(prefix(pos), inverse(prefix(pos - 1)));
    }

    void add(int pos, long long delta) {
        for (int idx = pos + 1; idx <= n; idx += (idx & -idx)) {
            bit[idx] = combine(bit[idx], delta);
        }
    }

public:
    FenwickTree(vector<int>& arr) {
        n = arr.size();
        bit.assign(n + 1, identity());
        if (n > 0) build(arr);
    }

    // Point Update (Safely sets arr[pos] = val)
    void update(int pos, long long val) {
        long long delta = combine(val, inverse(get(pos)));
        add(pos, delta);
    }

    // Point Add (arr[pos] += val, faster if you don't need to overwrite)
    void point_add(int pos, long long val) {
        add(pos, val);
    }

    // Range Query [l, r]
    long long query(int l, int r) {
        return combine(prefix(r), inverse(prefix(l - 1)));
    }
};

class RangeFenwickTree {
private:
    vector<long long> bit1, bit2;
    int n;

    void add(vector<long long>& bit, int pos, long long val) {
        for (int idx = pos + 1; idx <= n; idx += (idx & -idx)) bit[idx] += val;
    }

    long long query_internal(const vector<long long>& bit, int pos) {
        long long res = 0;
        for (int idx = pos + 1; idx > 0; idx -= (idx & -idx)) res += bit[idx];
        return res;
    }

    long long prefix(int pos) {
        // Core RURQ Math: sum(bit1) * pos - sum(bit2)
        return query_internal(bit1, pos) * pos - query_internal(bit2, pos);
    }

public:
    RangeFenwickTree(vector<int>& arr) {
        n = arr.size();
        bit1.assign(n + 1, 0);
        bit2.assign(n + 1, 0);
        for (int i = 0; i < n; i++) update(i, i, arr[i]);
    }

    // Range Update: Add 'val' to range [l, r]
    void update(int l, int r, long long val) {
        add(bit1, l, val);
        add(bit1, r + 1, -val);
        add(bit2, l, val * (l - 1));
        add(bit2, r + 1, -val * r);
    }

    // Range Query: Sum of range [l, r]
    long long query(int l, int r) {
        return prefix(r) - prefix(l - 1);
    }
};

// ============================================================
//                       2D FENWICK TREE
// ============================================================
//
// Supports:
//
//   1. Point Add
//        a[x][y] += delta
//
//   2. Point Update / Set
//        a[x][y] = value
//
//   3. Point Get
//        get(x, y)
//
//   4. Prefix Query
//        sum of [1..x] × [1..y]
//
//   5. Rectangle Query
//        sum of [x1..x2] × [y1..y2]
//
//   6. Build from an existing 2D array
//
//
//
// Complexity:
//
//   add()       O(log N * log M)
//   update()    O(log N * log M)
//   get()       O(1)
//   prefix()    O(log N * log M)
//   query()     O(log N * log M)
//   build()     O(N * M * log N * log M)
//
//
//
// IMPORTANT:
//
// Fenwick trees are 1-indexed.
//
// Valid coordinates:
//
//   x = 1 ... n
//   y = 1 ... m
//
// ============================================================

class FenwickTree2D {

private:

    int n, m;

    // Fenwick tree storage.
    //
    // bit[x][y] does NOT simply represent a[x][y].
    //
    // Each cell stores an aggregate over a rectangle
    // determined by:
    //
    //      lowbit(x) = x & -x
    //      lowbit(y) = y & -y
    //
    vector<vector<long long>> bit;

    // Store the actual array.
    //
    // This is technically not required for Fenwick itself,
    // but it allows us to implement:
    //
    //      get(x, y)
    //      update(x, y, value)
    //
    // efficiently.
    vector<vector<long long>> arr;


    // ========================================================
    //                       CONFIG
    // ========================================================

    long long MOD = 1e9 + 7;


    // --------------------------------------------------------
    // 1. IDENTITY
    // --------------------------------------------------------
    //
    // The value representing "nothing".
    //
    // SUM:
    //      0
    //
    // XOR:
    //      0
    //
    // PRODUCT:
    //      1
    //
    // MIN:
    //      +INF
    //
    // MAX:
    //      -INF
    //
    // --------------------------------------------------------

    long long identity() {
        return 0;
    }


    // --------------------------------------------------------
    // 2. COMBINE
    // --------------------------------------------------------
    //
    // How two Fenwick values are merged.
    //
    // SUM:
    //      a + b
    //
    // XOR:
    //      a ^ b
    //
    // PRODUCT:
    //      a * b
    //
    // MOD SUM:
    //      (a + b) % MOD
    //
    // MOD PRODUCT:
    //      (a * b) % MOD
    //
    // --------------------------------------------------------

    long long combine(long long a, long long b) {
        return a + b;
    }


    // --------------------------------------------------------
    // 3. INVERSE
    // --------------------------------------------------------
    //
    // Used when converting prefix queries into range queries.
    //
    // SUM:
    //      -a
    //
    // XOR:
    //      a
    //
    // MOD SUM:
    //      (MOD - a) % MOD
    //
    // PRODUCT:
    //      modInverse(a)
    //
    // --------------------------------------------------------

    long long inverse(long long a) {
        return -a;
    }


    // ========================================================
    //                         BUILD
    // ========================================================

    void build(const vector<vector<long long>>& input) {

        for (int x = 1; x <= n; x++) {
            for (int y = 1; y <= m; y++) {

                if (input[x][y] != identity()) {

                    add(x, y, input[x][y]);
                }
            }
        }
    }


    // ========================================================
    //                     PREFIX QUERY
    // ========================================================
    //
    // Returns:
    //
    //      aggregate of rectangle
    //
    //      (1,1) -> (x,y)
    //
    //
    // Example:
    //
    //      ┌───────────────┐
    //      │               │
    //      │   INCLUDED    │
    //      │               │
    //      └───────────────┘
    //       (1,1)       (x,y)
    //
    // ========================================================

    long long prefix(int x, int y) {

        long long res = identity();

        // Move downward in X.
        //
        // Same idea as 1D Fenwick:
        //
        //      x -= lowbit(x)
        //
        for (int i = x; i > 0; i -= (i & -i)) {

            // Move downward in Y.
            for (int j = y; j > 0; j -= (j & -j)) {

                res = combine(res, bit[i][j]);
            }
        }

        return res;
    }


    // ========================================================
    //                        POINT GET
    // ========================================================
    //
    // Returns the actual value at (x,y).
    //
    // O(1) because we keep a separate `arr`.
    //
    // ========================================================

    long long get(int x, int y) {

        return arr[x][y];
    }


    // ========================================================
    //                        POINT ADD
    // ========================================================
    //
    // Adds `delta` to a single point:
    //
    //      arr[x][y] += delta
    //
    //
    // Example:
    //
    //      add(3, 5, 10);
    //
    // means:
    //
    //      arr[3][5] += 10
    //
    // ========================================================

    void add(int x, int y, long long delta) {

        // Update all Fenwick nodes affected in X.
        for (int i = x; i <= n; i += (i & -i)) {

            // Update all Fenwick nodes affected in Y.
            for (int j = y; j <= m; j += (j & -j)) {

                bit[i][j] = combine(bit[i][j], delta);
            }
        }

        // Keep actual array synchronized.
        arr[x][y] = combine(arr[x][y], delta);
    }


public:

    // ========================================================
    //                       CONSTRUCTOR
    // ========================================================
    //
    // Creates an empty n × m Fenwick tree.
    //
    // Example:
    //
    //      FenwickTree2D ft(n, m);
    //
    // ========================================================

    FenwickTree2D(int n, int m)
        : n(n),
          m(m),
          bit(n + 1, vector<long long>(m + 1, identity())),
          arr(n + 1, vector<long long>(m + 1, identity())) {}


    // ========================================================
    //                          BUILD
    // ========================================================
    //
    // Build from an existing 2D array.
    //
    // The input MUST be 1-indexed:
    //
    //      input[1][1] ... input[n][m]
    //
    // ========================================================

    void init(const vector<vector<long long>>& input) {

        build(input);
    }


    // ========================================================
    //                     POINT ADD
    // ========================================================
    //
    //      arr[x][y] += delta
    //
    // ========================================================

    void point_add(int x, int y, long long delta) {

        add(x, y, delta);
    }


    // ========================================================
    //                     POINT UPDATE
    // ========================================================
    //
    // Safely sets:
    //
    //      arr[x][y] = value
    //
    //
    // Since Fenwick naturally supports ADD rather than SET,
    // calculate:
    //
    //      delta = new_value - old_value
    //
    // and then perform:
    //
    //      add(x, y, delta)
    //
    // ========================================================

    void update(int x, int y, long long value) {

        long long delta = combine(
            value,
            inverse(arr[x][y])
        );

        add(x, y, delta);
    }


    // ========================================================
    //                       POINT GET
    // ========================================================
    //
    // Returns:
    //
    //      arr[x][y]
    //
    // ========================================================

    long long get_value(int x, int y) {

        return get(x, y);
    }


    // ========================================================
    //                      PREFIX QUERY
    // ========================================================
    //
    // Public wrapper around prefix().
    //
    // Returns aggregate of:
    //
    //      [1..x] × [1..y]
    //
    // ========================================================

    long long prefix_query(int x, int y) {

        return prefix(x, y);
    }


    // ========================================================
    //                    RECTANGLE QUERY
    // ========================================================
    //
    // Returns aggregate of:
    //
    //      [x1..x2] × [y1..y2]
    //
    //
    // We use inclusion-exclusion.
    //
    //
    //      ┌─────────────────────┐
    //      │         A           │
    //      │                     │
    //      │       TARGET        │
    //      │                     │
    //      │                     │
    //      └─────────────────────┘
    //
    //
    // Formula:
    //
    //      P(x2,y2)
    //    - P(x1-1,y2)
    //    - P(x2,y1-1)
    //    + P(x1-1,y1-1)
    //
    // ========================================================

    long long query(
        int x1,
        int y1,
        int x2,
        int y2
    ) {

        return combine(
            combine(
                prefix(x2, y2),
                inverse(prefix(x1 - 1, y2))
            ),
            combine(
                inverse(prefix(x2, y1 - 1)),
                prefix(x1 - 1, y1 - 1)
            )
        );
    }
};

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, q; cin >> n >> q;
    vector<int> arr(n);
    for (int& x : arr) cin >> x;
    FenwickTree fw(arr);
    while (q--) {
        int c, l, r; cin >> c >> l >> r;
        if (c == 1) fw.update(l-1, r);
        else cout << fw.query(l-1, r-1) << '\n';
    }
}
