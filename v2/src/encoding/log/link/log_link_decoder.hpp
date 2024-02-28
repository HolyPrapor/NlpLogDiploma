//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_LOG_LINK_DECODER_HPP
#define DIPLOMA_LOG_LINK_DECODER_HPP

#include "encoding/utils/bit_input_stream.hpp"
#include "defs.hpp"
#include "log_link.hpp"

class LogLinkDecoder {
public:
    virtual LogLink DecodeLink(BitInputStream& inputStream) = 0;
    virtual Token DecodeToken(BitInputStream& inputStream) = 0;
};

#endif //DIPLOMA_LOG_LINK_DECODER_HPP
