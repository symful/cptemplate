#include <bits/stdc++.h>
using namespace std;

/*
============================================================
GREEDY + BINARY SEARCH TEMPLATE
============================================================

Isi:
1. Greedy dasar
2. Binary Search pada array
3. Binary Search untuk jawaban
4. Optimasi partisi
5. Alokasi / kapasitas minimum

============================================================
*/


/*
============================================================
1. GREEDY DASAR
============================================================

Pola umum:

sort(...)
ambil pilihan terbaik selama memungkinkan

Contoh:
memilih sebanyak mungkin interval yang tidak overlap.
*/

struct Activity {
    int start;
    int finish;
};

bool cmp(Activity a, Activity b) {
    return a.finish < b.finish;
}

int greedy_activity(vector<Activity>& activities) {

    sort(activities.begin(), activities.end(), cmp);

    int ans = 0;
    int last_finish = -1;

    for (auto activity : activities) {

        if (activity.start >= last_finish) {

            ans++;

            last_finish = activity.finish;
        }
    }

    return ans;
}


/*
============================================================
2. BINARY SEARCH ARRAY
============================================================

Mencari nilai dalam array yang sudah terurut.

lower_bound:
posisi pertama >= x

upper_bound:
posisi pertama > x
*/

int binary_search_array(vector<int>& a, int x) {

    int l = 0;
    int r = (int)a.size() - 1;

    while (l <= r) {

        int mid = l + (r - l) / 2;

        if (a[mid] == x)
            return mid;

        if (a[mid] < x)
            l = mid + 1;
        else
            r = mid - 1;
    }

    return -1;
}


/*
============================================================
3. BINARY SEARCH ON ANSWER
============================================================

Ini yang PALING PENTING.

Kita tidak mencari posisi/value secara langsung.

Kita mencari:

"Apakah jawaban X memungkinkan?"

Contoh:

minimum kapasitas yang diperlukan.

check(mid)
    true  -> kapasitas mid cukup
    false -> kapasitas mid tidak cukup

Jika:

false false false true true true

maka cari TRUE pertama.
*/

bool check(long long mid) {

    /*
    Tulis logika:

    Apakah jawaban = mid
    memenuhi semua kondisi?

    return true / false;
    */

    return true;
}

long long binary_search_answer(
    long long l,
    long long r
) {

    while (l < r) {

        long long mid = l + (r - l) / 2;

        if (check(mid)) {

            // mid memungkinkan
            r = mid;

        } else {

            // mid tidak memungkinkan
            l = mid + 1;
        }
    }

    return l;
}


/*
============================================================
4. OPTIMASI PARTISI
============================================================

Pola:

Diberikan array.

Partisi menjadi K bagian.

Tujuan:
meminimalkan nilai maksimum jumlah setiap bagian.

Contoh:

a = [7, 2, 5, 10, 8]

K = 2

Partisi:

[7,2,5] [10,8]

sum:
14       18

maksimum = 18

Kita ingin meminimalkan maksimum tersebut.

Binary search:

low  = max(a)
high = sum(a)

check(mid):
    apakah array bisa dibagi menjadi <= K bagian
    dengan setiap bagian memiliki sum <= mid?
*/


bool check_partition(
    vector<long long>& a,
    int k,
    long long mid
) {

    int cnt = 1;
    long long sum = 0;

    for (auto x : a) {

        // satu elemen saja sudah terlalu besar
        if (x > mid)
            return false;

        if (sum + x <= mid) {

            sum += x;

        } else {

            // buat bagian baru
            cnt++;

            sum = x;
        }
    }

    return cnt <= k;
}


long long minimize_partition(
    vector<long long>& a,
    int k
) {

    long long l = 0;
    long long r = 0;

    for (auto x : a) {

        l = max(l, x);
        r += x;
    }

    while (l < r) {

        long long mid = l + (r - l) / 2;

        if (check_partition(a, k, mid)) {

            r = mid;

        } else {

            l = mid + 1;
        }
    }

    return l;
}


/*
============================================================
5. ALOKASI / CAPACITY
============================================================

Pola yang sangat mirip dengan partisi.

Contoh:

N barang harus dimasukkan ke K container.

Setiap container memiliki kapasitas X.

Pertanyaan:

"Apakah kapasitas X cukup?"

Greedy:

Masukkan sebanyak mungkin ke container sekarang.
Kalau tidak muat -> pindah container.

Jika jumlah container <= K:
    X memungkinkan.

Kemudian binary search X.
*/


bool check_allocation(
    vector<long long>& a,
    int k,
    long long capacity
) {

    int cnt = 1;
    long long current = 0;

    for (auto x : a) {

        if (x > capacity)
            return false;

        if (current + x <= capacity) {

            current += x;

        } else {

            cnt++;
            current = x;
        }
    }

    return cnt <= k;
}


long long minimum_capacity(
    vector<long long>& a,
    int k
) {

    long long l = 0;
    long long r = 0;

    for (auto x : a) {

        l = max(l, x);
        r += x;
    }

    while (l < r) {

        long long mid = l + (r - l) / 2;

        if (check_allocation(a, k, mid)) {

            r = mid;

        } else {

            l = mid + 1;
        }
    }

    return l;
}


/*
============================================================
6. MAXIMIZE DENGAN BINARY SEARCH
============================================================

Tidak selalu mencari minimum.

Kadang kita mencari:

"nilai maksimum X yang masih memungkinkan"

Pola:

true true true true false false

Maka:

if (check(mid))
    l = mid;
else
    r = mid - 1;

Template:
*/

long long maximize_answer(
    long long l,
    long long r
) {

    while (l < r) {

        long long mid = l + (r - l + 1) / 2;

        if (check(mid)) {

            l = mid;

        } else {

            r = mid - 1;
        }
    }

    return l;
}


/*
============================================================
7. GREEDY + BINARY SEARCH TEMPLATE UMUM
============================================================

Kalau ketemu soal optimasi:

    "minimum X sehingga..."
atau
    "maximum X sehingga..."

Coba pikirkan:

1. Tentukan X sebagai jawaban.
2. Buat check(X).
3. Pastikan check monoton.
4. Binary search X.
5. Di dalam check(), gunakan greedy.

Pola:

bool check(long long mid) {

    // greedy

    return ...;
}

long long l = ...;
long long r = ...;

while (l < r) {

    long long mid = l + (r - l) / 2;

    if (check(mid))
        r = mid;
    else
        l = mid + 1;
}

cout << l;
*/


/*
============================================================
MAIN
============================================================
*/

int main() {

    /*
    Contoh penggunaan:

    int n, k;
    cin >> n >> k;

    vector<long long> a(n);

    for (auto &x : a)
        cin >> x;

    cout << minimize_partition(a, k) << '\n';
    */


    return 0;
}
