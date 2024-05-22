//
// Created by zeliboba on 3/17/24.
//

#include "identity_decoder.hpp"

IdentityDecoder::IdentityDecoder(const std::shared_ptr<BitInputStream>& inputStream)
    : inputStream_(inputStream) {}


void IdentityDecoder::Decode(BitOutputStream& data) {
    while (!inputStream_->IsEOF()) {
        data.Write(inputStream_->ReadWithoutEOF());
    }
}
