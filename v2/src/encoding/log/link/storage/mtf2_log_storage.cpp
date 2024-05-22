//
// Created by zeliboba on 5/23/24.
//

#include "mtf2_log_storage.hpp"
#include "vector_search.hpp"

Mtf2LogStorage::Mtf2LogStorage(int maxLogSize, bool useDynamicMovement) : storage(maxLogSize, useDynamicMovement) {}

void Mtf2LogStorage::Store(const std::vector<Token>& log) {
    storage.PushAndOverflow(log);
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