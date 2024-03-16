//
// Created by zeliboba on 3/11/24.
//

#include "modelling_encoder.hpp"

ModellingEncoder ModellingEncoder::CreateDefault(std::shared_ptr<BitOutputStream> outputStream) {
    auto model = std::make_unique<PPMEncoderModel>(4);
    auto encoder = std::make_unique<ArithmeticEncoder>(32, std::move(outputStream));
    return ModellingEncoder(std::move(model), std::move(encoder));
}

ModellingEncoder::ModellingEncoder(std::unique_ptr<PPMEncoderModel>&& model, std::unique_ptr<ArithmeticEncoder>&& encoder)
    : model(std::move(model)), encoder(std::move(encoder)) {}


void ModellingEncoder::Encode(BitInputStream &inputStream) {
    buffer.clear();
    inputStream.ReadAll(buffer);

    auto tokens = model->TokenizeChunk(buffer);
    for (Token token : tokens) {
        model->Feed(token);
        model->EncodeNextToken(*encoder, token);
    }
}

void ModellingEncoder::Feed(const Token &token) {
    model->Feed(token);
}

void ModellingEncoder::ClearContext() {
    model->ClearContext();
}

void ModellingEncoder::Finish() {
    auto lastToken = model->GetEndOfFileToken();
    model->Feed(lastToken);
    model->EncodeNextToken(*encoder, lastToken);
    encoder->Finish();
}