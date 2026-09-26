#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* ============================================================================
   FENWICK (BIT) FAMILY
   ----------------------------------------------------------------------------
   1. BIT        : point add, prefix / range query, lower_bound
   2. BITRange   : range add, range sum
   3. BIT2D      : 2D point add, rectangle sum
   ========================================================================= */


/* ----------------------------------------------------------------------------
   1. BIT — point add, prefix / range query
   ------------------------------------------------------------------------- */
template <typename T = ll>
struct BIT {
    int n;
    vector<T> b;

    BIT(int n = 0) : n(n), b(n + 1, T{}) {}

    BIT(const vector<T>& a) : n((int)a.size()), b(n + 1, T{}) {
        for (int i = 0; i < n; i++) add(i, a[i]);
    }

    // add d to position i (0-indexed)
    void add(int i, T d) {
        for (++i; i <= n; i += i & -i) b[i] += d;
    }

    // sum of [0..i]
    T prefix(int i) const {
        T r{};
        for (++i; i > 0; i -= i & -i) r += b[i];
        return r;
    }

    // sum of [l..r] inclusive; returns T{} if l > r
    T range(int l, int r) const {
        return l > r ? T{} : prefix(r) - (l ? prefix(l - 1) : T{});
    }

    // Smallest index p in [0, n-1] with prefix(p) >= target.
    // Assumes all entries are non-negative. Returns n if none.
    int lower_bound(T target) const {
        int idx = 0, step = 1;
        while ((step << 1) <= n) step <<= 1;
        T cur{};
        for (; step; step >>= 1) {
            if (idx + step <= n && cur + b[idx + step] < target) {
                idx += step;
                cur += b[idx];
            }
        }
        return idx;   // 0-based; prefix(idx) >= target
    }
};


/* ----------------------------------------------------------------------------
   2. BITRange — range add, range sum
   ------------------------------------------------------------------------- */
template <typename T = ll>
struct BITRange {
    int n;
    vector<T> b1, b2;

    BITRange(int n = 0) : n(n), b1(n + 1, T{}), b2(n + 1, T{}) {}

    void _add(vector<T>& b, int i, T v) {
        for (++i; i <= n; i += i & -i) b[i] += v;
    }

    T _sum(const vector<T>& b, int i) const {
        T r{};
        for (++i; i > 0; i -= i & -i) r += b[i];
        return r;
    }

    // add v to every element in [l..r]
    void range_add(int l, int r, T v) {
        _add(b1, l, v);         _add(b1, r + 1, -v);
        _add(b2, l, v * (l - 1)); _add(b2, r + 1, -v * r);
    }

    // sum of [0..i]
    T prefix(int i) const {
        return _sum(b1, i) * i - _sum(b2, i);
    }

    // sum of [l..r] inclusive
    T range(int l, int r) const {
        return prefix(r) - (l ? prefix(l - 1) : T{});
    }
};


/* ----------------------------------------------------------------------------
   3. BIT2D — point add, rectangle sum (0-indexed)
   ------------------------------------------------------------------------- */
template <typename T = ll>
struct BIT2D {
    int n, m;
    vector<vector<T>> b;

    BIT2D(int n = 0, int m = 0)
        : n(n), m(m), b(n + 1, vector<T>(m + 1, T{})) {}

    // add v at (x, y)
    void add(int x, int y, T v) {
        for (int i = x + 1; i <= n; i += i & -i)
            for (int j = y + 1; j <= m; j += j & -j)
                b[i][j] += v;
    }

    // sum of [0..x] x [0..y]
    T prefix(int x, int y) const {
        T r{};
        for (int i = x + 1; i > 0; i -= i & -i)
            for (int j = y + 1; j > 0; j -= j & -j)
                r += b[i][j];
        return r;
    }

    // sum of [x1..x2] x [y1..y2]
    T range(int x1, int y1, int x2, int y2) const {
        if (x1 > x2 || y1 > y2) return T{};
        T r = prefix(x2, y2);
        if (x1) r -= prefix(x1 - 1, y2);
        if (y1) r -= prefix(x2, y1 - 1);
        if (x1 && y1) r += prefix(x1 - 1, y1 - 1);
        return r;
    }
};

/* ============================================================================
   USAGE
   ----------------------------------------------------------------------------
   vector<ll> a = {1,2,3,4,5};
   BIT<ll> bit(a);
   bit.add(0, 10);                 // a[0] += 10
   cout << bit.range(0, 2);        // 10+2+3 = 15
   cout << bit.lower_bound(16);    // first prefix >= 16

   BITRange<ll> br(5);
   br.range_add(0, 4, 3);          // all +3
   cout << br.range(1, 3);         // 9

   BIT2D<ll> f2(100, 100);
   f2.add(10, 20, 5);
   cout << f2.range(0, 0, 50, 50); // 5
   ========================================================================= */
