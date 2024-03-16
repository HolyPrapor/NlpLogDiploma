//
// Created by zeliboba on 3/16/24.
//

#include "ppm_secondary_log_encoder.hpp"

PPMSecondaryLogEncoder::PPMSecondaryLogEncoder(std::shared_ptr<BitOutputStream> outputStream) : outputStream(std::move(outputStream)) {
    modellingEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(this->outputStream));
}

void PPMSecondaryLogEncoder::EncodeToken(const Token& token) {
    modellingEncoder->Feed(token);
    modellingEncoder->Encode(token);
}

void PPMSecondaryLogEncoder::Feed(const std::vector<Token>& line, const int& start, const int& length) {
    for (int i = start; i < start + length; i++) {
        modellingEncoder->Feed(line[i]);
    }
}

void PPMSecondaryLogEncoder::FinishLine() {
    modellingEncoder->ClearContext();
}

void PPMSecondaryLogEncoder::Finish() {
    modellingEncoder->Finish();
    outputStream->Close();
}