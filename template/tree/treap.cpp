#include <bits/stdc++.h>
using namespace std;

class Treap {
private:
    // ---------- Node structure ----------
    struct Node {
        int key, priority, size;
        Node *left, *right;
        Node(int k) : key(k), priority(rand()), size(1), left(nullptr), right(nullptr) {}
    };

    Node* root = nullptr;

    // ---------- Helpers ----------
    int get_size(Node* t) const { return t ? t->size : 0; }

    void update(Node* t) {
        if (t) t->size = 1 + get_size(t->left) + get_size(t->right);
    }

    // Splits tree into < key and >= key
    void split(Node* t, int key, Node*& left, Node*& right) {
        if (!t) { left = right = nullptr; return; }
        if (t->key < key) {
            split(t->right, key, t->right, right);
            left = t;
        } else {
            split(t->left, key, left, t->left);
            right = t;
        }
        update(t);
    }

    // Merges two trees (all keys in left < all keys in right)
    Node* merge(Node* left, Node* right) {
        if (!left || !right) return left ? left : right;
        if (left->priority > right->priority) {
            left->right = merge(left->right, right);
            update(left);
            return left;
        } else {
            right->left = merge(left, right->left);
            update(right);
            return right;
        }
    }

    // Recursive insert (no duplicates)
    Node* insert_rec(Node* t, int key) {
        if (!t) return new Node(key);
        if (key == t->key) return t; // ignore duplicates
        if (key < t->key) {
            t->left = insert_rec(t->left, key);
            if (t->left->priority > t->priority) {
                Node* new_root = t->left;
                t->left = new_root->right;
                new_root->right = t;
                update(t);
                update(new_root);
                return new_root;
            }
        } else {
            t->right = insert_rec(t->right, key);
            if (t->right->priority > t->priority) {
                Node* new_root = t->right;
                t->right = new_root->left;
                new_root->left = t;
                update(t);
                update(new_root);
                return new_root;
            }
        }
        update(t);
        return t;
    }

    // Recursive erase
    Node* erase_rec(Node* t, int key) {
        if (!t) return nullptr;
        if (key == t->key) {
            Node* tmp = merge(t->left, t->right);
            delete t;
            return tmp;
        }
        if (key < t->key) t->left = erase_rec(t->left, key);
        else t->right = erase_rec(t->right, key);
        update(t);
        return t;
    }

public:
    // ---------- Public API ----------
    Treap() { srand(time(0)); }

    void insert(int key) { root = insert_rec(root, key); }

    void erase(int key) { root = erase_rec(root, key); }

    // Returns largest key < x, or -1 if none
    int predecessor(int x) const {
        Node* cur = root;
        int best = -1;
        while (cur) {
            if (cur->key < x) {
                best = cur->key;
                cur = cur->right;
            } else {
                cur = cur->left;
            }
        }
        return best;
    }

    // Returns smallest key > x, or -1 if none
    int successor(int x) const {
        Node* cur = root;
        int best = -1;
        while (cur) {
            if (cur->key > x) {
                best = cur->key;
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        return best;
    }

    // Returns k‑th smallest (0‑based), or -1 if out of range
    int kth(int k) const {
        if (k < 0 || k >= size()) return -1;
        Node* cur = root;
        while (cur) {
            int left_sz = get_size(cur->left);
            if (k < left_sz) cur = cur->left;
            else if (k == left_sz) return cur->key;
            else { k -= left_sz + 1; cur = cur->right; }
        }
        return -1;
    }

    // Number of keys < x
    int count_less(int x) const {
        Node* cur = root;
        int ans = 0;
        while (cur) {
            if (cur->key < x) {
                ans += 1 + get_size(cur->left);
                cur = cur->right;
            } else {
                cur = cur->left;
            }
        }
        return ans;
    }

    int size() const { return get_size(root); }
    bool empty() const { return root == nullptr; }

    // For debugging: print inorder
    void inorder(Node* t, vector<int>& out) const {
        if (!t) return;
        inorder(t->left, out);
        out.push_back(t->key);
        inorder(t->right, out);
    }
    vector<int> inorder() const {
        vector<int> res;
        inorder(root, res);
        return res;
    }
};
