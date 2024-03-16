//
// Created by zeliboba on 3/11/24.
//

#include "modelling_decoder.hpp"

ModellingDecoder ModellingDecoder::CreateDefault(std::shared_ptr<BitInputStream> inputStream) {
    auto model = std::make_unique<PPMDecoderModel>(3);
    auto decoder = std::make_unique<ArithmeticDecoder>(32, std::move(inputStream));
    return ModellingDecoder(std::move(model), std::move(decoder));
}

ModellingDecoder::ModellingDecoder(std::unique_ptr<PPMDecoderModel>&& model, std::unique_ptr<ArithmeticDecoder>&& decoder)
    : model(std::move(model)), decoder(std::move(decoder)) {}

// todo: currently the whole file is read into memory, which is far from optimal
void ModellingDecoder::Decode(BitOutputStream &data) {
    std::vector<Token> tokens;

    while (true) {
        auto token = model->DecodeNextToken(*decoder);
        if (token == model->GetEndOfFileToken()) {
            break;
        }
        model->Feed(token);
        tokens.push_back(token);
    }

    auto buffer = model->DetokenizeChunk(tokens);
    data.WriteAll(buffer);
}

void ModellingDecoder::Feed(const Token &token) {
    model->Feed(token);
}

void ModellingDecoder::Finish() {
}