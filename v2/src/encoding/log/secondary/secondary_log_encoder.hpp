//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_SECONDARY_LOG_ENCODER_HPP
#define DIPLOMA_SECONDARY_LOG_ENCODER_HPP

#include "secondary_log_coder.hpp"

class SecondaryLogEncoder : public SecondaryLogCoder {
public:
    virtual void EncodeToken(const Token& token) = 0;
};

#endif //DIPLOMA_SECONDARY_LOG_ENCODER_HPP
