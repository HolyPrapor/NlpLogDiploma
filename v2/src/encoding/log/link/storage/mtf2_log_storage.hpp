//
// Created by zeliboba on 5/23/24.
//

#ifndef DIPLOMA_MTF2_LOG_STORAGE_HPP
#define DIPLOMA_MTF2_LOG_STORAGE_HPP

#include <memory>
#include "log_storage.hpp"
#include "utils/mtf2_storage.hpp"
#include "log_filter.hpp"

class Mtf2LogStorage : public LogStorage {
public:
    explicit Mtf2LogStorage(int maxLogSize, std::unique_ptr<LogFilter>&& filter = nullptr, int staticMovementDegree = 1, bool stable = false);
    ~Mtf2LogStorage() override = default;

    void Store(const std::vector<Token>& log) override;
    std::optional<LogLink> TryLink(const std::vector<Token>& log, const int& startIndex, const int& minLength = 1) override;
    const std::vector<Token>& GetLog(int index) override;
    int GetSize() override;

private:
    std::unique_ptr<MTF2Storage<std::vector<Token>>> storage;
    std::unique_ptr<LogFilter> filter;
    int maxLogSize;
};

#endif //DIPLOMA_MTF2_LOG_STORAGE_HPP
