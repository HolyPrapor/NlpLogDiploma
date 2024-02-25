//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_BASE_LOG_CODER_HPP
#define DIPLOMA_BASE_LOG_CODER_HPP

#include <vector>
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "defs.hpp"

template <typename T>
class DecodeResult {
    T link;
    int NextIndex;
};

class LogEncoder {
    void EncodeLine(BitOutputStream& outputStream, const std::vector<Token>& line);
};

class LogDecoder {
    // todo: add online decoding
    // void DecodeLine(BitInputStream& inputStream, BitOutputStream& outputStream);
    // void DecodeLine(const std::vector<Token>& line, BitOutputStream& outputStream);

    std::vector<Token> DecodeLine(BitInputStream& inputStream);
    std::vector<Token> DecodeLine(const std::vector<Token>& line);
};

#endif //DIPLOMA_BASE_LOG_CODER_HPP
