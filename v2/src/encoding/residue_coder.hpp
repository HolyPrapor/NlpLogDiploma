//
// Created by zeliboba on 3/16/24.
//

#ifndef DIPLOMA_RESIDUE_CODER_HPP
#define DIPLOMA_RESIDUE_CODER_HPP

#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/utils/bit_input_stream.hpp"

class ResidueCoder {
public:
    static void EncodeInt(BitOutputStream &outputStream, int value, int maxValue) {
        if (value < 0) {
            throw std::invalid_argument("Value must be non-negative");
        }
        while (value >= maxValue) {
            outputStream.WriteByte(255);
            value -= maxValue;
        }
        outputStream.WriteByte(255 - maxValue + value);
    }

    static int DecodeInt(BitInputStream &inputStream, int maxValue) {
        int value = 0;
        int byte = inputStream.ReadByte();
        while (byte == 255) {
            value += maxValue;
            byte = inputStream.ReadByte();
        }
        value += byte - (255 - maxValue);
        return value;
    }
};

#endif //DIPLOMA_RESIDUE_CODER_HPP
