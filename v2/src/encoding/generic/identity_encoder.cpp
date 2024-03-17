//
// Created by zeliboba on 3/17/24.
//

#include "identity_encoder.hpp"

IdentityEncoder::IdentityEncoder(const std::shared_ptr<BitOutputStream>& outputStream)
    : outputStream_(outputStream) {}

void IdentityEncoder::Encode(BitInputStream& data) {
    while (!data.IsEOF()) {
        outputStream_->Write(data.ReadWithoutEOF());
    }
}

void IdentityEncoder::Finish() {
    outputStream_->Close();
}