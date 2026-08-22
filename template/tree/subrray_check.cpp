#include <bits/stdc++.h>

using namespace std;

class SubarrayCheck {
    unordered_map<int, vector<int>> pos;
public:
    SubarrayCheck(const vector<int>& arr) {
        for (int i = 0; i < arr.size(); i++) {
            pos[arr[i]].push_back(i);
        }
    }

    // Returns true if element x exists in subarray arr[L...R]
    bool query(int L, int R, int x) {
        if (pos.find(x) == pos.end()) return false;
        const auto& indices = pos[x];
        // Find first occurrence >= L
        auto it = lower_bound(indices.begin(), indices.end(), L);
        // Check if it's within R
        return (it != indices.end() && *it <= R);
    }
};

int main() {
    vector<int> arr = {10, 20, 30, 10, 40, 20};
    SubarrayCheck checker(arr);

    cout << boolalpha;
    cout << checker.query(1, 4, 20) << endl; // true (at index 1 or 5)
    cout << checker.query(3, 5, 10) << endl; // true (at index 3)
    cout << checker.query(4, 5, 10) << endl; // false
    return 0;
}
