//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_RESIDUE_LINK_DECODER_HPP
#define DIPLOMA_RESIDUE_LINK_DECODER_HPP

#include "log_link_decoder.hpp"

class ResidueLinkDecoder : public LogLinkDecoder {
public:
    LogLink DecodeLink(BitInputStream& inputStream) override;
    Token DecodeToken(BitInputStream& inputStream) override;
};

#endif //DIPLOMA_RESIDUE_LINK_DECODER_HPP
