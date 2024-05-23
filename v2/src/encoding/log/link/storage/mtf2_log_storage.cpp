//
// Created by zeliboba on 5/23/24.
//

#include "mtf2_log_storage.hpp"
#include "vector_search.hpp"

Mtf2LogStorage::Mtf2LogStorage(int maxLogSize, std::unique_ptr<LogFilter>&& filter, int staticMovementDegree) : storage(maxLogSize, staticMovementDegree), filter(std::move(filter)), maxLogSize(maxLogSize) {}

void Mtf2LogStorage::Store(const std::vector<Token>& log) {
    if (storage.Elements.size() == maxLogSize && filter != nullptr && filter->Filter(log)) {
        return;
    }
    auto popped = storage.PushAndOverflow(log);
    if (popped.has_value() && filter != nullptr) {
        filter->Remove(popped.value());
    }
    if (filter != nullptr) {
        filter->Store(log);
    }
}

std::optional<LogLink> Mtf2LogStorage::TryLink(const std::vector<Token>& log, const int& startIndex, const int& minLength) {
    auto link = TryLinkGreedily(storage.Elements, log, startIndex);
    if (link.has_value() && link->Length >= minLength) {
        storage.MoveToFrontByIndex(link->RecordIndex);
        return link;
    }
    return std::nullopt;
}

const std::vector<Token>& Mtf2LogStorage::GetLog(int index) {
    auto newIndex = storage.MoveToFrontByIndex(index);
    return storage.Elements[newIndex];
}

int Mtf2LogStorage::GetSize() {
    return storage.Elements.size();
}