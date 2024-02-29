//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_RESIDUE_LINK_ENCODER_HPP
#define DIPLOMA_RESIDUE_LINK_ENCODER_HPP

#include "log_link_encoder.hpp"

class ResidueLinkEncoder : public LogLinkEncoder {
public:
    void EncodeLink(BitOutputStream& outputStream, const LogLink& link) override;
    void EncodeToken(BitOutputStream& outputStream, Token token) override;
};



#endif //DIPLOMA_RESIDUE_LINK_ENCODER_HPP