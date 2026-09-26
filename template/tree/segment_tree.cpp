#pragma once
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* ============================================================================
   UNIFIED SEGMENT TREE FAMILY (FLATTENED & OPTIMIZED)
   ============================================================================
   [MASTER MODIFICATION CHEAT SHEET]
   ---------------------------------------------------------------------------
   | Variant          | Node  | Tag      | op(a,b)   | id()      | apply(x, v, len)       | compose(old, v) |
   |------------------|-------|----------|-----------|-----------|------------------------|-----------------|
   | Add -> Sum       | ll    | ll       | a + b     | 0         | x += v * len           | old += v        |
   | Add -> Min       | ll    | ll       | min(a,b)  | LLONG_MAX | x += v                 | old += v        |
   | Set -> Sum       | ll    | opt<ll>  | a + b     | 0         | if(v) x = *v * len     | if(v) old = v   |
   | Affine -> Sum    | ll    | {a=1,b=0}| a + b     | 0         | x = v.a*x + v.b*len    | {v.a*old.a, ...}|
   | XOR -> Sum       | ll    | ll       | a + b     | 0         | if(len%2) x ^= v       | old ^= v        |
   ---------------------------------------------------------------------------
   [HOW TO USE]
   • SegTree<Policy, false>  : Static array (N <= 10^7).
   • SegTree<Policy, true>   : Dynamic/Sparse array (Coords up to 10^18).
   • MergeSortTree           : Offline 2D range counting, k-th smallest.
   • LiChaoTree<IsMax>       : Dynamic Convex Hull Trick.
   • SegTree2D<Policy>       : 2D Grid point updates, rectangle queries.
   • PersistentSegTree       : Historical queries.
   • SparseTable             : Static RMQ (O(1) query).
   ========================================================================= */

/* ==========================================================================
   PART 1: TRAITS (Configure the tree behavior)
   ========================================================================== */

struct SumAddTr {
    using Node = ll; using Tag = ll;
    static Node op(Node a, Node b) { return a + b; }
    static Node id() { return 0; }
    static Tag no_tag() { return 0; }
    static bool has_tag(const Tag& t) { return t != 0; }
    static void apply(Node& x, const Tag& v, ll len) { x += v * len; }
    static void compose(Tag& old, const Tag& v) { old += v; }
    static bool trivial(const Node&, const Tag&) { return false; }
    static bool fit(const Node&, const Tag&, ll) { return true; }
    static Node initial(ll, ll) { return 0; }
};

struct MinAddTr {
    using Node = ll; using Tag = ll;
    static Node op(Node a, Node b) { return min(a, b); }
    static Node id() { return LLONG_MAX; }
    static Tag no_tag() { return 0; }
    static bool has_tag(const Tag& t) { return t != 0; }
    static void apply(Node& x, const Tag& v, ll) { x += v; }
    static void compose(Tag& old, const Tag& v) { old += v; }
    static bool trivial(const Node&, const Tag&) { return false; }
    static bool fit(const Node&, const Tag&, ll) { return true; }
    static Node initial(ll, ll) { return LLONG_MAX; }
};

struct BeatsTr {
    struct Node { ll sum, mx, smx, mn, smn; int mxc, mnc; };
    struct Tag { ll add = 0; ll chmin = LLONG_MAX; ll chmax = LLONG_MIN; };

    static Node op(const Node& a, const Node& b) {
        Node r{a.sum + b.sum, max(a.mx, b.mx), LLONG_MIN, min(a.mn, b.mn), LLONG_MAX, 0, 0};
        if (a.mx == r.mx) r.mxc += a.mxc; else r.smx = max(r.smx, a.mx);
        if (b.mx == r.mx) r.mxc += b.mxc; else r.smx = max(r.smx, b.mx);
        if (a.mn == r.mn) r.mnc += a.mnc; else r.smn = min(r.smn, a.mn);
        if (b.mn == r.mn) r.mnc += b.mnc; else r.smn = min(r.smn, b.mn);
        return r;
    }
    static Node id() { return {0, LLONG_MIN, LLONG_MIN, LLONG_MAX, LLONG_MAX, 0, 0}; }
    static Tag no_tag() { return {}; }
    static bool has_tag(const Tag& t) { return t.add != 0 || t.chmin != LLONG_MAX || t.chmax != LLONG_MIN; }

    static void apply(Node& x, const Tag& v, ll len) {
        if (v.add != 0) { x.sum += v.add * len; x.mx += v.add; x.smx += v.add; x.mn += v.add; x.smn += v.add; }
        if (v.chmin != LLONG_MAX && x.mx > v.chmin) {
            x.sum -= (x.mx - v.chmin) * x.mxc;
            if (x.mn == x.mx) x.mn = v.chmin; if (x.smn == x.mx) x.smn = v.chmin;
            x.mx = v.chmin;
        }
        if (v.chmax != LLONG_MIN && x.mn < v.chmax) {
            x.sum += (v.chmax - x.mn) * x.mnc;
            if (x.mx == x.mn) x.mx = v.chmax; if (x.smx == x.mn) x.smx = v.chmax;
            x.mn = v.chmax;
        }
    }
    static void compose(Tag& old, const Tag& v) {
        if (v.add != 0) { old.add += v.add; if (old.chmin != LLONG_MAX) old.chmin += v.add; if (old.chmax != LLONG_MIN) old.chmax += v.add; }
        if (v.chmin != LLONG_MAX) old.chmin = min(old.chmin, v.chmin);
        if (v.chmax != LLONG_MIN) old.chmax = max(old.chmax, v.chmax);
    }
    static bool trivial(const Node& x, const Tag& v) {
        return (v.chmin != LLONG_MAX && x.mx <= v.chmin) || (v.chmax != LLONG_MIN && x.mn >= v.chmax);
    }
    static bool fit(const Node& x, const Tag& v, ll) {
        return (v.chmin != LLONG_MAX && x.smx < v.chmin) || (v.chmax != LLONG_MIN && x.smn > v.chmax);
    }
    static Node initial(ll, ll) { return {0, LLONG_MIN, LLONG_MIN, LLONG_MAX, LLONG_MAX, 0, 0}; }
};

/* ==========================================================================
   PART 2: UNIFIED SEGMENT TREE (Point, Range, Dynamic, Beats)
   ========================================================================== */
template <typename Policy, bool IsDyn = false>
class SegTree {
    using Node = typename Policy::Node;
    using Tag = typename Policy::Tag;
    struct Nd { Node v; Tag t; int lc = 0, rc = 0; };

    vector<Nd> t;
    ll L = 0, R = -1;
    int root = 1;

    int cl(int i) const { return IsDyn ? t[i].lc : (i << 1); }
    int cr(int i) const { return IsDyn ? t[i].rc : (i << 1 | 1); }

    int alloc(const Node& v, const Tag& tag, int lc = 0, int rc = 0) {
        t.push_back({v, tag, lc, rc});
        return (int)t.size() - 1;
    }

    void mk(int i, ll l, ll r) {
        if constexpr (!IsDyn) return;
        if (t[i].lc) return;
        ll m = (l + r) >> 1;
        t[i].lc = alloc(Policy::initial(l, m), Policy::no_tag());
        t[i].rc = alloc(Policy::initial(m + 1, r), Policy::no_tag());
    }

    void push(int i, ll l, ll r) {
        if (l == r || !Policy::has_tag(t[i].t)) return;
        mk(i, l, r);
        ll m = (l + r) >> 1;
        int li = cl(i), ri = cr(i);
        Policy::apply(t[li].v, t[i].t, m - l + 1);
        Policy::apply(t[ri].v, t[i].t, r - m);
        Policy::compose(t[li].t, t[i].t);
        Policy::compose(t[ri].t, t[i].t);
        t[i].t = Policy::no_tag();
    }

    void pull(int i) { t[i].v = Policy::op(t[cl(i)].v, t[cr(i)].v); }

    void bld(int i, ll l, ll r, const vector<Node>& a) {
        if (l == r) { t[i].v = a[l]; t[i].t = Policy::no_tag(); return; }
        ll m = (l + r) >> 1;
        bld(i << 1, l, m, a); bld(i << 1 | 1, m + 1, r, a);
        t[i].v = Policy::op(t[i << 1].v, t[i << 1 | 1].v);
        t[i].t = Policy::no_tag();
    }

    void upd(int i, ll l, ll r, ll ql, ll qr, const Tag& tg) {
        if (qr < l || r < ql) return;
        if (Policy::trivial(t[i].v, tg)) return;
        if (ql <= l && r <= qr && Policy::fit(t[i].v, tg, r - l + 1)) {
            Policy::apply(t[i].v, tg, r - l + 1);
            Policy::compose(t[i].t, tg);
            return;
        }
        if (l == r) return;
        mk(i, l, r); // CRITICAL: Materialize children before recursing
        push(i, l, r);
        ll m = (l + r) >> 1;
        upd(cl(i), l, m, ql, qr, tg);
        upd(cr(i), m + 1, r, ql, qr, tg);
        pull(i);
    }

    Node qry(int i, ll l, ll r, ll ql, ll qr) {
        if (!i || qr < l || r < ql) return Policy::id();
        if (ql <= l && r <= qr) return t[i].v;
        mk(i, l, r); // CRITICAL: Materialize children before querying
        push(i, l, r);
        ll m = (l + r) >> 1;
        return Policy::op(qry(cl(i), l, m, ql, qr), qry(cr(i), m + 1, r, ql, qr));
    }

    void upd_pt(int i, ll l, ll r, int p, const Node& v) {
        if (l == r) { t[i].v = v; t[i].t = Policy::no_tag(); return; }
        mk(i, l, r);
        push(i, l, r);
        ll m = (l + r) >> 1;
        if (p <= m) upd_pt(cl(i), l, m, p, v);
        else upd_pt(cr(i), m + 1, r, p, v);
        pull(i);
    }

    int find_first_dfs(int i, ll l, ll r, ll ql, Node& acc, auto& pred) {
        if (r < ql) return -1;
        if (ql <= l) {
            Node nxt = Policy::op(acc, t[i].v);
            if (!pred(nxt)) { acc = nxt; return -1; }
            if (l == r) return l;
        }
        mk(i, l, r);
        push(i, l, r);
        ll m = (l + r) >> 1;
        int res = find_first_dfs(cl(i), l, m, ql, acc, pred);
        if (res != -1) return res;
        return find_first_dfs(cr(i), m + 1, r, ql, acc, pred);
    }

public:
    SegTree(const vector<Node>& a) : L(0), R((ll)a.size() - 1) {
        int n = (int)a.size();
        t.resize(4 * max(n, 1));
        if (n) bld(1, 0, n - 1, a);
        root = 1;
    }

    SegTree(ll L_, ll R_) : L(L_), R(R_) {
        t.reserve(4000000); // Pre-allocate for dynamic trees to prevent TLE
        t.push_back({});
        root = alloc(Policy::initial(L, R), Policy::no_tag());
    }

    void update_point(int p, const Node& v) { upd_pt(root, L, R, p, v); }
    void update_range(ll l, ll r, const Tag& tg) { upd(root, L, R, l, r, tg); }
    Node query(ll l, ll r) { return qry(root, L, R, l, r); }

    template <typename P>
    int find_first(ll l, P pred) {
        Node acc = Policy::id();
        return find_first_dfs(root, L, R, l, acc, pred);
    }
};

/* ==========================================================================
   PART 3: MERGE SORT TREE
   ========================================================================== */
class MergeSortTree {
    int n; vector<vector<ll>> t;

    void build(int i, int l, int r, const vector<ll>& a) {
        if (l == r) { t[i] = {a[l]}; return; }
        int m = (l + r) >> 1;
        build(2*i, l, m, a); build(2*i+1, m+1, r, a);
        t[i].resize(r - l + 1);
        merge(t[2*i].begin(), t[2*i].end(), t[2*i+1].begin(), t[2*i+1].end(), t[i].begin());
    }
    int cnt(int i, int l, int r, int ql, int qr, ll v) const {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return (int)(upper_bound(t[i].begin(), t[i].end(), v) - t[i].begin());
        int m = (l + r) >> 1;
        return cnt(2*i, l, m, ql, qr, v) + cnt(2*i+1, m+1, r, ql, qr, v);
    }
public:
    MergeSortTree(const vector<ll>& a) : n((int)a.size()) {
        t.resize(4 * max(n, 1)); if (n) build(1, 0, n - 1, a);
    }
    int count_le(int l, int r, ll v) const { return cnt(1, 0, n - 1, l, r, v); }
    int count_lt(int l, int r, ll v) const { return count_le(l, r, v - 1); }
    ll kth(int l, int r, int k) const {
        ll lo = -2e18, hi = 2e18;
        while (lo < hi) { ll mid = lo + (hi - lo) / 2; if (count_le(l, r, mid) >= k) hi = mid; else lo = mid + 1; }
        return lo;
    }
};

/* ==========================================================================
   PART 4: LI CHAO TREE (Dynamic CHT)
   ========================================================================== */
template <bool IsMax = true>
class LiChaoTree {
    struct Line { ll m = 0, b = IsMax ? LLONG_MIN/4 : LLONG_MAX/4; ll eval(ll x) const { return m * x + b; } };
    struct Nd { Line ln; int lc = 0, rc = 0; };
    vector<Nd> t; ll xlo, xhi; int root = 0;

    int new_node() { t.push_back({}); return (int)t.size() - 1; }
    void insert(Line nw, int& i, ll l, ll r) {
        if (!i) { i = new_node(); t[i].ln = nw; return; }
        ll m = (l + r) >> 1;
        bool lef = IsMax ? (nw.eval(l) > t[i].ln.eval(l)) : (nw.eval(l) < t[i].ln.eval(l));
        bool mid = IsMax ? (nw.eval(m) > t[i].ln.eval(m)) : (nw.eval(m) < t[i].ln.eval(m));
        if (mid) swap(t[i].ln, nw);
        if (l == r) return;
        if (lef != mid) insert(nw, t[i].lc, l, m);
        else insert(nw, t[i].rc, m + 1, r);
    }
    ll qry(ll x, int i, ll l, ll r) const {
        if (!i) return IsMax ? LLONG_MIN/4 : LLONG_MAX/4;
        ll res = t[i].ln.eval(x);
        if (l == r) return res;
        ll m = (l + r) >> 1;
        return IsMax ? max(res, x <= m ? qry(x, t[i].lc, l, m) : qry(x, t[i].rc, m + 1, r))
                     : min(res, x <= m ? qry(x, t[i].lc, l, m) : qry(x, t[i].rc, m + 1, r));
    }
public:
    LiChaoTree(ll lo, ll hi) : xlo(lo), xhi(hi) { t.push_back({}); }
    void add_line(ll m, ll b) { insert({m, b}, root, xlo, xhi); }
    ll query(ll x) const { return qry(x, root, xlo, xhi); }
};
using LiChaoMax = LiChaoTree<true>;
using LiChaoMin = LiChaoTree<false>;

/* ==========================================================================
   PART 5: 2D SEGMENT TREE
   ========================================================================== */
template <typename Policy>
class SegTree2D {
    using Node = typename Policy::Node;
    int n, m; vector<vector<Node>> t; vector<vector<char>> inited;

    void build_y(int bi, int j, int l, int r, const vector<Node>& col) {
        if (l == r) { t[bi][j] = col[l]; inited[bi][j] = 1; return; }
        int mid = (l + r) >> 1;
        build_y(bi, 2*j, l, mid, col); build_y(bi, 2*j + 1, mid + 1, r, col);
        t[bi][j] = Policy::op(t[bi][2*j], t[bi][2*j + 1]);
    }
    void upd_y(int bi, int j, int l, int r, int y, const Node& v) {
        if (l == r) { t[bi][j] = v; return; }
        int mid = (l + r) >> 1;
        if (y <= mid) upd_y(bi, 2*j, l, mid, y, v); else upd_y(bi, 2*j + 1, mid + 1, r, y, v);
        t[bi][j] = Policy::op(t[bi][2*j], t[bi][2*j + 1]);
    }
    Node qry_y(int bi, int j, int l, int r, int ql, int qr) const {
        if (qr < l || r < ql) return Policy::id();
        if (ql <= l && r <= qr) return t[bi][j];
        int mid = (l + r) >> 1;
        return Policy::op(qry_y(bi, 2*j, l, mid, ql, qr), qry_y(bi, 2*j + 1, mid + 1, r, ql, qr));
    }
public:
    SegTree2D(int n_, int m_) : n(n_), m(m_) {
        t.assign(4 * max(n, 1), vector<Node>(4 * max(m, 1), Policy::id()));
        inited.assign(4 * max(n, 1), vector<char>(4 * max(m, 1), 0));
    }
    void update(int x, int y, const Node& v) {
        // Iterative-like recursive update for X
        function<void(int,int,int)> updx = [&](int i, int l, int r) {
            upd_y(i, 1, 0, m - 1, y, v);
            if (l == r) return;
            int mid = (l + r) >> 1;
            if (x <= mid) updx(2*i, l, mid); else updx(2*i + 1, mid + 1, r);
            for (int j = 1; j < 4 * m; j++) t[i][j] = Policy::op(t[2*i][j], t[2*i + 1][j]);
        };
        updx(1, 0, n - 1);
    }
    Node query(int x1, int y1, int x2, int y2) const {
        function<Node(int,int,int)> qryx = [&](int i, int l, int r) -> Node {
            if (x2 < l || r < x1) return Policy::id();
            if (x1 <= l && r <= x2) return qry_y(i, 1, 0, m - 1, y1, y2);
            int mid = (l + r) >> 1;
            return Policy::op(qryx(2*i, l, mid), qryx(2*i + 1, mid + 1, r));
        };
        return n ? qryx(1, 0, n - 1) : Policy::id();
    }
};

/* ==========================================================================
   PART 6: PERSISTENT SEGMENT TREE
   ========================================================================== */
template <typename Policy>
class PersistentSegTree {
    using Node = typename Policy::Node;
    struct Nd { Node val; int lc, rc; };
    vector<Nd> t; int n; vector<int> roots;

    int new_node(const Node& v, int lc, int rc) { t.push_back({v, lc, rc}); return (int)t.size() - 1; }
    int build(int l, int r, const vector<Node>& a) {
        if (l == r) return new_node(a[l], 0, 0);
        int m = (l + r) >> 1;
        int lc = build(l, m, a), rc = build(m + 1, r, a);
        return new_node(Policy::op(t[lc].val, t[rc].val), lc, rc);
    }
    int update(int prev, int l, int r, int p, const Node& v) {
        if (l == r) return new_node(v, 0, 0);
        int m = (l + r) >> 1, lc = t[prev].lc, rc = t[prev].rc;
        if (p <= m) lc = update(lc, l, m, p, v); else rc = update(rc, m + 1, r, p, v);
        return new_node(Policy::op(t[lc].val, t[rc].val), lc, rc);
    }
    Node qry(int i, int l, int r, int ql, int qr) const {
        if (!i || qr < l || r < ql) return Policy::id();
        if (ql <= l && r <= qr) return t[i].val;
        int m = (l + r) >> 1;
        return Policy::op(qry(t[i].lc, l, m, ql, qr), qry(t[i].rc, m + 1, r, ql, qr));
    }
public:
    PersistentSegTree() : n(0) {}
    PersistentSegTree(const vector<Node>& a) : n((int)a.size()) { if (n) roots.push_back(build(0, n - 1, a)); }
    int set(int ver, int p, const Node& v) {
        roots.push_back(update(roots[ver], 0, n - 1, p, v));
        return (int)roots.size() - 1;
    }
    Node query(int ver, int l, int r) const { return qry(roots[ver], 0, n - 1, l, r); }
};

/* ==========================================================================
   PART 7: SPARSE TABLE
   ========================================================================== */
template <typename Policy>
class SparseTable {
    int n; vector<vector<typename Policy::Node>> t; vector<int> lg;
public:
    SparseTable(const vector<typename Policy::Node>& a) : n((int)a.size()) {
        lg.assign(n + 1, 0);
        for (int i = 2; i <= n; i++) lg[i] = lg[i / 2] + 1;
        int K = n ? lg[n] + 1 : 1;
        t.assign(K, vector<typename Policy::Node>(max(n, 1), Policy::id()));
        if (n) t[0] = a;
        for (int k = 1; k < K; k++)
            for (int i = 0; i + (1 << k) <= n; i++)
                t[k][i] = Policy::op(t[k-1][i], t[k-1][i + (1 << (k-1))]);
    }
    typename Policy::Node query(int l, int r) const {
        int k = lg[r - l + 1];
        return Policy::op(t[k][l], t[k][r - (1 << k) + 1]);
    }
};
