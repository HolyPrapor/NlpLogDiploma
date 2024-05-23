//
// Created by zeliboba on 5/22/24.
//

#include "modelling_bwt_encoder.hpp"

ModellingBwtEncoder::ModellingBwtEncoder(const std::shared_ptr<BitOutputStream> outputStream, int context_size, int chunkSize, int mtfDegree)
    : outputStream(outputStream), ppmBwtEncoder(
        [outputStream, chunkSize, mtfDegree](std::shared_ptr<BitOutputStream> outputStream1) {
            return std::make_unique<BwtEncoder>(outputStream1, chunkSize, mtfDegree);
        },
        [outputStream, context_size](std::shared_ptr<BitOutputStream> outputStream2) {
            return ModellingEncoder::CreateDefault(outputStream2, context_size);
        },
        outputStream
    ) {}

void ModellingBwtEncoder::Encode(BitInputStream &data) {
    ppmBwtEncoder.Encode(data);
}

void ModellingBwtEncoder::Finish() {
    ppmBwtEncoder.Finish();
}