//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_RESIDUE_LINK_ENCODER_HPP
#define DIPLOMA_RESIDUE_LINK_ENCODER_HPP

#include "log_link_encoder.hpp"

class ResidueLinkEncoder : public LogLinkEncoder {
public:
    explicit ResidueLinkEncoder(int maxValue = 127);
    void EncodeLink(BitOutputStream& outputStream, const LogLink& link) override;
    void EncodeToken(BitOutputStream& outputStream, Token token) override;

private:
    int maxValue;
};



#endif //DIPLOMA_RESIDUE_LINK_ENCODER_HPP
