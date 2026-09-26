#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* ============================================================================
   SEGMENT TREE FAMILY
   ----------------------------------------------------------------------------
   1.  Traits library        (monoid & lazy traits)
   2.  SegTree<Tr>           (point update, range query, find_first)
   3.  LazySegTree<Tr>       (range update, range query)
   4.  DynSegTree<Tr>        (huge coordinate range, sparse nodes)
   5.  PersistentSegTree<Tr> (per-version queries)
   6.  SegTree2D<Tr>         (2D point update, rectangle query)
   7.  MergeSortTree         (count <= X, k-th smallest)
   8.  SegTreeBeats          (chmin / chmax / add / sum / min / max)
   9.  LiChaoMax / LiChaoMin (dynamic line container)
   10. SparseTable           (static idempotent RMQ)
   ========================================================================= */


/* ============================================================================
   PART 1.  TRAITS LIBRARY
   ========================================================================= */

/* ---- Monoid traits (SegTree, DynSegTree, PersistentSegTree, SegTree2D) ---- */
struct SumMonoid { using Node = ll; static Node id() { return 0; }         static Node op(Node a, Node b) { return a + b; } };
struct MinMonoid { using Node = ll; static Node id() { return LLONG_MAX; } static Node op(Node a, Node b) { return min(a, b); } };
struct MaxMonoid { using Node = ll; static Node id() { return LLONG_MIN; } static Node op(Node a, Node b) { return max(a, b); } };
struct GcdMonoid { using Node = ll; static Node id() { return 0; }         static Node op(Node a, Node b) { return gcd(a, b); } };
struct XorMonoid { using Node = ll; static Node id() { return 0; }         static Node op(Node a, Node b) { return a ^ b; } };

/* ---- Lazy traits (LazySegTree) ---- */

struct SumAdd {           // range add, range sum
    using Node = ll; using Lazy = ll;
    static Node id() { return 0; }              static Lazy lid() { return 0; }
    static Node op(Node a, Node b) { return a + b; }
    static void apply(Node& x, Lazy v, int len) { x += v * len; }
    static void compose(Lazy& old, Lazy v) { old += v; }
};

struct MinAdd {           // range add, range min
    using Node = ll; using Lazy = ll;
    static Node id() { return LLONG_MAX; }      static Lazy lid() { return 0; }
    static Node op(Node a, Node b) { return min(a, b); }
    static void apply(Node& x, Lazy v, int len) { x += v; }
    static void compose(Lazy& old, Lazy v) { old += v; }
};

struct MaxAdd {           // range add, range max
    using Node = ll; using Lazy = ll;
    static Node id() { return LLONG_MIN; }      static Lazy lid() { return 0; }
    static Node op(Node a, Node b) { return max(a, b); }
    static void apply(Node& x, Lazy v, int len) { x += v; }
    static void compose(Lazy& old, Lazy v) { old += v; }
};

struct SumSet {           // range set, range sum
    using Node = ll; using Lazy = optional<ll>;
    static Node id() { return 0; }              static Lazy lid() { return nullopt; }
    static Node op(Node a, Node b) { return a + b; }
    static void apply(Node& x, Lazy v, int len) { if (v) x = *v * len; }
    static void compose(Lazy& old, Lazy v) { if (v) old = v; }
};

struct SumAffine {        // range affine (a*x+b), range sum
    struct Lazy { ll a = 1, b = 0; };
    using Node = ll;
    static Node id() { return 0; }              static Lazy lid() { return {}; }
    static Node op(Node a, Node b) { return a + b; }
    static void apply(Node& x, Lazy v, int len) { x = v.a * x + v.b * len; }
    static void compose(Lazy& old, Lazy v) { old = { v.a * old.a, v.a * old.b + v.b }; }
};


/* ============================================================================
   PART 2.  GENERIC SEGMENT TREE  (point update, range query, find_first)
   ========================================================================= */
template <typename Traits>
class SegTree {
    using Node = typename Traits::Node;
    int n;
    vector<Node> t;

    void build(int i, int l, int r, const vector<Node>& a) {
        if (l == r) { t[i] = a[l]; return; }
        int m = (l + r) >> 1;
        build(2*i, l, m, a); build(2*i+1, m+1, r, a);
        t[i] = Traits::op(t[2*i], t[2*i+1]);
    }
    void upd(int i, int l, int r, int p, const Node& v) {
        if (l == r) { t[i] = v; return; }
        int m = (l + r) >> 1;
        if (p <= m) upd(2*i, l, m, p, v);
        else        upd(2*i+1, m+1, r, p, v);
        t[i] = Traits::op(t[2*i], t[2*i+1]);
    }
    Node qry(int i, int l, int r, int ql, int qr) const {
        if (qr < l || r < ql) return Traits::id();
        if (ql <= l && r <= qr) return t[i];
        int m = (l + r) >> 1;
        return Traits::op(qry(2*i, l, m, ql, qr), qry(2*i+1, m+1, r, ql, qr));
    }
    template <typename P>
    int find_first(int i, int l, int r, int ql, Node& acc, P& pred) const {
        if (r < ql) return n;
        if (ql <= l) {
            Node nxt = Traits::op(acc, t[i]);
            if (!pred(nxt)) { acc = nxt; return n; }
            if (l == r) { acc = nxt; return l; }
        }
        int m = (l + r) >> 1;
        int res = find_first(2*i, l, m, ql, acc, pred);
        if (res != n) return res;
        return find_first(2*i+1, m+1, r, ql, acc, pred);
    }
public:
    SegTree() : n(0) {}
    SegTree(const vector<Node>& a) : n((int)a.size()) {
        t.assign(4 * max(n, 1), Traits::id());
        if (n) build(1, 0, n - 1, a);
    }
    void update(int p, const Node& v) { upd(1, 0, n - 1, p, v); }
    Node query(int l, int r) const { return qry(1, 0, n - 1, l, r); }

    // Leftmost index i >= l such that op(a[l..i]) satisfies pred.
    template <typename P>
    int find_first(int l, P pred) const {
        if (n == 0 || l >= n) return n;
        Node acc = Traits::id();
        return find_first(1, 0, n - 1, l, acc, pred);
    }
};


/* ============================================================================
   PART 3.  GENERIC LAZY SEGMENT TREE
   ========================================================================= */
template <typename Traits>
class LazySegTree {
    using Node = typename Traits::Node;
    using Lazy = typename Traits::Lazy;
    int n;
    vector<Node> t;
    vector<Lazy> lz;
    vector<char> has;

    void build(int i, int l, int r, const vector<Node>& a) {
        if (l == r) { t[i] = a[l]; return; }
        int m = (l + r) >> 1;
        build(2*i, l, m, a); build(2*i+1, m+1, r, a);
        t[i] = Traits::op(t[2*i], t[2*i+1]);
    }
    void apply_lazy(int i, const Lazy& v, int len) {
        Traits::apply(t[i], v, len);
        if (len > 1) { Traits::compose(lz[i], v); has[i] = 1; }
    }
    void push(int i, int l, int r) {
        if (!has[i]) return;
        int m = (l + r) >> 1;
        apply_lazy(2*i, lz[i], m - l + 1);
        apply_lazy(2*i+1, lz[i], r - m);
        lz[i] = Traits::lid(); has[i] = 0;
    }
    void upd(int i, int l, int r, int ql, int qr, const Lazy& v) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { apply_lazy(i, v, r - l + 1); return; }
        push(i, l, r);
        int m = (l + r) >> 1;
        upd(2*i, l, m, ql, qr, v);
        upd(2*i+1, m+1, r, ql, qr, v);
        t[i] = Traits::op(t[2*i], t[2*i+1]);
    }
    Node qry(int i, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return Traits::id();
        if (ql <= l && r <= qr) return t[i];
        push(i, l, r);
        int m = (l + r) >> 1;
        return Traits::op(qry(2*i, l, m, ql, qr), qry(2*i+1, m+1, r, ql, qr));
    }
public:
    LazySegTree() : n(0) {}
    LazySegTree(const vector<Node>& a) : n((int)a.size()) {
        t.assign(4 * max(n, 1), Traits::id());
        lz.assign(4 * max(n, 1), Traits::lid());
        has.assign(4 * max(n, 1), 0);
        if (n) build(1, 0, n - 1, a);
    }
    void update(int l, int r, const Lazy& v) { upd(1, 0, n - 1, l, r, v); }
    Node query(int l, int r) { return qry(1, 0, n - 1, l, r); }
};


/* ============================================================================
   PART 4.  DYNAMIC / SPARSE SEGMENT TREE
   ========================================================================= */
template <typename Traits>
class DynSegTree {
    using Node = typename Traits::Node;
    struct Nd { Node val; int lc = 0, rc = 0; };
    vector<Nd> t;
    ll L, R;
    int root = 0;

    int new_node() { t.push_back({Traits::id(), 0, 0}); return (int)t.size() - 1; }

    void upd(int& i, ll l, ll r, ll p, const Node& v) {
        if (!i) i = new_node();
        if (l == r) { t[i].val = v; return; }
        ll m = l + (r - l) / 2;
        if (p <= m) upd(t[i].lc, l, m, p, v);
        else        upd(t[i].rc, m + 1, r, p, v);
        Node a = t[i].lc ? t[t[i].lc].val : Traits::id();
        Node b = t[i].rc ? t[t[i].rc].val : Traits::id();
        t[i].val = Traits::op(a, b);
    }
    Node qry(int i, ll l, ll r, ll ql, ll qr) const {
        if (!i || qr < l || r < ql) return Traits::id();
        if (ql <= l && r <= qr) return t[i].val;
        ll m = l + (r - l) / 2;
        return Traits::op(qry(t[i].lc, l, m, ql, qr), qry(t[i].rc, m + 1, r, ql, qr));
    }
public:
    DynSegTree(ll L, ll R) : L(L), R(R) { t.push_back({Traits::id(), 0, 0}); }
    void update(ll p, const Node& v) { upd(root, L, R, p, v); }
    Node query(ll l, ll r) const { return qry(root, L, R, l, r); }
};


/* ============================================================================
   PART 5.  PERSISTENT SEGMENT TREE
   ========================================================================= */
template <typename Traits>
class PersistentSegTree {
    using Node = typename Traits::Node;
    struct Nd { Node val; int lc, rc; };
    vector<Nd> t;
    int n;
    vector<int> roots;

    int new_node(const Node& v, int lc, int rc) { t.push_back({v, lc, rc}); return (int)t.size() - 1; }

    int build(int l, int r, const vector<Node>& a) {
        if (l == r) return new_node(a[l], 0, 0);
        int m = (l + r) >> 1;
        int lc = build(l, m, a), rc = build(m + 1, r, a);
        return new_node(Traits::op(t[lc].val, t[rc].val), lc, rc);
    }
    int update(int prev, int l, int r, int p, const Node& v) {
        if (l == r) return new_node(v, 0, 0);
        int m = (l + r) >> 1, lc = t[prev].lc, rc = t[prev].rc;
        if (p <= m) lc = update(lc, l, m, p, v);
        else        rc = update(rc, m + 1, r, p, v);
        return new_node(Traits::op(t[lc].val, t[rc].val), lc, rc);
    }
    Node qry(int i, int l, int r, int ql, int qr) const {
        if (!i || qr < l || r < ql) return Traits::id();
        if (ql <= l && r <= qr) return t[i].val;
        int m = (l + r) >> 1;
        return Traits::op(qry(t[i].lc, l, m, ql, qr), qry(t[i].rc, m + 1, r, ql, qr));
    }
public:
    PersistentSegTree() : n(0) {}
    PersistentSegTree(const vector<Node>& a) : n((int)a.size()) {
        if (n) roots.push_back(build(0, n - 1, a));
    }
    // Create new version from `ver` with a[p] = v; returns new version id.
    int set(int ver, int p, const Node& v) {
        roots.push_back(update(roots[ver], 0, n - 1, p, v));
        return (int)roots.size() - 1;
    }
    Node query(int ver, int l, int r) const { return qry(roots[ver], 0, n - 1, l, r); }
};


/* ============================================================================
   PART 6.  2D SEGMENT TREE  (point update, rectangle query)
             memory O(4n * 4m)
   ========================================================================= */
template <typename Traits>
class SegTree2D {
    using Node = typename Traits::Node;
    int n, m;
    vector<vector<Node>> t;
    vector<vector<char>> inited;

    void build_y(int bi, int j, int l, int r, const vector<Node>& col) {
        if (l == r) { t[bi][j] = col[l]; inited[bi][j] = 1; return; }
        int mid = (l + r) >> 1;
        build_y(bi, 2*j, l, mid, col);
        build_y(bi, 2*j + 1, mid + 1, r, col);
        t[bi][j] = Traits::op(t[bi][2*j], t[bi][2*j + 1]);
    }
    void upd_y(int bi, int j, int l, int r, int y, const Node& v) {
        if (l == r) { t[bi][j] = v; return; }
        int mid = (l + r) >> 1;
        if (y <= mid) upd_y(bi, 2*j, l, mid, y, v);
        else          upd_y(bi, 2*j + 1, mid + 1, r, y, v);
        t[bi][j] = Traits::op(t[bi][2*j], t[bi][2*j + 1]);
    }
    Node qry_y(int bi, int j, int l, int r, int ql, int qr) const {
        if (qr < l || r < ql) return Traits::id();
        if (ql <= l && r <= qr) return t[bi][j];
        int mid = (l + r) >> 1;
        return Traits::op(qry_y(bi, 2*j, l, mid, ql, qr),
                          qry_y(bi, 2*j + 1, mid + 1, r, ql, qr));
    }
public:
    SegTree2D(int n, int m) : n(n), m(m) {
        t.assign(4 * max(n, 1), vector<Node>(4 * max(m, 1), Traits::id()));
        inited.assign(4 * max(n, 1), vector<char>(4 * max(m, 1), 0));
    }
    // grid[x][y]
    SegTree2D(const vector<vector<Node>>& grid)
        : n((int)grid.size()), m(n ? (int)grid[0].size() : 0) {
        t.assign(4 * max(n, 1), vector<Node>(4 * max(m, 1), Traits::id()));
        inited.assign(4 * max(n, 1), vector<char>(4 * max(m, 1), 0));
        function<void(int,int,int)> buildx = [&](int i, int l, int r) {
            if (l == r) { build_y(i, 1, 0, m - 1, grid[l]); return; }
            int mid = (l + r) >> 1;
            buildx(2*i, l, mid); buildx(2*i + 1, mid + 1, r);
            for (int j = 1; j < 4 * m; j++)
                if (inited[2*i][j] || inited[2*i + 1][j])
                    t[i][j] = Traits::op(t[2*i][j], t[2*i + 1][j]);
        };
        if (n) buildx(1, 0, n - 1);
    }
    void update(int x, int y, const Node& v) {
        function<void(int,int,int)> updx = [&](int i, int l, int r) {
            upd_y(i, 1, 0, m - 1, y, v);
            if (l == r) return;
            int mid = (l + r) >> 1;
            if (x <= mid) updx(2*i, l, mid);
            else          updx(2*i + 1, mid + 1, r);
            for (int j = 1; j < 4 * m; j++)
                t[i][j] = Traits::op(t[2*i][j], t[2*i + 1][j]);
        };
        updx(1, 0, n - 1);
    }
    Node query(int x1, int y1, int x2, int y2) const {
        function<Node(int,int,int)> qryx = [&](int i, int l, int r) -> Node {
            if (x2 < l || r < x1) return Traits::id();
            if (x1 <= l && r <= x2) return qry_y(i, 1, 0, m - 1, y1, y2);
            int mid = (l + r) >> 1;
            return Traits::op(qryx(2*i, l, mid), qryx(2*i + 1, mid + 1, r));
        };
        return n ? qryx(1, 0, n - 1) : Traits::id();
    }
};


/* ============================================================================
   PART 7.  MERGE SORT TREE  (range count <= X, k-th smallest)
   ========================================================================= */
class MergeSortTree {
    int n;
    vector<vector<ll>> t;
    void build(int i, int l, int r, const vector<ll>& a) {
        if (l == r) { t[i] = {a[l]}; return; }
        int m = (l + r) >> 1;
        build(2*i, l, m, a); build(2*i+1, m+1, r, a);
        t[i].resize(r - l + 1);
        merge(t[2*i].begin(), t[2*i].end(),
              t[2*i+1].begin(), t[2*i+1].end(),
              t[i].begin());
    }
    int cnt(int i, int l, int r, int ql, int qr, ll v) const {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr)
            return (int)(upper_bound(t[i].begin(), t[i].end(), v) - t[i].begin());
        int m = (l + r) >> 1;
        return cnt(2*i, l, m, ql, qr, v) + cnt(2*i+1, m+1, r, ql, qr, v);
    }
public:
    MergeSortTree(const vector<ll>& a) : n((int)a.size()) {
        t.resize(4 * max(n, 1));
        if (n) build(1, 0, n - 1, a);
    }
    int count_le(int l, int r, ll v) const { return cnt(1, 0, n - 1, l, r, v); }
    int count_lt(int l, int r, ll v) const { return count_le(l, r, v - 1); }
    // k-th smallest in [l, r], 1-indexed k. O(log^2 n).
    ll kth(int l, int r, int k) const {
        ll lo = -1e18, hi = 1e18;
        while (lo < hi) {
            ll mid = lo + (hi - lo) / 2;
            if (count_le(l, r, mid) >= k) hi = mid;
            else                           lo = mid + 1;
        }
        return lo;
    }
};


/* ============================================================================
   PART 8.  SEGMENT TREE BEATS
            Supports: chmin, chmax, add, sum/min/max over ranges.
   ========================================================================= */
class SegTreeBeats {
    struct Nd {
        ll sum = 0;
        ll mx = LLONG_MIN, smx = LLONG_MIN; int mxc = 0;
        ll mn = LLONG_MAX, smn = LLONG_MAX; int mnc = 0;
        ll add = 0;
    };
    int n;
    vector<Nd> t;

    Nd merge(const Nd& a, const Nd& b) {
        Nd r;
        r.sum = a.sum + b.sum;
        if (a.mx > b.mx)      { r.mx = a.mx; r.mxc = a.mxc; r.smx = max(a.smx, b.mx); }
        else if (a.mx < b.mx) { r.mx = b.mx; r.mxc = b.mxc; r.smx = max(a.mx, b.smx); }
        else                  { r.mx = a.mx; r.mxc = a.mxc + b.mxc; r.smx = max(a.smx, b.smx); }
        if (a.mn < b.mn)      { r.mn = a.mn; r.mnc = a.mnc; r.smn = min(a.smn, b.mn); }
        else if (a.mn > b.mn) { r.mn = b.mn; r.mnc = b.mnc; r.smn = min(a.mn, b.smn); }
        else                  { r.mn = a.mn; r.mnc = a.mnc + b.mnc; r.smn = min(a.smn, b.smn); }
        return r;
    }
    void apply_add(int i, ll v, int len) {
        t[i].sum += v * len;
        t[i].mx += v; if (t[i].smx != LLONG_MIN) t[i].smx += v;
        t[i].mn += v; if (t[i].smn != LLONG_MAX) t[i].smn += v;
        t[i].add += v;
    }
    void apply_chmin(int i, ll v) {
        if (v >= t[i].mx) return;
        t[i].sum -= (t[i].mx - v) * t[i].mxc;
        if (t[i].mn == t[i].mx) t[i].mn = v;
        if (t[i].smn == t[i].mx) t[i].smn = v;
        t[i].mx = v;
    }
    void apply_chmax(int i, ll v) {
        if (v <= t[i].mn) return;
        t[i].sum += (v - t[i].mn) * t[i].mnc;
        if (t[i].mx == t[i].mn) t[i].mx = v;
        if (t[i].smx == t[i].mn) t[i].smx = v;
        t[i].mn = v;
    }
    void push(int i, int l, int r) {
        int m = (l + r) >> 1;
        ll mx = t[i].mx, mn = t[i].mn;
        if (t[i].add) {
            apply_add(2*i, t[i].add, m - l + 1);
            apply_add(2*i+1, t[i].add, r - m);
            t[i].add = 0;
        }
        if (mx < t[2*i].mx)   apply_chmin(2*i, mx);
        if (mx < t[2*i+1].mx) apply_chmin(2*i+1, mx);
        if (mn > t[2*i].mn)   apply_chmax(2*i, mn);
        if (mn > t[2*i+1].mn) apply_chmax(2*i+1, mn);
    }
    void build(int i, int l, int r, const vector<ll>& a) {
        if (l == r) { t[i] = {a[l], a[l], LLONG_MIN, 1, a[l], LLONG_MAX, 1, 0}; return; }
        int m = (l + r) >> 1;
        build(2*i, l, m, a); build(2*i+1, m+1, r, a);
        t[i] = merge(t[2*i], t[2*i+1]);
    }
    void up_chmin(int i, int l, int r, int ql, int qr, ll v) {
        if (qr < l || r < ql || v >= t[i].mx) return;
        if (ql <= l && r <= qr && v > t[i].smx) { apply_chmin(i, v); return; }
        push(i, l, r);
        int m = (l + r) >> 1;
        up_chmin(2*i, l, m, ql, qr, v); up_chmin(2*i+1, m+1, r, ql, qr, v);
        t[i] = merge(t[2*i], t[2*i+1]);
    }
    void up_chmax(int i, int l, int r, int ql, int qr, ll v) {
        if (qr < l || r < ql || v <= t[i].mn) return;
        if (ql <= l && r <= qr && v < t[i].smn) { apply_chmax(i, v); return; }
        push(i, l, r);
        int m = (l + r) >> 1;
        up_chmax(2*i, l, m, ql, qr, v); up_chmax(2*i+1, m+1, r, ql, qr, v);
        t[i] = merge(t[2*i], t[2*i+1]);
    }
    void up_add(int i, int l, int r, int ql, int qr, ll v) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { apply_add(i, v, r - l + 1); return; }
        push(i, l, r);
        int m = (l + r) >> 1;
        up_add(2*i, l, m, ql, qr, v); up_add(2*i+1, m+1, r, ql, qr, v);
        t[i] = merge(t[2*i], t[2*i+1]);
    }
    ll q_sum(int i, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return t[i].sum;
        push(i, l, r);
        int m = (l + r) >> 1;
        return q_sum(2*i, l, m, ql, qr) + q_sum(2*i+1, m+1, r, ql, qr);
    }
    ll q_max(int i, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return LLONG_MIN;
        if (ql <= l && r <= qr) return t[i].mx;
        push(i, l, r);
        int m = (l + r) >> 1;
        return max(q_max(2*i, l, m, ql, qr), q_max(2*i+1, m+1, r, ql, qr));
    }
    ll q_min(int i, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return LLONG_MAX;
        if (ql <= l && r <= qr) return t[i].mn;
        push(i, l, r);
        int m = (l + r) >> 1;
        return min(q_min(2*i, l, m, ql, qr), q_min(2*i+1, m+1, r, ql, qr));
    }
public:
    SegTreeBeats(const vector<ll>& a) : n((int)a.size()) {
        t.resize(4 * max(n, 1));
        if (n) build(1, 0, n - 1, a);
    }
    void chmin(int l, int r, ll v) { up_chmin(1, 0, n - 1, l, r, v); }
    void chmax(int l, int r, ll v) { up_chmax(1, 0, n - 1, l, r, v); }
    void add  (int l, int r, ll v) { up_add  (1, 0, n - 1, l, r, v); }
    ll sum(int l, int r) { return q_sum(1, 0, n - 1, l, r); }
    ll max(int l, int r) { return q_max(1, 0, n - 1, l, r); }
    ll min(int l, int r) { return q_min(1, 0, n - 1, l, r); }
};


/* ============================================================================
   PART 9.  LI CHAO TREE  (max of lines; Min variant included)
   ========================================================================= */
class LiChaoMax {
    struct Line { ll m = 0, b = LLONG_MIN / 4; ll eval(ll x) const { return m * x + b; } };
    struct Nd { Line ln; int lc = 0, rc = 0; };
    vector<Nd> t;
    ll xlo, xhi;
    int root = 0;

    int new_node() { t.push_back({}); return (int)t.size() - 1; }
    void insert(Line nw, int& i, ll l, ll r) {
        if (!i) { i = new_node(); t[i].ln = nw; return; }
        ll m = (l + r) >> 1;
        bool lef = nw.eval(l) > t[i].ln.eval(l);
        bool mid = nw.eval(m) > t[i].ln.eval(m);
        if (mid) swap(t[i].ln, nw);
        if (l == r) return;
        if (lef != mid) insert(nw, t[i].lc, l, m);
        else            insert(nw, t[i].rc, m + 1, r);
    }
    ll qry(ll x, int i, ll l, ll r) const {
        if (!i) return LLONG_MIN / 4;
        ll res = t[i].ln.eval(x);
        if (l == r) return res;
        ll m = (l + r) >> 1;
        if (x <= m) return max(res, qry(x, t[i].lc, l, m));
        return max(res, qry(x, t[i].rc, m + 1, r));
    }
public:
    LiChaoMax(ll lo, ll hi) : xlo(lo), xhi(hi) { t.push_back({}); }
    void add_line(ll m, ll b) { insert({m, b}, root, xlo, xhi); }
    ll query(ll x) const { return qry(x, root, xlo, xhi); }
};

class LiChaoMin {
    LiChaoMax inner;
public:
    LiChaoMin(ll lo, ll hi) : inner(lo, hi) {}
    void add_line(ll m, ll b) { inner.add_line(-m, -b); }
    ll query(ll x) const { return -inner.query(x); }
};


/* ============================================================================
   PART 10.  SPARSE TABLE  (static, idempotent op, O(1) query)
   ========================================================================= */
template <typename T, typename Comb>
class SparseTable {
    int n;
    vector<vector<T>> t;
    vector<int> lg;
    Comb cmb;
    T id;
public:
    SparseTable(const vector<T>& a, T identity, Comb combine)
        : n((int)a.size()), cmb(combine), id(identity) {
        lg.assign(n + 1, 0);
        for (int i = 2; i <= n; i++) lg[i] = lg[i / 2] + 1;
        int K = n ? lg[n] + 1 : 1;
        t.assign(K, vector<T>(max(n, 1), id));
        if (n) t[0] = a;
        for (int k = 1; k < K; k++)
            for (int i = 0; i + (1 << k) <= n; i++)
                t[k][i] = cmb(t[k-1][i], t[k-1][i + (1 << (k-1))]);
    }
    T query(int l, int r) const {
        int k = lg[r - l + 1];
        return cmb(t[k][l], t[k][r - (1 << k) + 1]);
    }
};


/* ============================================================================
   USAGE
   ----------------------------------------------------------------------------
   vector<ll> a = {1,2,3,4,5};

   // SegTree (range sum)
   SegTree<SumMonoid> st_sum(a);
   st_sum.query(1, 3);                              // 9
   st_sum.find_first(0, [](ll s){ return s >= 6; });// 3

   // SegTree (range max)
   SegTree<MaxMonoid> st_max(a);
   st_max.query(0, 4);                              // 5

   // LazySegTree (range add, range sum)
   LazySegTree<SumAdd> lst(a);
   lst.update(1, 3, 10);
   lst.query(0, 4);                                 // 45

   // LazySegTree (range affine, range sum)
   LazySegTree<SumAffine> affine(a);
   affine.update(0, 4, {2, 1});                     // x -> 2x+1
   affine.query(0, 4);                              // 35

   // Dynamic segtree on [1, 1e18]
   DynSegTree<SumMonoid> dyn(1, (ll)1e18);
   dyn.update(1000000000000LL, 7);
   dyn.query(1, (ll)1e18);                          // 7

   // Persistent
   PersistentSegTree<SumMonoid> pst(a);
   int v1 = pst.set(0, 0, 100);
   int v2 = pst.set(v1, 4, 200);
   pst.query(v2, 0, 4);                             // 115

   // MergeSortTree
   MergeSortTree mst(a);
   mst.count_le(0, 4, 3);                           // 3
   mst.kth(0, 4, 3);                                // 3

   // Beats
   SegTreeBeats beats(a);
   beats.chmin(0, 4, 3);
   beats.sum(0, 4);                                 // 12

   // Li Chao
   LiChaoMax lichao(-1000, 1000);
   lichao.add_line(1, 0);
   lichao.add_line(-1, 10);
   lichao.query(3);                                 // 7

   // Sparse table
   SparseTable<ll, function<ll(ll,ll)>> sp(a, LLONG_MIN,
       [](ll x, ll y){ return max(x, y); });
   sp.query(1, 3);                                  // 4
   ========================================================================= */
