#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

const uint64_t MOD = (1ULL << 61) - 1;
const uint64_t BASE_R = 10007;
const uint64_t BASE_C = 10009;

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

uint64_t sub_mod(uint64_t a, uint64_t b) {
    return a >= b ? a - b : a + MOD - b;
}

struct GridHash {
    int R, C;
    vector<vector<uint64_t>> H;
    vector<uint64_t> PR, PC;

    GridHash(const vector<string>& grid) {
        R = grid.size();
        C = grid[0].size();
        H.assign(R + 1, vector<uint64_t>(C + 1, 0));
        PR.assign(R + 1, 1);
        PC.assign(C + 1, 1);

        for (int i = 1; i <= R; i++) PR[i] = mul_mod(PR[i - 1], BASE_R);
        for (int j = 1; j <= C; j++) PC[j] = mul_mod(PC[j - 1], BASE_C);

        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                uint64_t term = add_mod(mul_mod(H[i][j + 1], BASE_R), mul_mod(H[i + 1][j], BASE_C));
                term = sub_mod(term, mul_mod(H[i][j], mul_mod(BASE_R, BASE_C)));
                H[i + 1][j + 1] = add_mod(term, (uint64_t)grid[i][j]);
            }
        }
    }

    // 0-indexed subgrid query: (r1, c1) top-left to (r2, c2) bottom-right
    uint64_t get(int r1, int c1, int r2, int c2) {
        int len_r = r2 - r1 + 1;
        int len_c = c2 - c1 + 1;
        uint64_t res = H[r2 + 1][c2 + 1];
        res = sub_mod(res, mul_mod(H[r1][c2 + 1], PR[len_r]));
        res = sub_mod(res, mul_mod(H[r2 + 1][c1], PC[len_c]));
        res = add_mod(res, mul_mod(H[r1][c1], mul_mod(PR[len_r], PC[len_c])));
        return res;
    }
};

int main() {
    vector<string> big_grid = {
        "abcde",
        "fghij",
        "klmno",
        "pqrst"
    };

    vector<string> pattern = {
        "gh",
        "lm"
    };

    GridHash gh_big(big_grid);
    GridHash gh_pat(pattern);

    uint64_t target_hash = gh_pat.get(0, 0, 1, 1);

    cout << "Searching 2x2 pattern in 4x5 grid:\n";
    for (int r = 0; r <= 4 - 2; r++) {
        for (int c = 0; c <= 5 - 2; c++) {
            if (gh_big.get(r, c, r + 1, c + 1) == target_hash) {
                cout << "Match found at row: " << r << ", col: " << c << "\n";
            }
        }
    }

    return 0;
}
