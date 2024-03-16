//
// Created by zeliboba on 3/16/24.
//

#include "ppm_secondary_log_decoder.hpp"
#include "encoding/generic/modelling_decoder.hpp"

PPMSecondaryLogDecoder::PPMSecondaryLogDecoder(const std::shared_ptr<BitInputStream> &inputStream) {
    modellingDecoder = std::make_unique<ModellingDecoder>(ModellingDecoder::CreateDefault(inputStream));
}

Token PPMSecondaryLogDecoder::DecodeToken() {
    return modellingDecoder->Decode();
}

void PPMSecondaryLogDecoder::Feed(const std::vector<Token>& line, const int& start, const int& length) {
    for (int i = start; i < start + length; i++) {
        modellingDecoder->Feed(line[i]);
    }
}

void PPMSecondaryLogDecoder::FinishLine() {
//    modellingDecoder->ClearContext();
}