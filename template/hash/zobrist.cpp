#include <iostream>
#include <map>
#include <random>
#include <chrono>

using namespace std;

struct ZobristHasher {
    mt19937_64 rng;
    map<int, uint64_t> val_map;

    ZobristHasher() : rng(chrono::steady_clock::now().time_since_epoch().count()) {}

    uint64_t get_random(int val) {
        if (!val_map.count(val)) {
            val_map[val] = rng();
        }
        return val_map[val];
    }
};

struct MultisetHash {
    uint64_t hash_val = 0;
    ZobristHasher& zh;

    MultisetHash(ZobristHasher& hasher) : zh(hasher) {}

    // Add/remove element to multiset (order independent)
    void insert(int x) { hash_val += zh.get_random(x); }
    void erase(int x)  { hash_val -= zh.get_random(x); }

    // Merge two multiset hashes
    static uint64_t merge(uint64_t h1, uint64_t h2) { return h1 + h2; }
};

struct SetHash {
    uint64_t hash_val = 0;
    ZobristHasher& zh;

    SetHash(ZobristHasher& hasher) : zh(hasher) {}

    // Toggle element in set (XOR)
    void toggle(int x) { hash_val ^= zh.get_random(x); }
};

int main() {
    ZobristHasher hasher;

    // Multiset demonstration: Check if [1, 2, 2, 3] is an anagram of [3, 2, 1, 2]
    MultisetHash bag1(hasher), bag2(hasher);
    for (int x : {1, 2, 2, 3}) bag1.insert(x);
    for (int x : {3, 2, 1, 2}) bag2.insert(x);

    cout << "Multiset [1, 2, 2, 3] == [3, 2, 1, 2]: "
         << (bag1.hash_val == bag2.hash_val ? "TRUE" : "FALSE") << "\n";

    // Set demonstration: Check if sets match after inserting and removing duplicates
    SetHash setA(hasher), setB(hasher);
    setA.toggle(5); setA.toggle(10);
    setB.toggle(10); setB.toggle(5); setB.toggle(99); setB.toggle(99); // 99 cancelled

    cout << "Set {5, 10} == {10, 5, 99, 99}: "
         << (setA.hash_val == setB.hash_val ? "TRUE" : "FALSE") << "\n";

    return 0;
}
