//
// Created by zeliboba on 5/22/24.
//

#include "bwt_decoder.hpp"
#include "encoding/residue_coder.hpp"

BwtDecoder::BwtDecoder(const std::shared_ptr<BitInputStream> inputStream, int chunkSize, int mtfDegree) : chunkSize(chunkSize), bwt(chunkSize, mtfDegree), inputStream(inputStream) {
    buffer.reserve(chunkSize);
}

void BwtDecoder::Decode(BitOutputStream& data) {
    while (!inputStream->IsEOF()) {
        auto byte = ResidueCoder::DecodeInt(*inputStream, 255);
        buffer.push_back(byte);
        if (buffer.size() == chunkSize) {
            auto decoded = bwt.Decode(buffer);
            for (auto token : decoded) {
                data.WriteByte(token);
            }
            buffer.clear();
        }
    }
    data.Flush();
}

void BwtDecoder::Finish(BitOutputStream& data) {
    if (!buffer.empty()) {
        auto decoded = bwt.Decode(buffer);
        for (auto token : decoded) {
            data.WriteByte(token);
        }
    }
    data.Flush();
}