// src/memory/memory_index.cpp — YOU IMPLEMENT. Spec §4.3.

#include "memory/memory_index.h"

#include <utility>
#include <vector>

MemoryIndex::~MemoryIndex() {
    clear();
}

bool MemoryIndex::insert(MemoryRecord rec) {
    Node** link = &root_;
    while (*link != nullptr) {
        if (rec.id == (*link)->rec.id) return false;
        link = rec.id < (*link)->rec.id ? &(*link)->left : &(*link)->right;
    }
    *link = new Node{std::move(rec), nullptr, nullptr};
    ++count_;
    return true;
}

const MemoryRecord* MemoryIndex::find(std::uint64_t id) const {
    Node* node = root_;
    while (node != nullptr) {
        if (id == node->rec.id) return &node->rec;
        node = id < node->rec.id ? node->left : node->right;
    }
    return nullptr;
}

bool MemoryIndex::remove(std::uint64_t id) {
    Node** link = &root_;
    while (*link != nullptr && (*link)->rec.id != id) {
        link = id < (*link)->rec.id ? &(*link)->left : &(*link)->right;
    }
    if (*link == nullptr) return false;

    Node* node = *link;
    if (node->left == nullptr) {
        *link = node->right;
        delete node;
    } else if (node->right == nullptr) {
        *link = node->left;
        delete node;
    } else {
        Node** successor_link = &node->right;
        while ((*successor_link)->left != nullptr) {
            successor_link = &(*successor_link)->left;
        }
        Node* successor = *successor_link;
        node->rec = std::move(successor->rec);
        *successor_link = successor->right;
        delete successor;
    }
    --count_;
    return true;
}

std::size_t MemoryIndex::size() const noexcept { return count_; }

std::size_t MemoryIndex::height() const {
    if (root_ == nullptr) return 0;
    std::size_t result = 0;
    std::vector<std::pair<Node*, std::size_t>> pending{{root_, 1}};
    while (!pending.empty()) {
        auto [node, depth] = pending.back();
        pending.pop_back();
        if (depth > result) result = depth;
        if (node->left != nullptr) pending.push_back({node->left, depth + 1});
        if (node->right != nullptr) pending.push_back({node->right, depth + 1});
    }
    return result;
}

void MemoryIndex::for_each_in_order(const std::function<void(const MemoryRecord&)>& fn) const {
    std::vector<Node*> pending;
    Node* node = root_;
    while (node != nullptr || !pending.empty()) {
        while (node != nullptr) {
            pending.push_back(node);
            node = node->left;
        }
        node = pending.back();
        pending.pop_back();
        fn(node->rec);
        node = node->right;
    }
}

void MemoryIndex::clear() {
    std::vector<Node*> pending;
    if (root_ != nullptr) pending.push_back(root_);
    while (!pending.empty()) {
        Node* node = pending.back();
        pending.pop_back();
        if (node->left != nullptr) pending.push_back(node->left);
        if (node->right != nullptr) pending.push_back(node->right);
        delete node;
    }
    root_ = nullptr;
    count_ = 0;
}
