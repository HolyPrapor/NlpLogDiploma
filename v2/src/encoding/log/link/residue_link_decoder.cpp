//
// Created by zeliboba on 2/25/24.
//

#include "residue_link_decoder.hpp"
#include "encoding/residue_coder.hpp"

ResidueLinkDecoder::ResidueLinkDecoder(int maxValue) : maxValue(maxValue) {}

LogLink ResidueLinkDecoder::DecodeLink(BitInputStream &inputStream) {
    int recordIndex = 0;
    int startIndex = 0;
    int length = 0;
    recordIndex = ResidueCoder::DecodeInt(inputStream, maxValue);
    startIndex = ResidueCoder::DecodeInt(inputStream, maxValue);
    length = ResidueCoder::DecodeInt(inputStream, maxValue);
    return LogLink{recordIndex, startIndex, length};
}

Token ResidueLinkDecoder::DecodeToken(BitInputStream &inputStream) {
    return ResidueCoder::DecodeInt(inputStream, maxValue);
}