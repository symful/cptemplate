#include <bits/stdc++.h>

using namespace std;

class TrieNode {
public:
    // Array to store pointers to child nodes (26 for 'a' through 'z')
    vector<TrieNode*> children;
    // Flag to mark if a node represents the end of a complete word
    bool isEndOfWord;

    TrieNode() {
        children.assign(26, nullptr);
        isEndOfWord = false;
    }

    ~TrieNode() {
        for (TrieNode* child : children) {
            delete child;
        }
    }
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    ~Trie() {
        delete root;
    }

    // Inserts a word into the trie
    void insert(const string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            int index = ch - 'a'; // Map character to 0-25 index
            if (current->children[index] == nullptr) {
                current->children[index] = new TrieNode();
            }
            current = current->children[index];
        }
        current->isEndOfWord = true;
    }

    // Returns true if the word is exactly in the trie
    bool search(const string& word) const {
        TrieNode* current = root;
        for (char ch : word) {
            int index = ch - 'a';
            if (current->children[index] == nullptr) {
                return false;
            }
            current = current->children[index];
        }
        return current->isEndOfWord;
    }

    // Returns true if there is any word in the trie that starts with the given prefix
    bool startsWith(const string& prefix) const {
        TrieNode* current = root;
        for (char ch : prefix) {
            int index = ch - 'a';
            if (current->children[index] == nullptr) {
                return false;
            }
            current = current->children[index];
        }
        return true;
    }
};

int main() {
    Trie* myTrie = new Trie();

    myTrie->insert("apple");
    cout << boolalpha; // Print true/false instead of 1/0

    cout << "Search 'apple': " << myTrie->search("apple") << "\n";   // true
    cout << "Search 'app': " << myTrie->search("app") << "\n";       // false
    cout << "Starts with 'app': " << myTrie->startsWith("app") << "\n"; // true

    myTrie->insert("app");
    cout << "Search 'app' after insertion: " << myTrie->search("app") << "\n"; // true

    delete myTrie;
    return 0;
}
