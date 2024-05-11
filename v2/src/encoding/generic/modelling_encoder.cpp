//
// Created by zeliboba on 3/11/24.
//

#include "modelling_encoder.hpp"

std::unique_ptr<ModellingEncoder> ModellingEncoder::CreateDefault(const std::shared_ptr<BitOutputStream> &outputStream) {
    auto model = std::make_unique<PPMEncoderModel>(5);
    auto encoder = std::make_unique<ArithmeticEncoder>(32, outputStream);
    return std::make_unique<ModellingEncoder>(std::move(model), std::move(encoder));
}

ModellingEncoder::ModellingEncoder(std::unique_ptr<PPMEncoderModel>&& model, std::unique_ptr<ArithmeticEncoder>&& encoder)
    : model(std::move(model)), encoder(std::move(encoder)) {}


void ModellingEncoder::Encode(BitInputStream &inputStream) {
    buffer.clear();
    inputStream.ReadAll(buffer);

    auto tokens = model->TokenizeChunk(buffer);
    for (Token token : tokens) {
        Encode(token);
    }
}

void ModellingEncoder::Encode(const Token &token) {
    model->Feed(token);
    model->EncodeNextToken(*encoder, token);
}

void ModellingEncoder::Feed(const Token &token) {
    model->Feed(token);
    model->UpdateTrie();
}

void ModellingEncoder::Finish() {
    auto lastToken = model->GetEndOfFileToken();
    model->Feed(lastToken);
    model->EncodeNextToken(*encoder, lastToken);
    encoder->Finish();
}