#ifndef MODULE1_TREES_H
#define MODULE1_TREES_H

#include "Ticket.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <stdexcept>

// ============================================================================
// 1A. Binary Max-Heap (Priority Queue) with Timestamp FIFO Tie-Breaking
// ============================================================================
class QueryMaxHeap {
private:
    std::vector<Ticket> heap;

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx].hasHigherPriorityThan(heap[parent])) {
                std::swap(heap[idx], heap[parent]);
                idx = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int idx, int n) {
        while (true) {
            int largest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;

            if (left < n && heap[left].hasHigherPriorityThan(heap[largest]))
                largest = left;
            if (right < n && heap[right].hasHigherPriorityThan(heap[largest]))
                largest = right;

            if (largest != idx) {
                std::swap(heap[idx], heap[largest]);
                idx = largest;
            } else {
                break;
            }
        }
    }

public:
    void insert(const Ticket& t) {
        heap.push_back(t);
        heapifyUp(static_cast<int>(heap.size()) - 1);
    }

    bool empty() const { return heap.empty(); }
    size_t size() const { return heap.size(); }

    Ticket peek() const {
        if (heap.empty()) throw std::runtime_error("Priority Queue is empty.");
        return heap[0];
    }

    Ticket extractMax() {
        if (heap.empty()) throw std::runtime_error("Priority Queue is empty.");
        Ticket top = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0, static_cast<int>(heap.size()));
        return top;
    }

    // Heap Sort returning a priority-ordered copy (highest priority first)
    std::vector<Ticket> getSortedReport() const {
        QueryMaxHeap copy = *this;
        std::vector<Ticket> sorted;
        while (!copy.empty()) {
            sorted.push_back(copy.extractMax());
        }
        return sorted;
    }
};

// ============================================================================
// 1B. Trie (Prefix Tree) for Keyword Extraction & Auto-Categorization
// ============================================================================
class CategoryTrie {
private:
    struct TrieNode {
        std::unordered_map<char, TrieNode*> children;
        bool isEndOfWord = false;
        std::string category;
        int urgencyBoost = 0;

        ~TrieNode() {
            for (auto& kv : children) delete kv.second;
        }
    };

    TrieNode* root;

    static std::string normalizeWord(const std::string& raw) {
        std::string out;
        for (char c : raw) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            }
        }
        return out;
    }

public:
    CategoryTrie() : root(new TrieNode()) {
        // Default university keyword dictionary
        insertKeyword("exam", "Examination Cell", 45);
        insertKeyword("hallticket", "Examination Cell", 50);
        insertKeyword("admit", "Examination Cell", 50);
        insertKeyword("marksheet", "Examination Cell", 35);
        insertKeyword("result", "Examination Cell", 35);
        insertKeyword("revaluation", "Examination Cell", 30);

        insertKeyword("fee", "Accounts & Finance", 40);
        insertKeyword("fees", "Accounts & Finance", 40);
        insertKeyword("scholarship", "Accounts & Finance", 35);
        insertKeyword("refund", "Accounts & Finance", 30);
        insertKeyword("receipt", "Accounts & Finance", 25);

        insertKeyword("hostel", "Hostel Administration", 30);
        insertKeyword("mess", "Hostel Administration", 20);
        insertKeyword("room", "Hostel Administration", 25);

        insertKeyword("wifi", "IT Support", 25);
        insertKeyword("portal", "IT Support", 35);
        insertKeyword("erp", "IT Support", 35);
        insertKeyword("password", "IT Support", 30);

        insertKeyword("attendance", "Academic Department", 30);
        insertKeyword("syllabus", "Academic Department", 20);
        insertKeyword("timetable", "Academic Department", 25);
        insertKeyword("lab", "Academic Department", 25);
        insertKeyword("urgent", "General", 20);
        insertKeyword("tomorrow", "General", 25);
    }

    ~CategoryTrie() { delete root; }

    void insertKeyword(const std::string& word, const std::string& category, int urgencyBoost) {
        std::string w = normalizeWord(word);
        TrieNode* curr = root;
        for (char c : w) {
            if (!curr->children.count(c)) curr->children[c] = new TrieNode();
            curr = curr->children[c];
        }
        curr->isEndOfWord = true;
        curr->category = category;
        curr->urgencyBoost = urgencyBoost;
    }

    bool searchKeyword(const std::string& word, std::string& outCategory, int& outBoost) const {
        std::string w = normalizeWord(word);
        TrieNode* curr = root;
        for (char c : w) {
            auto it = curr->children.find(c);
            if (it == curr->children.end()) return false;
            curr = it->second;
        }
        if (curr->isEndOfWord) {
            outCategory = curr->category;
            outBoost = curr->urgencyBoost;
            return true;
        }
        return false;
    }

    // Parses full query text, returns {detectedCategory, computedPriorityScore}
    std::pair<std::string, int> categorizeQuery(const std::string& queryText) const {
        std::istringstream iss(queryText);
        std::string token;
        std::string bestCategory = "General";
        int maxCatBoost = 0;
        int totalScore = 20; // Base priority

        while (iss >> token) {
            std::string cat;
            int boost = 0;
            if (searchKeyword(token, cat, boost)) {
                totalScore += boost;
                if (cat != "General" && boost > maxCatBoost) {
                    maxCatBoost = boost;
                    bestCategory = cat;
                }
            }
        }
        if (totalScore > 100) totalScore = 100;
        return {bestCategory, totalScore};
    }
};

// ============================================================================
// 1C. Self-Balancing AVL Tree for O(log n) Ticket Registry
// ============================================================================
class TicketAVLTree {
private:
    struct Node {
        Ticket data;
        Node* left;
        Node* right;
        int height;
        explicit Node(const Ticket& t) : data(t), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root = nullptr;

    int height(Node* n) const { return n ? n->height : 0; }
    int balanceFactor(Node* n) const { return n ? height(n->left) - height(n->right) : 0; }

    void updateHeight(Node* n) {
        if (n) n->height = 1 + std::max(height(n->left), height(n->right));
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    Node* balance(Node* n) {
        updateHeight(n);
        int bf = balanceFactor(n);
        if (bf > 1) {
            if (balanceFactor(n->left) < 0) n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (bf < -1) {
            if (balanceFactor(n->right) > 0) n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }

    Node* insertNode(Node* n, const Ticket& t) {
        if (!n) return new Node(t);
        if (t.ticketId < n->data.ticketId) n->left = insertNode(n->left, t);
        else if (t.ticketId > n->data.ticketId) n->right = insertNode(n->right, t);
        else {
            n->data = t; // Update existing ticket
            return n;
        }
        return balance(n);
    }

    Node* minValueNode(Node* n) const {
        Node* curr = n;
        while (curr && curr->left) curr = curr->left;
        return curr;
    }

    Node* deleteNode(Node* n, int ticketId) {
        if (!n) return nullptr;
        if (ticketId < n->data.ticketId) n->left = deleteNode(n->left, ticketId);
        else if (ticketId > n->data.ticketId) n->right = deleteNode(n->right, ticketId);
        else {
            if (!n->left || !n->right) {
                Node* temp = n->left ? n->left : n->right;
                delete n;
                return temp;
            }
            Node* temp = minValueNode(n->right);
            n->data = temp->data;
            n->right = deleteNode(n->right, temp->data.ticketId);
        }
        return balance(n);
    }

    Node* searchNode(Node* n, int ticketId) const {
        if (!n || n->data.ticketId == ticketId) return n;
        if (ticketId < n->data.ticketId) return searchNode(n->left, ticketId);
        return searchNode(n->right, ticketId);
    }

    void inorder(Node* n, std::vector<Ticket>& out) const {
        if (!n) return;
        inorder(n->left, out);
        out.push_back(n->data);
        inorder(n->right, out);
    }

    bool verifyBalance(Node* n) const {
        if (!n) return true;
        int bf = balanceFactor(n);
        if (bf < -1 || bf > 1) return false;
        return verifyBalance(n->left) && verifyBalance(n->right);
    }

    void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

public:
    ~TicketAVLTree() { destroy(root); }

    void insert(const Ticket& t) { root = insertNode(root, t); }
    void remove(int ticketId) { root = deleteNode(root, ticketId); }

    bool search(int ticketId, Ticket& out) const {
        Node* res = searchNode(root, ticketId);
        if (!res) return false;
        out = res->data;
        return true;
    }

    std::vector<Ticket> getAllSorted() const {
        std::vector<Ticket> out;
        inorder(root, out);
        return out;
    }

    int getTreeHeight() const { return height(root); }
    bool isBalanced() const { return verifyBalance(root); }
};

// ============================================================================
// 1D. Hash Map FAQ Cache for O(1) Instant Resolution
// ============================================================================
class FAQHashCache {
private:
    std::unordered_map<std::string, std::string> faqMap;

    static std::string normalizeKey(const std::string& s) {
        std::string res;
        for (char c : s) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == ' ')
                res.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
        return res;
    }

public:
    FAQHashCache() {
        addFAQ("download admit card", "Log in to the NIET ERP portal -> Examination -> Download Hall Ticket.");
        addFAQ("erp password reset", "Visit ERP login page -> Forgot Password -> Verify with registered college email.");
        addFAQ("fee payment receipt", "Open ERP -> Accounts -> Fee History -> Click 'Download Receipt'.");
        addFAQ("hostel wifi issue", "Submit your MAC address and room number at the IT Helpdesk (Block B, Room 104).");
        addFAQ("minimum attendance criteria", "Students must maintain at least 75% attendance as per university norms.");
    }

    void addFAQ(const std::string& key, const std::string& answer) {
        faqMap[normalizeKey(key)] = answer;
    }

    bool lookup(const std::string& query, std::string& outAnswer) const {
        std::string norm = normalizeKey(query);
        auto it = faqMap.find(norm);
        if (it != faqMap.end()) {
            outAnswer = it->second;
            return true;
        }
        return false;
    }
};

#endif // MODULE1_TREES_H
