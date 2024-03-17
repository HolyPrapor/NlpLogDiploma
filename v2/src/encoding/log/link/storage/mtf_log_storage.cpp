//
// Created by zeliboba on 3/17/24.
//

#include "mtf_log_storage.hpp"
#include "vector_search.hpp"

MtfLogStorage::MtfLogStorage(int maxLogSize) : maxLogSize_(maxLogSize) {}

void MtfLogStorage::Store(const std::vector<Token>& log) {
    storage_.push_back(log);
    if (storage_.size() > maxLogSize_) {
        storage_.pop_front();
    }
}

std::optional<LogLink> MtfLogStorage::TryLink(const std::vector<Token>& log, const int& startIndex) {
    auto link = TryLinkGreedily(storage_, log, startIndex);
    if (link.has_value()) {
        auto record = storage_.begin();
        std::advance(record, link->RecordIndex);
        storage_.erase(record);
        storage_.push_front(*record);
    }
    return link;
}

const std::vector<Token>& MtfLogStorage::GetLog(int index) {
    auto it = storage_.begin();
    std::advance(it, index);
    return *it;
}

int MtfLogStorage::GetSize() {
    return storage_.size();
}