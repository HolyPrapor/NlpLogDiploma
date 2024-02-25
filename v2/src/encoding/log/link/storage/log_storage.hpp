//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_LOG_STORAGE_HPP
#define DIPLOMA_LOG_STORAGE_HPP

#include <vector>
#include "defs.hpp"
#include "encoding/log/link/log_link_coder.hpp"

class LogStorage {
public:
    void Store(const std::vector<Token>& log);
    std::optional<LogLink> TryLink(const std::vector<Token>& log, const int& startIndex);
};

#endif //DIPLOMA_LOG_STORAGE_HPP
