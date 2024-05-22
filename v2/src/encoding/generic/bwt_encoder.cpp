//
// Created by zeliboba on 5/22/24.
//

#include "bwt_encoder.hpp"

BwtEncoder::BwtEncoder(const std::shared_ptr<BitOutputStream>& outputStream, int chunkSize) : chunkSize(chunkSize), bwt(chunkSize), outputStream(outputStream) {
    buffer.reserve(chunkSize);
}

void BwtEncoder::Encode(BitInputStream& data) {
    while (!data.IsEOF()) {
        auto byte = data.ReadByte();
        buffer.push_back(byte);
        if (buffer.size() == chunkSize) {
            auto encoded = bwt.Encode(buffer);
            for (auto token : encoded) {
                outputStream->WriteByte(token);
            }
            buffer.clear();
        }
    }
}

void BwtEncoder::Finish() {
    if (!buffer.empty()) {
        auto encoded = bwt.Encode(buffer);
        for (auto token : encoded) {
            outputStream->WriteByte(token);
        }
        buffer.clear();
    }

    outputStream->Close();
}