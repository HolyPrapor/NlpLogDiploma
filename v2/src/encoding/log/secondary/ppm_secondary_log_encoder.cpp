//
// Created by zeliboba on 3/16/24.
//

#include "ppm_secondary_log_encoder.hpp"

PPMSecondaryLogEncoder::PPMSecondaryLogEncoder(const std::shared_ptr<BitOutputStream>& outputStream, int context_size) : outputStream(outputStream) {
    modellingEncoder = ModellingEncoder::CreateDefault(outputStream, context_size);
}

void PPMSecondaryLogEncoder::EncodeToken(const Token& token) {
    modellingEncoder->Encode(token);
}

void PPMSecondaryLogEncoder::Feed(const std::vector<Token>& line, const int& start, const int& length) {
    for (int i = start; i < start + length; i++) {
        modellingEncoder->Feed(line[i]);
    }
}

void PPMSecondaryLogEncoder::FinishLine() {
    outputStream->Flush();
}

void PPMSecondaryLogEncoder::Finish() {
    modellingEncoder->Finish();
}