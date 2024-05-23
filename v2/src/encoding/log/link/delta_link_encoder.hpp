//
// Created by zeliboba on 5/23/24.
//

#ifndef DIPLOMA_DELTA_LINK_ENCODER_HPP
#define DIPLOMA_DELTA_LINK_ENCODER_HPP

#include "log_link_encoder.hpp"

class DeltaLinkEncoder : public LogLinkEncoder {
public:
    explicit DeltaLinkEncoder(int maxValue = 255);
    ~DeltaLinkEncoder() override = default;
    void EncodeLink(BitOutputStream& outputStream, const LogLink& link) override;
    void EncodeToken(BitOutputStream& outputStream, Token token) override;

private:
    int maxValue;
    LogLink prevLink{0, 0, 0};
};

#endif //DIPLOMA_DELTA_LINK_ENCODER_HPP
