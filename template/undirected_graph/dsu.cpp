#include <bits/stdc++.h>
using namespace std;

class DSU {
    public:
        static const int N = 1e5+5;
        int parent[N];
        int gsize[N];
        DSU(int n) {
            for(int i = 1; i <= n; i++){
                parent[i] = i;
                gsize[i] = 1;
            }
        }
        int find(int node){
            if(parent[node] == node) return node;
            return parent[node] = find(parent[node]);
        }
        void union_by_size(int node1, int node2){
            int leaderA = find(node1);
            int leaderB = find(node2);
            if (leaderA != leaderB){
                if (gsize[leaderA] > gsize[leaderB]){
                    parent[leaderB] =leaderA;
                    gsize[leaderA] += gsize[leaderB];
                } else {
                    parent[leaderA] = leaderB;
                    gsize[leaderB] += gsize[leaderA];
                }
            }
        }
};

int main() {
    int n = 6;
    DSU dsu(n);
    vector<pair<int, int>> list;
    // Koneksi node 1 ke node 2
    list.emplace_back(1, 2);
    // Koneksi node 2 ke node 3
    list.emplace_back(2, 3);
    // Koneksi node 1 ke node 3
    list.emplace_back(1, 3);
    // Koneksi node 4 ke node 5
    list.emplace_back(4, 5);
    set<int> leaders;
    vector<bool> used(n+1, false);
    int lingkaran = 0;
    for (auto [a, b] : list){
        int leaderA = dsu.find(a);
        int leaderB = dsu.find(b);
        if (leaderA == leaderB){
            lingkaran++;
        } else{
            dsu.union_by_size(a,b);
        }
        used[a] = true;
        used[b] = true;
    }
    for (int i = 1; i <= n; i++) {
        if (used[i]) {
            leaders.insert(dsu.find(i));

            cout << "Anggota " << i << " memiliki ketua " << dsu.find(i) << "\n";
        } else {
            cout << "Anggota " << i << " tidak memiliki kelompok\n";
        }
    }
    cout << "Jumlah lingkaran: " << lingkaran << endl;
    cout << "Jumlah kelompok: " << leaders.size() << endl;
    return 0;
}
