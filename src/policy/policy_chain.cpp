// src/policy/policy_chain.cpp — YOU IMPLEMENT. Spec §5.3.

#include "policy/policy_chain.h"

#include <utility>

bool PolicyChain::add(std::unique_ptr<Policy> child) {
    if (child == nullptr) return false;
    children_.push_back(std::move(child));
    return true;
}

std::size_t PolicyChain::size() const noexcept { return children_.size(); }

Decision PolicyChain::evaluate(const CommandRequest& req) {
    if (children_.empty()) {
        return Decision::deny("empty policy chain", name());
    }

    bool has_ask = false;
    Decision first_ask;
    for (const auto& child : children_) {
        Decision decision = child->evaluate(req);
        if (decision.kind == Decision::Kind::Deny) return decision;
        if (!has_ask && decision.kind == Decision::Kind::Ask) {
            first_ask = std::move(decision);
            has_ask = true;
        }
    }
    if (has_ask) return first_ask;
    return Decision::allow("all policies allowed the command", name());
}
