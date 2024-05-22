//
// Created by zeliboba on 5/22/24.
//

#include "modelling_bwt_decoder.hpp"
#include "bwt_decoder.hpp"
#include "modelling_decoder.hpp"

ModellingBwtDecoder::ModellingBwtDecoder(const std::shared_ptr<BitInputStream> inputStream, int context_size, int chunkSize)
    : inputStream(inputStream), ppmBwtDecoder(
        [inputStream, context_size](std::shared_ptr<BitInputStream> inputStream1) {
            return ModellingDecoder::CreateDefault(inputStream1, context_size);
        },
        [inputStream, chunkSize](std::shared_ptr<BitInputStream> inputStream2) {
            return std::make_unique<BwtDecoder>(inputStream2, chunkSize);
        },
        inputStream
    ) {}

void ModellingBwtDecoder::Decode(BitOutputStream &data) {
    ppmBwtDecoder.Decode(data);
    data.Flush();
}

void ModellingBwtDecoder::Finish(BitOutputStream &data) {
    ppmBwtDecoder.Finish(data);
    data.Flush();
}