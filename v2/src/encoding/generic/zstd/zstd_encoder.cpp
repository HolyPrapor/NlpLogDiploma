//
// Created by zeliboba on 5/30/24.
//

#include "zstd_encoder.hpp"

ZstdEncoder::ZstdEncoder(const std::shared_ptr<BitOutputStream>& outputStream)
    : outputStream_(outputStream), zstdStream_(*outputStream->Data()) {}

void ZstdEncoder::Encode(BitInputStream& data) {
    while (!data.IsEOF()) {
        zstdStream_ << data.ReadByte();
    }
}

void ZstdEncoder::Finish() {
    zstdStream_.flush();
    outputStream_->Close();
}