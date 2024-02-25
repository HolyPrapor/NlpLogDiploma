//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_BASE_LOG_CODER_HPP
#define DIPLOMA_BASE_LOG_CODER_HPP

#include <vector>
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "defs.hpp"

class LogEncoder {
    virtual void EncodeLine(const std::vector<Token>& line) = 0;
};

class LogDecoder {
    virtual std::vector<Token> DecodeLine() = 0;
};

#endif //DIPLOMA_BASE_LOG_CODER_HPP
