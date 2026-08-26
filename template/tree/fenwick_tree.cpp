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

class FenwickTree2D {

private:

    // Actual dimensions of the array.
    int n, m;

    // Fenwick tree storage.
    //
    // The actual array is 0-indexed:
    //
    //      arr[y][x]
    //
    // while the Fenwick tree itself is internally 1-indexed:
    //
    //      bit[x + 1][y + 1]
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
    //
    // IMPORTANT:
    //      arr is 0-indexed.
    vector<vector<long long>> arr;


    // ================================================================
    // CONFIG (The Cheat Sheet)
    // ================================================================

    long long MOD = 1e9 + 7; // Useful for modular operations


    // --- 1. IDENTITY ---
    // SUM / XOR / MOD_SUM: 0
    // PROD / MOD_PROD:     1
    //
    // For MIN:
    //      +INF
    //
    // For MAX:
    //      -INF
    //
    long long identity() {
        return 0;
    }


    // --- 2. COMBINE ---
    // SUM:      return a + b;
    // XOR:      return a ^ b;
    // PROD:     return a * b;
    // MOD_SUM:  return (a + b) % MOD;
    // MOD_PROD: return (a * b) % MOD;
    //
    long long combine(long long a, long long b) {
        return a + b;
    }


    // --- 3. INVERSE (Crucial for Range Queries) ---
    // SUM:      return -a;
    // XOR:      return a; (XOR is its own inverse)
    // MOD_SUM:  return (MOD - a) % MOD;
    // MOD_PROD: return modInverse(a);
    //           (Requires binary exponentiation:
    //            binpow(a, MOD - 2))
    //
    long long inverse(long long a) {
        return -a;
    }


    // ================================================================
    // BUILD
    // ================================================================

    void build(const vector<vector<long long>>& input) {

        for (int y = 0; y < n; y++) {
            for (int x = 0; x < m; x++) {

                if (input[y][x] != identity()) {
                    add(x, y, input[y][x]);
                }
            }
        }
    }


    // ================================================================
    // PREFIX
    // ================================================================

    long long prefix(int x, int y) {

        long long res = identity();

        // Convert 0-indexed coordinates to 1-indexed Fenwick
        // coordinates.
        //
        // Same idea as the 1D version:
        //
        //      pos + 1
        //
        for (int i = x + 1; i > 0; i -= (i & -i)) {

            for (int j = y + 1; j > 0; j -= (j & -j)) {

                res = combine(res, bit[i][j]);
            }
        }

        return res;
    }


    // ================================================================
    // GET
    // ================================================================

    long long get(int x, int y) {

        return arr[y][x];
    }


    // ================================================================
    // ADD
    // ================================================================

    void add(int x, int y, long long delta) {

        // Convert 0-indexed coordinates to 1-indexed Fenwick
        // coordinates.
        //
        // Same idea as the 1D version:
        //
        //      pos + 1
        //
        for (int i = x + 1; i <= m; i += (i & -i)) {

            for (int j = y + 1; j <= n; j += (j & -j)) {

                bit[i][j] = combine(bit[i][j], delta);
            }
        }

        // Keep the actual array synchronized.
        arr[y][x] = combine(arr[y][x], delta);
    }


public:

    // ================================================================
    // CONSTRUCTOR
    // ================================================================
    //
    // Same behavior as the original 1D FenwickTree.
    //
    // The input array is 0-indexed:
    //
    //      arr[y][x]
    //
    // Example:
    //
    //      vector<vector<int>> arr(n, vector<int>(m));
    //      FenwickTree2D ft(arr);
    //
    // ================================================================

    FenwickTree2D(vector<vector<long long>>& input) {

        n = input.size();

        m = (n > 0 ? input[0].size() : 0);

        bit.assign(
            m + 1,
            vector<long long>(n + 1, identity())
        );

        arr.assign(
            n,
            vector<long long>(m, identity())
        );

        if (n > 0 && m > 0) {
            build(input);
        }
    }


    // ================================================================
    // POINT UPDATE (Safely sets arr[y][x] = val)
    // ================================================================
    //
    // Same behavior as the original 1D version.
    //
    // Since Fenwick naturally supports ADD rather than SET,
    // calculate:
    //
    //      delta = new_value - old_value
    //
    // and apply that delta.
    //
    // ================================================================

    void update(int x, int y, long long val) {

        long long delta = combine(
            val,
            inverse(get(x, y))
        );

        add(x, y, delta);
    }


    // ================================================================
    // POINT ADD (arr[y][x] += val)
    // ================================================================
    //
    // Faster than update() if you already know the delta.
    //
    // ================================================================

    void point_add(int x, int y, long long val) {

        add(x, y, val);
    }


    // ================================================================
    // PREFIX QUERY
    // ================================================================
    //
    // Returns the aggregate of:
    //
    //      [0..x] × [0..y]
    //
    // Example:
    //
    //      prefix(2, 3)
    //
    // means:
    //
    //      arr[0][0] ... arr[2][3]
    //
    // ================================================================

    long long prefix_query(int x, int y) {

        return prefix(x, y);
    }


    // ================================================================
    // RANGE QUERY [x1..x2] × [y1..y2]
    // ================================================================
    //
    // Returns the aggregate inside the inclusive rectangle:
    //
    //      x1 <= x <= x2
    //      y1 <= y <= y2
    //
    //
    // Since prefix() gives:
    //
    //      [0..x] × [0..y]
    //
    // we use inclusion-exclusion:
    //
    //
    //      P(x2, y2)
    //    - P(x1-1, y2)
    //    - P(x2, y1-1)
    //    + P(x1-1, y1-1)
    //
    // ================================================================

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
