#include <bits/stdc++.h>
using namespace std;

class FenwickTree {
private:
    vector<int> bit;
    int n;

    // =================== CONFIG (Change these 3) ===================
    // Operation          combine(a,b)   identity()   inverse(a)
    // -------------------------------------------------------------
    // SUM:                 a + b          0            -a
    // XOR:                 a ^ b          0            a
    // PRODUCT (int):       a * b          1            1/a
    // MOD PRODUCT (1e9+7): a*b%MOD        1            modInverse(a)
    // ===============================================================
    int combine(int a, int b) { return a + b; }
    int identity() { return 0; }
    int inverse(int a) { return -a; }
    // ===============================================================

    void build(vector<int>& arr) {
        for (int i = 1; i <= n; i++) {
            bit[i] = combine(bit[i], arr[i-1]);
            int p = i + (i & -i);
            if (p <= n) bit[p] = combine(bit[p], bit[i]);
        }
    }

    int prefix(int pos) {
        int idx = pos + 1, res = identity();
        while (idx > 0) {
            res = combine(res, bit[idx]);
            idx -= (idx & -idx);
        }
        return res;
    }

    int get(int pos) {
        return combine(prefix(pos), inverse(prefix(pos - 1)));
    }

public:
    FenwickTree(vector<int>& arr) {
        n = arr.size();
        bit.resize(n + 1, identity());
        build(arr);
    }

    void add(int pos, int delta) {
        int idx = pos + 1;
        while (idx <= n) {
            bit[idx] = combine(bit[idx], delta);
            idx += (idx & -idx);
        }
    }

    void update(int pos, int val) {
        int delta = combine(val, inverse(get(pos)));
        add(pos, delta);
    }

    int query(int l, int r) {
        return combine(prefix(r), inverse(prefix(l - 1)));
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
