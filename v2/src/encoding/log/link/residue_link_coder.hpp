//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_RESIDUE_LINK_CODER_HPP
#define DIPLOMA_RESIDUE_LINK_CODER_HPP

#include "log_link_coder.hpp"

class ResidueLinkEncoder : public LogLinkEncoder {
public:
    void EncodeLink(BitOutputStream& outputStream, const LogLink& link) override;
    void EncodeToken(BitOutputStream& outputStream, Token token) override;
};

class ResidueLinkDecoder : public LogLinkDecoder {
public:
    LogLink DecodeLink(BitInputStream& inputStream) override;
    Token DecodeToken(BitInputStream& inputStream) override;
};

#endif //DIPLOMA_RESIDUE_LINK_CODER_HPP
