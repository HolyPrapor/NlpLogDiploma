//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_GREEDY_LOG_STORAGE_HPP
#define DIPLOMA_GREEDY_LOG_STORAGE_HPP

#include <list>
#include "log_storage.hpp"

class GreedyLogStorage : public LogStorage {
public:
    explicit GreedyLogStorage(int maxLogSize);

    void Store(const std::vector<Token>& log) override;
    std::optional<LogLink> TryLink(const std::vector<Token>& log, const int& startIndex) override;
    const std::vector<Token>& GetLog(int index) override;
    int GetSize() override;
private:
    std::list<std::vector<Token>> storage_;
    int maxLogSize_;
};

#endif //DIPLOMA_GREEDY_LOG_STORAGE_HPP
