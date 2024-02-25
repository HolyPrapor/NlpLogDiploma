//
// Created by zeliboba on 2/25/24.
//

#include "residue_link_coder.hpp"

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

void ResidueLinkEncoder::EncodeLink(BitOutputStream &outputStream, const LogLink &link) {
    encodeInt(outputStream, link.RecordIndex);
    encodeInt(outputStream, link.StartIndex);
    encodeInt(outputStream, link.Length);
}

void ResidueLinkEncoder::EncodeToken(BitOutputStream &outputStream, Token token) {
    encodeInt(outputStream, token);
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