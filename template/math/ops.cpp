#include <bits/stdc++.h>
using namespace std;

const int MAXN = 1000005;
const int MOD = 1e9 + 7;

long long fact[MAXN];
long long invFact[MAXN];

// Fast exponentiation: O(log power)
long long power(long long base, long long exp) {
    long long res = 1;
    base %= MOD;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % MOD;
        base = (base * base) % MOD;
        exp /= 2;
    }
    return res;
}

// Modular inverse via Fermat's Little Theorem: O(log MOD)
long long modInverse(long long n) {
    return power(n, MOD - 2);
}

// O(N) precomputation for factorials and their inverses
void precompute() {
    fact[0] = invFact[0] = 1;
    for (int i = 1; i < MAXN; i++) {
        fact[i] = (fact[i - 1] * i) % MOD;
    }
    invFact[MAXN - 1] = modInverse(fact[MAXN - 1]);
    for (int i = MAXN - 2; i >= 1; i--) {
        invFact[i] = (invFact[i + 1] * (i + 1)) % MOD;
    }
}

// O(1) nCr Query
long long nCr(int n, int r) {
    if (r < 0 || r > n) return 0;
    return fact[n] * invFact[r] % MOD * invFact[n - r] % MOD;
}

// O(1) nPr Query
long long nPr(int n, int r) {
    if (r < 0 || r > n) return 0;
    return fact[n] * invFact[n - r] % MOD;
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); // Fast I/O
    precompute();

    cout << nCr(5, 2) << "\n"; // Output: 10
    cout << nPr(5, 2) << "\n"; // Output: 20
    return 0;
}
