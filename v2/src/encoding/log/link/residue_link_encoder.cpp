//
// Created by zeliboba on 2/28/24.
//

#include "residue_link_encoder.hpp"
#include "encoding/residue_coder.hpp"

ResidueLinkEncoder::ResidueLinkEncoder(int maxValue) : maxValue(maxValue) {}

void ResidueLinkEncoder::EncodeLink(BitOutputStream &outputStream, const LogLink &link) {
    ResidueCoder::EncodeInt(outputStream, link.RecordIndex, maxValue);
    ResidueCoder::EncodeInt(outputStream, link.StartIndex, maxValue);
    ResidueCoder::EncodeInt(outputStream, link.Length, maxValue);
}

void ResidueLinkEncoder::EncodeToken(BitOutputStream &outputStream, Token token) {
    ResidueCoder::EncodeInt(outputStream, token, maxValue);
}