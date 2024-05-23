//
// Created by zeliboba on 5/23/24.
//

#include "delta_link_encoder.hpp"
#include "encoding/residue_coder.hpp"
#include "encoding/log/zigzag_coder.hpp"

DeltaLinkEncoder::DeltaLinkEncoder(int maxValue) : maxValue(maxValue) {}

void DeltaLinkEncoder::EncodeLink(BitOutputStream &outputStream, const LogLink &link) {
    auto recordIndex = ZigzagCoder::Encode(link.RecordIndex - prevLink.RecordIndex);
    auto startIndex = ZigzagCoder::Encode(link.StartIndex - prevLink.StartIndex);
    auto length = ZigzagCoder::Encode(link.Length - prevLink.Length);
    ResidueCoder::EncodeInt(outputStream, recordIndex, maxValue);
    ResidueCoder::EncodeInt(outputStream, startIndex, maxValue);
    ResidueCoder::EncodeInt(outputStream, length, maxValue);
    prevLink = link;
}

void DeltaLinkEncoder::EncodeToken(BitOutputStream &outputStream, Token token) {
    ResidueCoder::EncodeInt(outputStream, token, maxValue);
}