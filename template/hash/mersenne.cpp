#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>

using namespace std;

// Modulo 2^61 - 1
const uint64_t MOD = (1ULL << 61) - 1;

// Fast multiplication modulo 2^61 - 1
uint64_t mul_mod(uint64_t a, uint64_t b) {
    uint64_t l1 = (uint32_t)a, h1 = a >> 32, l2 = (uint32_t)b, h2 = b >> 32;
    uint64_t l = l1 * l2, m = l1 * h2 + l2 * h1, h = h1 * h2;
    uint64_t ret = (l & MOD) + (l >> 61) + (h << 3) + (m >> 29) + (m << 35 >> 3) + 1;
    ret = (ret & MOD) + (ret >> 61);
    return ret - 1;
}

uint64_t add_mod(uint64_t a, uint64_t b) {
    uint64_t res = a + b;
    if (res >= MOD) res -= MOD;
    return res;
}

uint64_t sub_mod(uint64_t a, uint64_t b) {
    return a >= b ? a - b : a + MOD - b;
}

struct MersenneHash {
    static uint64_t BASE;
    static vector<uint64_t> P;
    vector<uint64_t> H;

    static void init_powers(int max_len) {
        if (P.empty()) {
            mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
            BASE = 300 + (rng() % 100000);
            if (BASE % 2 == 0) BASE++;
            P = {1};
        }
        while ((int)P.size() <= max_len) {
            P.push_back(mul_mod(P.back(), BASE));
        }
    }

    void build(const string& s) {
        int n = s.size();
        init_powers(n);
        H.assign(n + 1, 0);
        for (int i = 0; i < n; i++) {
            H[i + 1] = add_mod(mul_mod(H[i], BASE), (uint64_t)s[i]);
        }
    }

    // 0-indexed substring hash in [l, r]
    uint64_t get(int l, int r) const {
        return sub_mod(H[r + 1], mul_mod(H[l], P[r - l + 1]));
    }

    // Append char c to hash of string S
    static uint64_t append_char(uint64_t hash_val, char c) {
        return add_mod(mul_mod(hash_val, BASE), (uint64_t)c);
    }

    // Prepend char c to hash of string S (with length len)
    static uint64_t prepend_char(uint64_t hash_val, char c, int len) {
        init_powers(len);
        return add_mod(mul_mod((uint64_t)c, P[len]), hash_val);
    }

    // Merge H(S1) and H(S2) given length of S2
    static uint64_t merge(uint64_t h1, uint64_t h2, int len2) {
        init_powers(len2);
        return add_mod(mul_mod(h1, P[len2]), h2);
    }
};

uint64_t MersenneHash::BASE = 0;
vector<uint64_t> MersenneHash::P;

int main() {
    string s = "abracadabra";
    MersenneHash mh;
    mh.build(s);

    // 1. Substring comparison: "abra" at [0, 3] and [7, 10]
    uint64_t h1 = mh.get(0, 3);
    uint64_t h2 = mh.get(7, 10);
    cout << "Substring [0..3] == [7..10]: " << (h1 == h2 ? "TRUE" : "FALSE") << "\n";

    // 2. Append demonstration: Hash("ab") + 'c' == Hash("abc")
    MersenneHash test;
    test.build("abc");
    uint64_t h_ab = mh.get(0, 1);
    uint64_t h_abc_appended = MersenneHash::append_char(h_ab, 'c');
    cout << "Append 'c' to 'ab' matches 'abc': " << (h_abc_appended == test.get(0, 2) ? "TRUE" : "FALSE") << "\n";

    // 3. Prepend demonstration: 'a' + Hash("bc") == Hash("abc")
    uint64_t h_bc = test.get(1, 2);
    uint64_t h_abc_prepended = MersenneHash::prepend_char(h_bc, 'a', 2);
    cout << "Prepend 'a' to 'bc' matches 'abc': " << (h_abc_prepended == test.get(0, 2) ? "TRUE" : "FALSE") << "\n";

    // 4. Concat demonstration: Hash("abra") + Hash("cadabra") == Hash("abracadabra")
    uint64_t h_cadabra = mh.get(4, 10);
    uint64_t h_full = MersenneHash::merge(h1, h_cadabra, 7);
    cout << "Concat matches full string: " << (h_full == mh.get(0, 10) ? "TRUE" : "FALSE") << "\n";

    return 0;
}
