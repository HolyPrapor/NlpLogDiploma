//
// Created by zeliboba on 2/25/24.
//

#include "greedy_log_storage.hpp"
#include "vector_search.hpp"

GreedyLogStorage::GreedyLogStorage(int maxLogSize) : maxLogSize_(maxLogSize) {}

void GreedyLogStorage::Store(const std::vector<Token>& log) {
    if (storage_.size() == maxLogSize_) {
        storage_.pop_front();
    }
    storage_.push_back(log);
}

// find the longest matching substring across all stored logs
std::optional<LogLink> GreedyLogStorage::TryLink(const std::vector<Token>& log, const int& startIndex, const int& minLength) {
    auto link = TryLinkGreedily(storage_, log, startIndex);
    if (link.has_value() && link->Length >= minLength) {
        return link;
    }
    return std::nullopt;
}

// todo: this is probably not the most efficient way to get the log
const std::vector<Token>& GreedyLogStorage::GetLog(int index) {
    auto it = storage_.begin();
    std::advance(it, index);
    return *it;
}

int GreedyLogStorage::GetSize() {
    return storage_.size();
}