//
// Created by zeliboba on 3/17/24.
//

#include "mtf_log_storage.hpp"
#include "vector_search.hpp"

static void moveToFront(std::list<std::vector<Token>>& storage, int index) {
    auto it = storage.begin();
    std::advance(it, index);
    auto record = *it;
    storage.erase(it);
    storage.push_back(record);
}

MtfLogStorage::MtfLogStorage(int maxLogSize) : maxLogSize_(maxLogSize) {}

void MtfLogStorage::Store(const std::vector<Token>& log) {
    storage_.push_back(log);
    if (storage_.size() > maxLogSize_) {
        storage_.pop_front();
    }
}

std::optional<LogLink> MtfLogStorage::TryLink(const std::vector<Token>& log, const int& startIndex, const int& minLength) {
    auto link = TryLinkGreedily(storage_, log, startIndex);
    if (link.has_value() && link->Length >= minLength) {
        moveToFront(storage_, link->RecordIndex);
        return link;
    }
    return std::nullopt;
}

const std::vector<Token>& MtfLogStorage::GetLog(int index) {
    moveToFront(storage_, index);
    return storage_.back();
}

int MtfLogStorage::GetSize() {
    return storage_.size();
}