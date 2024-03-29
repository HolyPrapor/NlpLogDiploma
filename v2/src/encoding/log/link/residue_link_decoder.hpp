//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_RESIDUE_LINK_DECODER_HPP
#define DIPLOMA_RESIDUE_LINK_DECODER_HPP

#include "log_link_decoder.hpp"

class ResidueLinkDecoder : public LogLinkDecoder {
public:
    explicit ResidueLinkDecoder(int maxValue = 255);
    LogLink DecodeLink(BitInputStream& inputStream) override;
    Token DecodeToken(BitInputStream& inputStream) override;

private:
    int maxValue;
};

#endif //DIPLOMA_RESIDUE_LINK_DECODER_HPP
