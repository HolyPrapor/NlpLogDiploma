//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_SECONDARY_LOG_DECODER_HPP
#define DIPLOMA_SECONDARY_LOG_DECODER_HPP

#include "encoding/log/secondary_log_coder.hpp"

class SecondaryLogDecoder : public SecondaryLogCoder {
public:
    virtual Token DecodeToken() = 0;
};

#endif //DIPLOMA_SECONDARY_LOG_DECODER_HPP
