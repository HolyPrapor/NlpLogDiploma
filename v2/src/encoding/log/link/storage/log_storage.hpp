//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_LOG_STORAGE_HPP
#define DIPLOMA_LOG_STORAGE_HPP

#include <vector>
#include <optional>
#include "defs.hpp"
#include "encoding/log/link/log_link.hpp"

class LogStorage {
public:
    virtual void Store(const std::vector<Token>& log) = 0;
    virtual std::optional<LogLink> TryLink(const std::vector<Token>& log, const int& startIndex, const int& minLength = 1) = 0;
    virtual const std::vector<Token>& GetLog(int index) = 0;
    virtual int GetSize() = 0;
};

#endif //DIPLOMA_LOG_STORAGE_HPP
