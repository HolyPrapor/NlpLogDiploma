//
// Created by zeliboba on 2/28/24.
//

#include "naive_secondary_log_encoder.hpp"

NaiveSecondaryLogEncoder::NaiveSecondaryLogEncoder(std::unique_ptr<BitOutputStream> outputStream) : outputStream(std::move(outputStream)) {
}

void NaiveSecondaryLogEncoder::EncodeToken(const Token& token) {
    for (int i = 3; i >= 0; i--) {
        outputStream->WriteByte(token >> (i * 8));
    }
}

void NaiveSecondaryLogEncoder::Feed(const std::vector<Token>& line, const int& start, const int& length) {
}