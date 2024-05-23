//
// Created by zeliboba on 5/23/24.
//

#include "delta_link_decoder.hpp"
#include "encoding/residue_coder.hpp"
#include "encoding/log/zigzag_coder.hpp"


DeltaLinkDecoder::DeltaLinkDecoder(int maxValue) : maxValue(maxValue) {}

LogLink DeltaLinkDecoder::DecodeLink(BitInputStream &inputStream) {
    auto recordIndex = ZigzagCoder::Decode(ResidueCoder::DecodeInt(inputStream, maxValue));
    auto startIndex = ZigzagCoder::Decode(ResidueCoder::DecodeInt(inputStream, maxValue));
    auto length = ZigzagCoder::Decode(ResidueCoder::DecodeInt(inputStream, maxValue));
    prevLink = {prevLink.RecordIndex + recordIndex, prevLink.StartIndex + startIndex, prevLink.Length + length};
    return prevLink;
}

Token DeltaLinkDecoder::DecodeToken(BitInputStream &inputStream) {
    return ResidueCoder::DecodeInt(inputStream, maxValue);
}