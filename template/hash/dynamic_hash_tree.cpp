#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>

using namespace std;

const uint64_t MOD = (1ULL << 61) - 1;

uint64_t mul_mod(uint64_t a, uint64_t b) {
    uint64_t l1 = (uint32_t)a, h1 = a >> 32, l2 = (uint32_t)b, h2 = b >> 32;
    uint64_t l = l1 * l2, m = l1 * h2 + l2 * h1, h = h1 * h2;
    uint64_t ret = (l & MOD) + (l >> 61) + (h << 3) + (m >> 29) + (m << 35 >> 3) + 1;
    ret = (ret & MOD) + (ret >> 61);
    return ret - 1;
}

uint64_t add_mod(uint64_t a, uint64_t b) {
    uint64_t res = a + b;
    return (res >= MOD) ? res - MOD : res;
}

struct DynamicHashTree {
    struct Node {
        uint64_t hash = 0;
        int len = 0;
    };

    int n;
    uint64_t BASE;
    vector<uint64_t> P;
    vector<Node> tree;

    Node merge(const Node& left, const Node& right) {
        return {
            add_mod(mul_mod(left.hash, P[right.len]), right.hash),
            left.len + right.len
        };
    }

    DynamicHashTree(const string& s) {
        n = s.size();
        mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
        BASE = 10007 + (rng() % 50000);
        if (BASE % 2 == 0) BASE++;

        P.assign(n + 1, 1);
        for (int i = 1; i <= n; i++) P[i] = mul_mod(P[i - 1], BASE);

        tree.resize(4 * n);
        build(1, 0, n - 1, s);
    }

    void build(int node, int l, int r, const string& s) {
        if (l == r) {
            tree[node] = {(uint64_t)s[l], 1};
            return;
        }
        int mid = (l + r) / 2;
        build(node * 2, l, mid, s);
        build(node * 2 + 1, mid + 1, r, s);
        tree[node] = merge(tree[node * 2], tree[node * 2 + 1]);
    }

    void update(int node, int l, int r, int idx, char c) {
        if (l == r) {
            tree[node] = {(uint64_t)c, 1};
            return;
        }
        int mid = (l + r) / 2;
        if (idx <= mid) update(node * 2, l, mid, idx, c);
        else update(node * 2 + 1, mid + 1, r, idx, c);
        tree[node] = merge(tree[node * 2], tree[node * 2 + 1]);
    }

    Node query(int node, int l, int r, int ql, int qr) {
        if (ql <= l && r <= qr) return tree[node];
        int mid = (l + r) / 2;
        if (qr <= mid) return query(node * 2, l, mid, ql, qr);
        if (ql > mid) return query(node * 2 + 1, mid + 1, r, ql, qr);
        return merge(query(node * 2, l, mid, ql, qr), query(node * 2 + 1, mid + 1, r, ql, qr));
    }

    void set_char(int idx, char c) { update(1, 0, n - 1, idx, c); }
    uint64_t get_hash(int l, int r) { return query(1, 0, n - 1, l, r).hash; }
};

int main() {
    string s = "bananas";
    DynamicHashTree dht(s);

    // Hash of "ana" at [1..3] and [3..5]
    cout << "Original 'ana' == 'ana': " << (dht.get_hash(1, 3) == dht.get_hash(3, 5) ? "TRUE" : "FALSE") << "\n";

    // Change index 3 from 'a' -> 'x' -> string becomes "banxnas"
    dht.set_char(3, 'x');
    cout << "After update, [1..3] ('anx') == [3..5] ('xna'): "
         << (dht.get_hash(1, 3) == dht.get_hash(3, 5) ? "TRUE" : "FALSE") << "\n";

    return 0;
}
