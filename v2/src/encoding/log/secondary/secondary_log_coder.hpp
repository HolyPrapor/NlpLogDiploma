//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_SECONDARY_LOG_CODER_HPP
#define DIPLOMA_SECONDARY_LOG_CODER_HPP

#include <vector>
#include "defs.hpp"

class SecondaryLogCoder {
public:
    virtual ~SecondaryLogCoder() = default;

    virtual void Feed(const std::vector<Token>& line, const int& start, const int& length) = 0;
    virtual void FinishLine() {}
};

#endif //DIPLOMA_SECONDARY_LOG_CODER_HPP
