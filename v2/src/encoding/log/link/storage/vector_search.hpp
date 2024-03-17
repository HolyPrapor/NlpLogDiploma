//
// Created by zeliboba on 3/17/24.
//

#ifndef DIPLOMA_VECTOR_SEARCH_HPP
#define DIPLOMA_VECTOR_SEARCH_HPP

#include <vector>
#include <optional>
#include <list>
#include "encoding/log/link/log_link.hpp"
#include "defs.hpp"

static std::optional<LogLink> TryLinkGreedily(const std::list<std::vector<Token>> &storage, const std::vector<Token>& log, const int& startIndex) {
    LogLink link{-1, -1, -1};
    auto logIndex = 0;
    for (auto &line : storage) {
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

#endif //DIPLOMA_VECTOR_SEARCH_HPP
