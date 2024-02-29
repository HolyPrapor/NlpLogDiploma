//
// Created by zeliboba on 2/28/24.
//

#include "residue_link_encoder.hpp"

static void encodeInt(BitOutputStream &outputStream, int value, int maxValue) {
    if (value < 0) {
        throw std::invalid_argument("Value must be non-negative");
    }
    while (value >= maxValue) {
        outputStream.WriteByte(255);
        value -= maxValue;
    }
    outputStream.WriteByte(255 - maxValue + value);
}

ResidueLinkEncoder::ResidueLinkEncoder(int maxValue) : maxValue(maxValue) {}

void ResidueLinkEncoder::EncodeLink(BitOutputStream &outputStream, const LogLink &link) {
    encodeInt(outputStream, link.RecordIndex, maxValue);
    encodeInt(outputStream, link.StartIndex, maxValue);
    encodeInt(outputStream, link.Length, maxValue);
}

void ResidueLinkEncoder::EncodeToken(BitOutputStream &outputStream, Token token) {
    encodeInt(outputStream, token, maxValue);
}