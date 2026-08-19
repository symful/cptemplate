#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
private:
    vector<int> tree;
    int n;

    // =================== CONFIG (Change these 2) ===================
    // SUM:   return a + b;          ID: 0
    // MIN:   return min(a, b);      ID: INT_MAX
    // MAX:   return max(a, b);      ID: INT_MIN
    // XOR:   return a ^ b;          ID: 0
    // PROD:  return a * b;          ID: 1
    // GCD:   return gcd(a,b);  ID: 0
    int combine(int a, int b) { return a + b; }
    int identity() { return 0; }
    // ===============================================================

    void build(vector<int>& arr, int idx, int l, int r) {
        if (l == r) { tree[idx] = arr[l]; return; }
        int m = (l + r) / 2;
        build(arr, idx*2, l, m);
        build(arr, idx*2+1, m+1, r);
        tree[idx] = combine(tree[idx*2], tree[idx*2+1]);
    }

    int query(int idx, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return identity();
        if (ql <= l && r <= qr) return tree[idx];
        int m = (l + r) / 2;
        return combine(query(idx*2, l, m, ql, qr),
                       query(idx*2+1, m+1, r, ql, qr));
    }

    void update(int idx, int l, int r, int pos, int val) {
        if (l == r) { tree[idx] = val; return; }
        int m = (l + r) / 2;
        if (pos <= m) update(idx*2, l, m, pos, val);
        else update(idx*2+1, m+1, r, pos, val);
        tree[idx] = combine(tree[idx*2], tree[idx*2+1]);
    }

public:
    SegmentTree(vector<int>& arr) {
        n = arr.size();
        tree.resize(4*n, identity());
        build(arr, 1, 0, n-1);
    }
    int query(int l, int r) { return query(1, 0, n-1, l, r); }
    void update(int pos, int val) { update(1, 0, n-1, pos, val); }
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
