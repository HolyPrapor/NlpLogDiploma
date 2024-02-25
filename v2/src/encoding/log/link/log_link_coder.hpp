//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_LOG_LINK_CODER_HPP
#define DIPLOMA_LOG_LINK_CODER_HPP

#include <vector>
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "defs.hpp"
#include "encoding/log/base_log_coder.hpp"

struct LogLink {
    int RecordIndex;
    int StartIndex;
    int Length;
};

class LogLinkEncoder {
public:
    virtual void EncodeLink(BitOutputStream& outputStream, const LogLink& link) = 0;
    virtual void EncodeToken(BitOutputStream& outputStream, Token token) = 0;
};

class LogLinkDecoder {
public:
    virtual LogLink DecodeLink(BitInputStream& inputStream) = 0;
    virtual DecodeResult<LogLink> DecodeLink(const std::vector<Token>& tokens, const int& startIndex) = 0;
    virtual Token DecodeToken(BitInputStream& inputStream) = 0;
    virtual DecodeResult<Token> DecodeToken(const std::vector<Token>& tokens, const int& startIndex) = 0;
};

#endif //DIPLOMA_LOG_LINK_CODER_HPP
