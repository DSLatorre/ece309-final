// src/memory/context_builder.cpp — YOU IMPLEMENT. Spec §4.4.

#include "memory/context_builder.h"
#include "util/min_heap.h"

ContextPlan select_context(const std::vector<ContextItem>& items,
                           std::size_t budget, std::size_t keep_recent) {
    ContextPlan plan;
    std::vector<bool> keep(items.size(), true);
    std::size_t total = 0;
    for (const ContextItem& item : items) total += item.tokens;

    if (total <= budget) {
        plan.total_tokens = total;
        plan.keep.reserve(items.size());
        for (std::size_t i = 0; i < items.size(); ++i) plan.keep.push_back(i);
        return plan;
    }

    const std::size_t recent_start = keep_recent >= items.size()
        ? 0
        : items.size() - keep_recent;
    MinHeap<std::pair<int, std::size_t>> candidates;
    for (std::size_t i = 0; i < items.size(); ++i) {
        const ContextItem& item = items[i];
        const bool protected_item = item.pinned || item.role == Role::System || i >= recent_start;
        if (!protected_item) candidates.push({drop_rank(item.role), i});
    }

    while (total > budget && !candidates.empty()) {
        const std::size_t index = candidates.pop().second;
        keep[index] = false;
        total -= items[index].tokens;
    }

    for (std::size_t i = 0; i < items.size(); ++i) {
        if (keep[i]) plan.keep.push_back(i);
    }
    plan.total_tokens = total;
    plan.over_budget = total > budget;
    return plan;
}
