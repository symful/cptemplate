#include <bits/stdc++.h>
using namespace std;

// O(R) Single nCr execution
long long single_nCr(int n, int r) {
    if (r < 0 || r > n) return 0;
    if (r > n - r) r = n - r; // Optimize using symmetry: nCr = nC(n-r)
    long long res = 1;
    for (int i = 1; i <= r; i++) {
        res = res * (n - r + i) / i; // Interleaving multiplication & division to avoid early overflow
    }
    return res;
}

// O(R) Single nPr execution
long long single_nPr(int n, int r) {
    if (r < 0 || r > n) return 0;
    long long res = 1;
    for (int i = 0; i < r; i++) {
        res *= (n - i);
    }
    return res;
}
