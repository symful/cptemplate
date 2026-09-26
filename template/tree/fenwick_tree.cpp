#pragma once
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* ============================================================================
   UNIFIED FENWICK (BIT) FAMILY (100% COVERAGE)
   ============================================================================
   EXACTLY ONE TOP-LEVEL CLASS: `Fenwick`.

   [MODIFICATION TABLE: WHICH TEMPLATE ARGS TO USE]
   ---------------------------------------------------------------------------
   | Variant                | Template Arguments          | Update Method               | Query Method               |
   |------------------------|-----------------------------|-----------------------------|----------------------------|
   | 1D Point Add, Range Sum| Fenwick::Core<ll, 1, false> | add(x, v)                   | range_query(l, r)          |
   | 1D Range Add, Range Sum| Fenwick::Core<ll, 1, true>  | range_add(l, r, v)          | range_query(l, r)          |
   | 2D Point Add, Rect Sum | Fenwick::Core<ll, 2, false> | add(x, y, v)                | range_query(x1,y1, x2,y2)  |
   | 2D Range Add, Rect Sum | Fenwick::Core<ll, 2, true>  | range_add(x1,y1, x2,y2, v)  | range_query(x1,y1, x2,y2)  |
   ---------------------------------------------------------------------------
   [HOW TO MODIFY]
   • Sum -> XOR: Change `+=` to `^=`, `-=` to `^=`, and `T{}` to `0`.
     (Note: Range Add/Query does NOT work for XOR, use Point only).
   • Custom Structs: Overload `operator+=` and `operator-=` for your struct.
   ============================================================================ */

class Fenwick {
public:
    template <typename T = ll, int Dim = 1, bool IsRange = false>
    class Core {
        int n = 0, m = 1;
        vector<vector<T>> t1, t2, t3, t4;

        void init(int n_, int m_) {
            n = n_; m = max(1, m_);
            t1.assign(n + 1, vector<T>(m, T{}));
            if constexpr (IsRange) {
                t2.assign(n + 1, vector<T>(m, T{}));
                if constexpr (Dim == 2) {
                    t3.assign(n + 1, vector<T>(m, T{}));
                    t4.assign(n + 1, vector<T>(m, T{}));
                }
            }
        }

        void _add(vector<vector<T>>& t, int x, int y, T v) {
            for (int i = x; i <= n; i += i & -i)
                for (int j = y; j <= m; j += j & -j)
                    t[i][j] += v;
        }

        T _sum(const vector<vector<T>>& t, int x, int y) const {
            T res = T{};
            for (int i = x; i > 0; i -= i & -i)
                for (int j = y; j > 0; j -= j & -j)
                    res += t[i][j];
            return res;
        }

    public:
        Core(int n_, int m_ = 1) { init(n_, m_); }

        Core(const vector<vector<T>>& grid) {
            init(grid.size(), grid.empty() ? 1 : grid[0].size());
            if constexpr (!IsRange) {
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < m; j++)
                        if (grid[i][j] != T{}) add(i + 1, j + 1, grid[i][j]);
            }
        }

        // Point add (1D: y is ignored, 2D: y is actual y)
        void add(int x, int y, T v) {
            if constexpr (!IsRange) _add(t1, x, y, v);
        }

        // Range add: [x1, x2] x [y1, y2]
        void range_add(int x1, int y1, int x2, int y2, T v) {
            if constexpr (IsRange) {
                // Clean 2D/1D unified math using 4 BITs
                auto add_pt = [&](int x, int y, T val) {
                    _add(t1, x, y, val);
                    _add(t2, x, y, val * (x - 1));
                    if constexpr (Dim == 2) {
                        _add(t3, x, y, val * (y - 1));
                        _add(t4, x, y, val * (x - 1) * (y - 1));
                    }
                };
                auto sub_pt = [&](int x, int y, T val) { add_pt(x, y, -val); };

                add_pt(x1, y1, v); sub_pt(x2 + 1, y1, v);
                sub_pt(x1, y2 + 1, v); add_pt(x2 + 1, y2 + 1, v);
            }
        }

        // Prefix query: [1, x] x [1, y]
        T prefix(int x, int y) const {
            if constexpr (!IsRange) {
                return _sum(t1, x, y);
            } else {
                T res = _sum(t1, x, y) * x * y - _sum(t2, x, y) * x;
                if constexpr (Dim == 2) {
                    res -= _sum(t3, x, y) * y;
                    res += _sum(t4, x, y);
                }
                return res;
            }
        }

        // 1D Convenience Wrappers
        void add(int x, T v) { if constexpr (Dim == 1) add(x, 1, v); }
        void range_add(int l, int r, T v) { if constexpr (Dim == 1) range_add(l, 1, r, 1, v); }

        T range_query(int x1, int y1, int x2, int y2) const {
            if constexpr (Dim == 2) {
                T res = prefix(x2, y2);
                if (x1 > 1) res -= prefix(x1 - 1, y2);
                if (y1 > 1) res -= prefix(x2, y1 - 1);
                if (x1 > 1 && y1 > 1) res += prefix(x1 - 1, y1 - 1);
                return res;
            } else {
                return prefix(x2, 1) - (x1 > 1 ? prefix(x1 - 1, 1) : T{});
            }
        }
        T range_query(int l, int r) const { if constexpr (Dim == 1) return range_query(l, 1, r, 1); else return T{}; }
    };

    // Type aliases for easy copy-pasting
    template <typename T = ll> using Point1D = Core<T, 1, false>;
    template <typename T = ll> using Range1D = Core<T, 1, true>;
    template <typename T = ll> using Point2D = Core<T, 2, false>;
    template <typename T = ll> using Range2D = Core<T, 2, true>;
};
