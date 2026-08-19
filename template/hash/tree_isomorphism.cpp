#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// 64-bit SplitMix-like Pseudo-Random Function
uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

struct TreeHasher {
    int n;
    vector<vector<int>> adj;
    vector<int> sz;

    TreeHasher(int n) : n(n), adj(n + 1), sz(n + 1) {}

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void get_sz(int u, int p) {
        sz[u] = 1;
        for (int v : adj[u]) {
            if (v != p) {
                get_sz(v, u);
                sz[u] += sz[v];
            }
        }
    }

    void get_centroids(int u, int p, vector<int>& centroids) {
        bool is_centroid = true;
        for (int v : adj[u]) {
            if (v != p) {
                get_centroids(v, u, centroids);
                if (sz[v] > n / 2) is_centroid = false;
            }
        }
        if (n - sz[u] > n / 2) is_centroid = false;
        if (is_centroid) centroids.push_back(u);
    }

    uint64_t compute_hash(int u, int p) {
        uint64_t sub_hash = 1;
        for (int v : adj[u]) {
            if (v != p) {
                sub_hash += splitmix64(compute_hash(v, u));
            }
        }
        return sub_hash;
    }

    // Returns a canonical pair of hashes representing the unrooted tree
    uint64_t get_canonical_hash() {
        get_sz(1, 0);
        vector<int> centroids;
        get_centroids(1, 0, centroids);

        vector<uint64_t> hashes;
        for (int c : centroids) {
            hashes.push_back(compute_hash(c, 0));
        }
        sort(hashes.begin(), hashes.end());
        return hashes[0]; // If 2 centroids, pick the minimum
    }
};

int main() {
    // Tree 1: 1-2-3-4 (Line of 4 nodes)
    TreeHasher t1(4);
    t1.add_edge(1, 2);
    t1.add_edge(2, 3);
    t1.add_edge(3, 4);

    // Tree 2: 4-1-3-2 (Isomorphic line of 4 nodes with permuted labels)
    TreeHasher t2(4);
    t2.add_edge(4, 1);
    t2.add_edge(1, 3);
    t2.add_edge(3, 2);

    // Tree 3: Star graph with center 1
    TreeHasher t3(4);
    t3.add_edge(1, 2);
    t3.add_edge(1, 3);
    t3.add_edge(1, 4);

    cout << "Tree 1 == Tree 2 (Isomorphic): "
         << (t1.get_canonical_hash() == t2.get_canonical_hash() ? "TRUE" : "FALSE") << "\n";
    cout << "Tree 1 == Tree 3 (Non-Isomorphic): "
         << (t1.get_canonical_hash() == t3.get_canonical_hash() ? "TRUE" : "FALSE") << "\n";

    return 0;
}
