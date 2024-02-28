//
// Created by zeliboba on 2/28/24.
//

#include "residue_link_encoder.hpp"

static void encodeInt(BitOutputStream &outputStream, int value) {
    if (value < 0) {
        throw std::invalid_argument("Value must be non-negative");
    }
    while (value > 255) {
        outputStream.WriteByte(255);
        value -= 255;
    }
    if (value > 127) {
        outputStream.WriteByte(value - 128);
        value -= 128;
    }
    outputStream.WriteByte(value);
}


void ResidueLinkEncoder::EncodeLink(BitOutputStream &outputStream, const LogLink &link) {
    encodeInt(outputStream, link.RecordIndex);
    encodeInt(outputStream, link.StartIndex);
    encodeInt(outputStream, link.Length);
}

void ResidueLinkEncoder::EncodeToken(BitOutputStream &outputStream, Token token) {
    encodeInt(outputStream, token);
}