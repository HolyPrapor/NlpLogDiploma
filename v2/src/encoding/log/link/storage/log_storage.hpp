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
    virtual void Store(const std::vector<Token>& log) = 0;
    virtual std::optional<LogLink> TryLink(const std::vector<Token>& log, const int& startIndex) = 0;
};

#endif //DIPLOMA_LOG_STORAGE_HPP