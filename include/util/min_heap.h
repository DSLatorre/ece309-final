// include/util/min_heap.h
//
// YOU IMPLEMENT (this whole file; it is a template, so the code lives in the
// header). Spec §4.1.
//
// A binary MIN-heap stored in an array (a std::vector), plus top_k(), which
// uses a size-k min-heap to pick the k largest items in O(n log k).
//
// Rules for this file:
//   * Write sift-up and sift-down yourself.
//   * Do NOT use std::priority_queue, std::make_heap / push_heap / pop_heap /
//     sort_heap, std::sort / stable_sort / partial_sort / nth_element, or
//     std::set / multiset / map / multimap (full list: spec §7.3).
//   * std::vector is fine as the storage array; std::swap, std::reverse and
//     std::pair are fine too.
//
// `Less` is a "less-than" function object, exactly like the one std::sort
// takes: less(a, b) is true when a should come before b. The heap keeps the
// SMALLEST element (by less) at the top.

#pragma once
#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T, typename Less = std::less<T>>
class MinHeap {
public:
    explicit MinHeap(Less less = Less()) : less_(std::move(less)) {}

    // Adds value. O(log n).
    void push(T value) {
        data_.push_back(std::move(value));
        sift_up(data_.size() - 1);
    }

    // The smallest element. Throws std::out_of_range if empty. O(1).
    const T& top() const {
        if (data_.empty()) throw std::out_of_range("MinHeap::top on empty heap");
        return data_.front();
    }

    // Removes and returns the smallest element. Throws std::out_of_range if
    // empty. O(log n).
    T pop() {
        if (data_.empty()) throw std::out_of_range("MinHeap::pop on empty heap");
        T result = std::move(data_.front());
        if (data_.size() == 1) {
            data_.pop_back();
            return result;
        }
        data_.front() = std::move(data_.back());
        data_.pop_back();
        sift_down(0);
        return result;
    }

    std::size_t size() const noexcept { return data_.size(); }
    bool        empty() const noexcept { return data_.empty(); }
    void        clear() noexcept { data_.clear(); }

private:
    void sift_up(std::size_t index) {
        while (index > 0) {
            std::size_t parent = (index - 1) / 2;
            if (!less_(data_[index], data_[parent])) break;
            std::swap(data_[index], data_[parent]);
            index = parent;
        }
    }

    void sift_down(std::size_t index) {
        while (true) {
            std::size_t left = 2 * index + 1;
            if (left >= data_.size()) break;
            std::size_t child = left;
            std::size_t right = left + 1;
            if (right < data_.size() && less_(data_[right], data_[left])) child = right;
            if (!less_(data_[child], data_[index])) break;
            std::swap(data_[index], data_[child]);
            index = child;
        }
    }

    std::vector<T> data_;
    Less           less_;
};

// Returns the k LARGEST items of `items` (by `less`), largest first.
//   * k == 0 or items empty -> empty result
//   * k >= items.size()     -> every item, largest first
// Must keep a MinHeap of at most k items (the k largest seen so far, with
// the smallest of them on top), so it runs in O(n log k) time and O(k) extra
// space. Items that compare equal may come out in any order.
template <typename T, typename Less = std::less<T>>
std::vector<T> top_k(const std::vector<T>& items, std::size_t k, Less less = Less()) {
    if (k == 0 || items.empty()) return {};

    MinHeap<T, Less> heap(less);
    for (const T& item : items) {
        if (heap.size() < k) {
            heap.push(item);
        } else if (less(heap.top(), item)) {
            heap.pop();
            heap.push(item);
        }
    }

    std::vector<T> result;
    result.reserve(heap.size());
    while (!heap.empty()) result.push_back(heap.pop());
    std::reverse(result.begin(), result.end());
    return result;
}
