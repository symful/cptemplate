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

template <typename T>
class RMQSegmentTree {
public:
    struct Node {
        T val;
        int idx;
    };

private:
    vector<Node> tree;
    int n;

    // =================== CONFIG (The Cheat Sheet) ===================

    // --- 1. IDENTITY ---
    // Range MAX: Set to negative infinity (or safe min value), idx = -1
    // Range MIN: Set to positive infinity (or safe max value), idx = -1
    Node identity() {
        return {numeric_limits<T>::lowest(), -1};
    }

    // --- 2. COMBINE ---
    // Range MAX: return (a.val >= b.val) ? a : b;
    // Range MIN: return (a.val <= b.val) ? a : b;
    Node combine(const Node& a, const Node& b) {
        if (a.idx == -1) return b;
        if (b.idx == -1) return a;
        return (a.val >= b.val) ? a : b; // Default is Range MAX
    }
    // ================================================================

    void build(const vector<T>& arr, int idx, int l, int r) {
        if (l == r) {
            tree[idx] = {arr[l], l};
            return;
        }
        int m = l + (r - l) / 2;
        build(arr, idx * 2, l, m);
        build(arr, idx * 2 + 1, m + 1, r);
        tree[idx] = combine(tree[idx * 2], tree[idx * 2 + 1]);
    }

    void update(int idx, int l, int r, int pos, T val) {
        if (l == r) {
            tree[idx] = {val, pos};
            return;
        }
        int m = l + (r - l) / 2;
        if (pos <= m) update(idx * 2, l, m, pos, val);
        else update(idx * 2 + 1, m + 1, r, pos, val);
        tree[idx] = combine(tree[idx * 2], tree[idx * 2 + 1]);
    }

    Node query(int idx, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return identity();
        if (ql <= l && r <= qr) return tree[idx];
        int m = l + (r - l) / 2;
        return combine(query(idx * 2, l, m, ql, qr),
                       query(idx * 2 + 1, m + 1, r, ql, qr));
    }

public:
    RMQSegmentTree() : n(0) {}

    RMQSegmentTree(const vector<T>& arr) {
        n = arr.size();
        tree.assign(4 * n, identity());
        if (n > 0) build(arr, 1, 0, n - 1);
    }

    // Point update at 0-indexed pos
    void update(int pos, T val) {
        update(1, 0, n - 1, pos, val);
    }

    // Range query [l, r] inclusive (0-indexed)
    Node query(int l, int r) {
        return query(1, 0, n - 1, l, r);
    }

    // Helper accessors
    T query_val(int l, int r) { return query(l, r).val; }
    int query_idx(int l, int r) { return query(l, r).idx; }
};

class FenwickTree2D {
private:
    int n, m;                     // n = rows, m = cols
    vector<vector<long long>> bit; // bit[col+1][row+1] (1‑based internally)

    long long identity() { return 0; }
    long long combine(long long a, long long b) { return a + b; }
    long long inverse(long long a) { return -a; }

    // Add delta at (x, y) – 0‑based column x, row y
    void add(int x, int y, long long delta) {
        for (int i = x + 1; i <= m; i += (i & -i))
            for (int j = y + 1; j <= n; j += (j & -j))
                bit[i][j] = combine(bit[i][j], delta);
    }

    // Prefix sum [0..x] × [0..y]
    long long prefix(int x, int y) {
        long long res = identity();
        for (int i = x + 1; i > 0; i -= (i & -i))
            for (int j = y + 1; j > 0; j -= (j & -j))
                res = combine(res, bit[i][j]);
        return res;
    }

public:
    // Constructor: input is 0‑indexed [row][col] = arr[y][x]
    FenwickTree2D(const vector<vector<long long>>& input) {
        n = input.size();
        m = (n > 0 ? input[0].size() : 0);
        bit.assign(m + 1, vector<long long>(n + 1, identity()));
        for (int y = 0; y < n; ++y)
            for (int x = 0; x < m; ++x)
                if (input[y][x] != identity())
                    add(x, y, input[y][x]);
    }

    // Point value at (x, y) – computed from BIT
    long long get(int x, int y) {
        return combine(
            combine(prefix(x, y), inverse(prefix(x - 1, y))),
            combine(inverse(prefix(x, y - 1)), prefix(x - 1, y - 1))
        );
    }

    // Set arr[y][x] = val
    void update(int x, int y, long long val) {
        long long delta = combine(val, inverse(get(x, y)));
        add(x, y, delta);
    }

    // Add val to arr[y][x]
    void point_add(int x, int y, long long val) {
        add(x, y, val);
    }

    // Prefix query [0..x] × [0..y]
    long long prefix_query(int x, int y) {
        return prefix(x, y);
    }

    // Range query: inclusive rectangle [x1..x2] × [y1..y2]
    long long query(int x1, int y1, int x2, int y2) {
        return combine(
            combine(prefix(x2, y2), inverse(prefix(x1 - 1, y2))),
            combine(inverse(prefix(x2, y1 - 1)), prefix(x1 - 1, y1 - 1))
        );
    }
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
