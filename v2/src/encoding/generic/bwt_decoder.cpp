//
// Created by zeliboba on 5/22/24.
//

#include "bwt_decoder.hpp"

BwtDecoder::BwtDecoder(const std::shared_ptr<BitInputStream>& inputStream, int chunkSize) : chunkSize(chunkSize), bwt(chunkSize), inputStream(inputStream) {
    buffer.reserve(chunkSize);
}

void BwtDecoder::Decode(BitOutputStream& data) {
    while (!inputStream->IsEOF()) {
        auto byte = inputStream->ReadByte();
        buffer.push_back(byte);
        if (buffer.size() == chunkSize) {
            auto decoded = bwt.Decode(buffer);
            for (auto token : decoded) {
                data.WriteByte(token);
            }
            buffer.clear();
        }
    }
}

void BwtDecoder::Finish(BitOutputStream& data) {
    if (!buffer.empty()) {
        auto decoded = bwt.Decode(buffer);
        for (auto token : decoded) {
            data.WriteByte(token);
        }
    }
}