#include <bits/stdc++.h>
using namespace std;

int n;

vector<double> dp;

double solve(int cur) {

    // base case
    if (cur == n)
        return 0;

    // kalau sudah dihitung
    if (dp[cur] != -1)
        return dp[cur];

    double ans = 0;

    // jumlah kemungkinan
    int cnt = 0;

    // contoh transition
    for (int next = cur + 1; next <= n; next++) {
        cnt++;

        ans += solve(next);
    }

    ans /= cnt;

    return dp[cur] = ans;
}

int main() {
    cin >> n;

    dp.assign(n + 1, -1);

    cout << fixed << setprecision(10);

    cout << solve(0) << endl;

    return 0;
}
