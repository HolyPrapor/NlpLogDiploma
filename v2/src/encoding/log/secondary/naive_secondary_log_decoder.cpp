//
// Created by zeliboba on 2/28/24.
//

#include "naive_secondary_log_decoder.hpp"

NaiveSecondaryLogDecoder::NaiveSecondaryLogDecoder(std::shared_ptr<BitInputStream> inputStream) : inputStream(std::move(inputStream)) {
}

Token NaiveSecondaryLogDecoder::DecodeToken() {
    auto token = inputStream->ReadByte() << 24 | inputStream->ReadByte() << 16 | inputStream->ReadByte() << 8 | inputStream->ReadByte();
    return token;
}

void NaiveSecondaryLogDecoder::Feed(const std::vector<Token>& line, const int& start, const int& length) {
}