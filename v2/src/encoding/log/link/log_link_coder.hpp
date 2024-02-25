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
    void EncodeLink(BitOutputStream& outputStream, const LogLink& link);
    void EncodeToken(BitOutputStream& outputStream, Token token);
};

class LogLinkDecoder {
public:
    LogLink DecodeLink(BitInputStream& inputStream);
    DecodeResult<LogLink> DecodeLink(const std::vector<Token>& tokens, const int& startIndex);
    Token DecodeToken(BitInputStream& inputStream);
    DecodeResult<Token> DecodeToken(const std::vector<Token>& tokens, const int& startIndex);
};

#endif //DIPLOMA_LOG_LINK_CODER_HPP
