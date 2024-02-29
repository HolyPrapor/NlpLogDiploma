//
// Created by zeliboba on 2/25/24.
//

#include <iostream>
#include "greedy_log_storage.hpp"

GreedyLogStorage::GreedyLogStorage(int maxLogSize) : maxLogSize_(maxLogSize) {}

void GreedyLogStorage::Store(const std::vector<Token>& log) {
    if (storage_.size() == maxLogSize_) {
        storage_.pop_front();
    }
    storage_.push_back(log);
}

// find the longest matching substring across all stored logs
std::optional<LogLink> GreedyLogStorage::TryLink(const std::vector<Token>& log, const int& startIndex) {
    LogLink link{-1, -1, -1};
    auto logIndex = 0;
    for (auto &line : storage_) {
        for (auto i = 0; i < line.size(); i++) {
            for (auto j = startIndex; j < log.size() && i + j - startIndex < line.size(); j++) {
                if (log[j] != line[i + j - startIndex]) {
                    break;
                }
                if (j - startIndex + 1 > link.Length) {
                    link.RecordIndex = logIndex;
                    link.StartIndex = i;
                    link.Length = j - startIndex + 1;
                }
            }
        }
        logIndex++;
    }
    if (link.Length > 0) {
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