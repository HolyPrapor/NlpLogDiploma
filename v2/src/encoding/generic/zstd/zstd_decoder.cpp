//
// Created by zeliboba on 5/30/24.
//

#include "zstd_decoder.hpp"

ZstdDecoder::ZstdDecoder(const std::shared_ptr<BitInputStream>& inputStream)
    : inputStream_(inputStream), zstdStream_(*inputStream->Data()) {}

void ZstdDecoder::Decode(BitOutputStream& data) {
    char ch;
    while (zstdStream_.get(ch)) {
        data.WriteByte(ch);
    }
    data.Flush();
}

void ZstdDecoder::Finish(BitOutputStream& data) {
    Decode(data);
    data.Flush();
}