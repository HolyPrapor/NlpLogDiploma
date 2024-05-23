//
// Created by zeliboba on 5/22/24.
//

#include "bwt_encoder.hpp"
#include "encoding/residue_coder.hpp"

BwtEncoder::BwtEncoder(const std::shared_ptr<BitOutputStream> outputStream, int chunkSize, int mtfDegree) : chunkSize(chunkSize), bwt(chunkSize, mtfDegree), outputStream(outputStream) {
    buffer.reserve(chunkSize);
}

void BwtEncoder::Encode(BitInputStream& data) {
    while (!data.IsEOF()) {
        auto byte = data.ReadByte();
        buffer.push_back(byte);
        if (buffer.size() == chunkSize) {
            auto encoded = bwt.Encode(buffer);
            for (auto token : encoded) {
                ResidueCoder::EncodeInt(*outputStream, token, 255);
            }
            buffer.clear();
        }
    }
}

void BwtEncoder::Finish() {
    if (!buffer.empty()) {
        auto encoded = bwt.Encode(buffer);
        for (auto token : encoded) {
            ResidueCoder::EncodeInt(*outputStream, token, 255);
        }
        buffer.clear();
    }

    outputStream->Close();
}