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
