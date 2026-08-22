#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
private:
    vector<long long> tree;
    int n;
    int height;

    // =================== CONFIG (The Cheat Sheet) ===================

    // --- 1. IDENTITY ---
    // SUM / XOR / GCD:  0
    // PROD / LCM:       1
    // MIN:              2e18  (Use a safe INF)
    // MAX:             -2e18  (Use a safe -INF)
    long long identity() { return 0; }

    // --- 2. COMBINE ---
    // SUM:  return a + b;
    // MIN:  return min(a, b);
    // MAX:  return max(a, b);
    // XOR:  return a ^ b;
    // PROD: return a * b; // Or (a * b) % MOD
    // GCD:  return std::gcd(a, b);
    // LCM:  return a / gcd(a, b) * b;
    // ALT:  return (level % 2 == 0) ? (a ^ b) : (a | b);
    long long combine(long long a, long long b, int level) {
        return a + b;
    }
    // ================================================================

    void build(const vector<int>& arr, int idx, int l, int r, int level) {
        if (l == r) { tree[idx] = arr[l]; return; }
        int m = (l + r) / 2;
        build(arr, idx * 2, l, m, level + 1);
        build(arr, idx * 2 + 1, m + 1, r, level + 1);
        tree[idx] = combine(tree[idx * 2], tree[idx * 2 + 1], level);
    }

    void update(int idx, int l, int r, int pos, long long val, int level) {
        if (l == r) { tree[idx] = val; return; } // Use tree[idx] += val for Point Add
        int m = (l + r) / 2;
        if (pos <= m) update(idx * 2, l, m, pos, val, level + 1);
        else update(idx * 2 + 1, m + 1, r, pos, val, level + 1);
        tree[idx] = combine(tree[idx * 2], tree[idx * 2 + 1], level);
    }

    long long query(int idx, int l, int r, int ql, int qr, int level) {
        if (qr < l || r < ql) return identity();
        if (ql <= l && r <= qr) return tree[idx];
        int m = (l + r) / 2;
        return combine(query(idx * 2, l, m, ql, qr, level + 1),
                       query(idx * 2 + 1, m + 1, r, ql, qr, level + 1), level);
    }

public:
    SegmentTree(const vector<int>& arr) {
        n = arr.size();
        height = 0;
        while ((1 << height) < n) ++height;
        tree.assign(4 * n, identity());
        if (n > 0) build(arr, 1, 0, n - 1, 0);
    }

    void update(int pos, long long val) { update(1, 0, n - 1, pos, val, 0); }
    long long query(int l, int r) { return query(1, 0, n - 1, l, r, 0); }
};

class LazySegmentTree {
private:
    vector<long long> tree, lazy;
    vector<bool> has_lazy;
    int n;
    int height;

    // =================== CONFIG (The Cheat Sheet) ===================

    // --- 1. IDENTITIES ---
    // SUM / XOR:   0
    // MIN:         2e18
    // MAX:        -2e18
    long long identity() { return 0; }

    // LAZY_ADD:    0
    // LAZY_SET:    2e18 (Must be a value outside your input constraints)
    long long lazy_identity() { return 0; }

    // --- 2. COMBINE (For query & pushup) ---
    // SUM: return a + b;
    // MIN: return min(a, b);
    // MAX: return max(a, b);
    long long combine(long long a, long long b, int level) {
        return a + b;
    }

    // --- 3. APPLY LAZY (For update & pushdown) ---
    // [ADD to SUM]: tree[idx] += val * len; if(len > 1){ lazy[idx] += val; has_lazy[idx] = true; }
    // [SET to SUM]: tree[idx] = val * len;  if(len > 1){ lazy[idx] = val;  has_lazy[idx] = true; }
    // [ADD to MIN]: tree[idx] += val;       if(len > 1){ lazy[idx] += val; has_lazy[idx] = true; }
    // [SET to MIN]: tree[idx] = val;        if(len > 1){ lazy[idx] = val;  has_lazy[idx] = true; }
    // [XOR Range]:  tree[idx] ^= (len % 2 == 1 ? val : 0); if(len > 1){ lazy[idx] ^= val; has_lazy[idx] = true; }
    void apply(int idx, long long val, int len, int level) {
        tree[idx] += val * len;
        if (len > 1) {
            lazy[idx] += val;
            has_lazy[idx] = true;
        }
    }
    // ======================================================================

    void push(int idx, int l, int r, int level) {
        if (!has_lazy[idx]) return;
        int m = (l + r) / 2;
        apply(idx * 2, lazy[idx], m - l + 1, level + 1);
        apply(idx * 2 + 1, lazy[idx], r - m, level + 1);
        lazy[idx] = lazy_identity();
        has_lazy[idx] = false;
    }

    void build(const vector<int>& arr, int idx, int l, int r, int level) {
        if (l == r) { tree[idx] = arr[l]; return; }
        int m = (l + r) / 2;
        build(arr, idx * 2, l, m, level + 1);
        build(arr, idx * 2 + 1, m + 1, r, level + 1);
        tree[idx] = combine(tree[idx * 2], tree[idx * 2 + 1], level);
    }

    void update(int idx, int l, int r, int ql, int qr, long long val, int level) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { apply(idx, val, r - l + 1, level); return; }
        push(idx, l, r, level);
        int m = (l + r) / 2;
        update(idx * 2, l, m, ql, qr, val, level + 1);
        update(idx * 2 + 1, m + 1, r, ql, qr, val, level + 1);
        tree[idx] = combine(tree[idx * 2], tree[idx * 2 + 1], level);
    }

    long long query(int idx, int l, int r, int ql, int qr, int level) {
        if (qr < l || r < ql) return identity();
        if (ql <= l && r <= qr) return tree[idx];
        push(idx, l, r, level);
        int m = (l + r) / 2;
        return combine(query(idx * 2, l, m, ql, qr, level + 1),
                       query(idx * 2 + 1, m + 1, r, ql, qr, level + 1), level);
    }

public:
    LazySegmentTree(const vector<int>& arr) {
        n = arr.size();
        height = 0;
        while ((1 << height) < n) ++height;
        tree.assign(4 * n, identity());
        lazy.assign(4 * n, lazy_identity());
        has_lazy.assign(4 * n, false);
        if (n > 0) build(arr, 1, 0, n - 1, 0);
    }

    void update(int l, int r, long long val) { update(1, 0, n - 1, l, r, val, 0); }
    long long query(int l, int r) { return query(1, 0, n - 1, l, r, 0); }
};

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, q; cin >> n >> q;
    vector<int> arr(n);
    for (int& x : arr) cin >> x;
    SegmentTree st(arr);
    while (q--) {
        int c, l, r; cin >> c >> l >> r;
        if (c == 1) st.update(l-1, r);
        else cout << st.query(l-1, r-1) << '\n';
    }
}
