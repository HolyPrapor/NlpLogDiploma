//
// Created by zeliboba on 2/25/24.
//

#include "residue_link_decoder.hpp"

static int decodeInt(BitInputStream &inputStream, int maxValue) {
    int value = 0;
    int byte = inputStream.ReadByte();
    while (byte == 255) {
        value += maxValue;
        byte = inputStream.ReadByte();
    }
    value += byte - (255 - maxValue);
    return value;
}

ResidueLinkDecoder::ResidueLinkDecoder(int maxValue) : maxValue(maxValue) {}

LogLink ResidueLinkDecoder::DecodeLink(BitInputStream &inputStream) {
    int recordIndex = 0;
    int startIndex = 0;
    int length = 0;
    recordIndex = decodeInt(inputStream, maxValue);
    startIndex = decodeInt(inputStream, maxValue);
    length = decodeInt(inputStream, maxValue);
    return LogLink{recordIndex, startIndex, length};
}

Token ResidueLinkDecoder::DecodeToken(BitInputStream &inputStream) {
    return decodeInt(inputStream, maxValue);
}