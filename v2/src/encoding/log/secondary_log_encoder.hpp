//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_SECONDARY_LOG_ENCODER_HPP
#define DIPLOMA_SECONDARY_LOG_ENCODER_HPP

#include "encoding/log/secondary_log_coder.hpp"

class SecondaryLogEncoder : public SecondaryLogCoder {
public:
    virtual void EncodeToken(Token token) = 0;
};

#endif //DIPLOMA_SECONDARY_LOG_ENCODER_HPP
