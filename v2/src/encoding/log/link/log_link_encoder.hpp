//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_LOG_LINK_ENCODER_HPP
#define DIPLOMA_LOG_LINK_ENCODER_HPP

#include "encoding/utils/bit_output_stream.hpp"
#include "defs.hpp"
#include "log_link.hpp"

class LogLinkEncoder {
public:
    virtual ~LogLinkEncoder() = default;

    virtual void EncodeLink(BitOutputStream& outputStream, const LogLink& link) = 0;
    virtual void EncodeToken(BitOutputStream& outputStream, Token token) = 0;
};

#endif //DIPLOMA_LOG_LINK_ENCODER_HPP
