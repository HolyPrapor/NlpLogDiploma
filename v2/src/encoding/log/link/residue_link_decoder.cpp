//
// Created by zeliboba on 2/25/24.
//

#include "residue_link_decoder.hpp"

static int decodeInt(BitInputStream &inputStream) {
    int value = 0;
    int byte = inputStream.ReadByte();
    while (byte == 255) {
        value += 255;
        byte = inputStream.ReadByte();
    }
    if (byte > 127) {
        value += byte - 128;
        byte = inputStream.ReadByte();
    }
    value += byte;
    return value;
}

LogLink ResidueLinkDecoder::DecodeLink(BitInputStream &inputStream) {
    int recordIndex = 0;
    int startIndex = 0;
    int length = 0;
    recordIndex = decodeInt(inputStream);
    startIndex = decodeInt(inputStream);
    length = decodeInt(inputStream);
    return LogLink{recordIndex, startIndex, length};
}

Token ResidueLinkDecoder::DecodeToken(BitInputStream &inputStream) {
    return decodeInt(inputStream);
}