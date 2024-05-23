//
// Created by zeliboba on 5/23/24.
//

#ifndef DIPLOMA_LOG_FILTER_HPP
#define DIPLOMA_LOG_FILTER_HPP

#include <vector>
#include "defs.hpp"

class LogFilter {
public:
    virtual ~LogFilter() = default;

    virtual bool Filter(const std::vector<Token>& log) = 0;
    virtual void Store(const std::vector<Token>& log) = 0;
    virtual void Remove(const std::vector<Token>& log) = 0;
};

#endif //DIPLOMA_LOG_FILTER_HPP
