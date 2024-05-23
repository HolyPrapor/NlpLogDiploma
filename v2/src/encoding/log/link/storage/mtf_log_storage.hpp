//
// Created by zeliboba on 3/17/24.
//

#ifndef DIPLOMA_MTF_LOG_STORAGE_HPP
#define DIPLOMA_MTF_LOG_STORAGE_HPP

#include <list>
#include <memory>
#include "log_storage.hpp"
#include "log_filter.hpp"

class MtfLogStorage : public LogStorage {
public:
    explicit MtfLogStorage(int maxLogSize, std::unique_ptr<LogFilter>&& filter = nullptr);
    ~MtfLogStorage() override = default;

    void Store(const std::vector<Token>& log) override;
    std::optional<LogLink> TryLink(const std::vector<Token>& log, const int& startIndex, const int& minLength = 1) override;
    const std::vector<Token>& GetLog(int index) override;
    int GetSize() override;

private:
    std::list<std::vector<Token>> storage_;
    std::unique_ptr<LogFilter> filter;
    int maxLogSize_;
};

#endif //DIPLOMA_MTF_LOG_STORAGE_HPP
