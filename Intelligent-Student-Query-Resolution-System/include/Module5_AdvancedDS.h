#ifndef MODULE5_ADVANCEDDS_H
#define MODULE5_ADVANCEDDS_H

#include "Ticket.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

// ============================================================================
// 5A. Red-Black Tree for Write-Heavy In-Memory Audit Log Indexing
// Guarantees O(log n) search/insert with <= 2 rotations per insertion
// ============================================================================
class AuditRedBlackTree {
public:
    enum Color { RED, BLACK };

private:
    struct RBNode {
        Ticket data;
        Color color;
        RBNode* left;
        RBNode* right;
        RBNode* parent;

        explicit RBNode(const Ticket& t)
            : data(t), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    RBNode* root = nullptr;

    void rotateLeft(RBNode* x) {
        RBNode* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotateRight(RBNode* y) {
        RBNode* x = y->left;
        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        if (!y->parent) root = x;
        else if (y == y->parent->left) y->parent->left = x;
        else y->parent->right = x;
        x->right = y;
        y->parent = x;
    }

    void fixInsert(RBNode* z) {
        while (z->parent && z->parent->color == RED) {
            RBNode* gp = z->parent->parent;
            if (!gp) break;
            if (z->parent == gp->left) {
                RBNode* uncle = gp->right;
                if (uncle && uncle->color == RED) {
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    z = gp;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotateLeft(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    rotateRight(gp);
                }
            } else {
                RBNode* uncle = gp->left;
                if (uncle && uncle->color == RED) {
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    z = gp;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotateRight(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    rotateLeft(gp);
                }
            }
        }
        if (root) root->color = BLACK;
    }

    void inorder(RBNode* n, std::vector<Ticket>& out) const {
        if (!n) return;
        inorder(n->left, out);
        out.push_back(n->data);
        inorder(n->right, out);
    }

    // Validates RB invariants: returns black-height, or -1 if violated
    int validateNode(RBNode* n) const {
        if (!n) return 1;
        if (n->color == RED) {
            if ((n->left && n->left->color == RED) || (n->right && n->right->color == RED))
                return -1;
        }
        int lh = validateNode(n->left);
        int rh = validateNode(n->right);
        if (lh == -1 || rh == -1 || lh != rh) return -1;
        return lh + (n->color == BLACK ? 1 : 0);
    }

    void destroy(RBNode* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

public:
    ~AuditRedBlackTree() { destroy(root); }

    bool insert(const Ticket& t) {
        RBNode* y = nullptr;
        RBNode* x = root;
        while (x) {
            y = x;
            if (t.ticketId < x->data.ticketId) x = x->left;
            else if (t.ticketId > x->data.ticketId) x = x->right;
            else return false; // Duplicate Ticket ID rejected
        }
        RBNode* z = new RBNode(t);
        z->parent = y;
        if (!y) root = z;
        else if (z->data.ticketId < y->data.ticketId) y->left = z;
        else y->right = z;

        fixInsert(z);
        return true;
    }

    bool search(int ticketId, Ticket& out) const {
        RBNode* curr = root;
        while (curr) {
            if (ticketId == curr->data.ticketId) {
                out = curr->data;
                return true;
            }
            curr = (ticketId < curr->data.ticketId) ? curr->left : curr->right;
        }
        return false;
    }

    std::vector<Ticket> getAuditLogInOrder() const {
        std::vector<Ticket> out;
        inorder(root, out);
        return out;
    }

    bool verifyInvariants() const {
        if (!root) return true;
        if (root->color != BLACK) return false;
        return validateNode(root) != -1;
    }
};

// ============================================================================
// 5B. Simplified B+ Tree Leaf-Linked Archive for Range Compliance Scans
// ============================================================================
class BPlusArchive {
private:
    static constexpr int LEAF_CAPACITY = 4;

    struct LeafBlock {
        std::vector<Ticket> records;
        LeafBlock* next = nullptr;
    };

    std::vector<LeafBlock*> leaves;

public:
    ~BPlusArchive() {
        for (auto* b : leaves) delete b;
    }

    void archiveTicket(const Ticket& t) {
        if (leaves.empty()) {
            leaves.push_back(new LeafBlock());
        }
        // Insert into appropriate leaf block keeping IDs sorted
        LeafBlock* target = leaves.back();
        for (auto* blk : leaves) {
            if (blk->records.empty() || t.ticketId <= blk->records.back().ticketId) {
                target = blk;
                break;
            }
        }
        target->records.push_back(t);
        std::sort(target->records.begin(), target->records.end(),
                  [](const Ticket& a, const Ticket& b) { return a.ticketId < b.ticketId; });

        // Split leaf block if over capacity
        if (static_cast<int>(target->records.size()) > LEAF_CAPACITY) {
            LeafBlock* newBlock = new LeafBlock();
            int mid = static_cast<int>(target->records.size()) / 2;
            newBlock->records.assign(target->records.begin() + mid, target->records.end());
            target->records.erase(target->records.begin() + mid, target->records.end());

            newBlock->next = target->next;
            target->next = newBlock;
            auto it = std::find(leaves.begin(), leaves.end(), target);
            leaves.insert(it + 1, newBlock);
        }
    }

    // Sequential leaf-linked range query [minId, maxId]
    std::vector<Ticket> rangeScan(int minId, int maxId) const {
        std::vector<Ticket> res;
        if (leaves.empty()) return res;
        LeafBlock* curr = leaves.front();
        while (curr) {
            for (const auto& t : curr->records) {
                if (t.ticketId >= minId && t.ticketId <= maxId) res.push_back(t);
                if (t.ticketId > maxId) return res;
            }
            curr = curr->next;
        }
        return res;
    }
};

#endif // MODULE5_ADVANCEDDS_H
