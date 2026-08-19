#include <bits/stdc++.h>
using namespace std;

const int MAXN = 1000005;
const int MOD = 1e9 + 7;

long long fact[MAXN];
long long invFact[MAXN];

pair<long, long> decimalToRelational(double decimal) {
    // Handle special cases
    if (std::isnan(decimal) || std::isinf(decimal)) {
        return {0, 1};  // Or handle as error
    }

    // Handle negative numbers
    long sign = 1;
    if (decimal < 0) {
        sign = -1;
        decimal = -decimal;
    }

    // Separate integer and fractional parts
    long integerPart = static_cast<long>(decimal);
    double fractionalPart = decimal - integerPart;

    // If no fractional part, return as integer
    if (fractionalPart == 0) {
        return {sign * integerPart, 1};
    }

    // Use continued fraction method for better precision
    long numerator = 1;
    long denominator = 1;
    double error = 1e-12;  // Desired precision

    // Handle the fractional part using continued fractions
    long a = static_cast<long>(fractionalPart);
    long b = 1;
    long c = 1;
    long d = 0;

    while (true) {
        // Calculate next convergent
        long newNumerator = a * numerator + c;
        long newDenominator = a * denominator + d;

        // Check if this approximation is close enough
        double approx = static_cast<double>(newNumerator) / newDenominator;
        if (std::abs(approx - fractionalPart) < error * newDenominator) {
            numerator = newNumerator;
            denominator = newDenominator;
            break;
        }

        // Update for next iteration
        c = numerator;
        d = denominator;
        numerator = newNumerator;
        denominator = newDenominator;

        // Calculate next partial quotient
        double remaining = 1.0 / (fractionalPart - static_cast<double>(a));
        a = static_cast<long>(remaining);

        // Safety check to prevent infinite loops
        if (denominator > 1000000) {
            break;
        }
    }

    // Combine with integer part
    numerator = integerPart * denominator + numerator;

    // Apply sign and reduce fraction
    long gcd = std::gcd(numerator, denominator);
    numerator /= gcd;
    denominator /= gcd;

    return {sign * numerator, denominator};
}

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
