//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_MODELLING_BWT_DECODER_HPP
#define DIPLOMA_MODELLING_BWT_DECODER_HPP

#include "generic_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "chained_generic_decoder.hpp"

class ModellingBwtDecoder : public GenericDecoder {
public:
    explicit ModellingBwtDecoder(const std::shared_ptr<BitInputStream> inputStream, int context_size = 3, int chunkSize = 900000);
    ~ModellingBwtDecoder() override = default;

    void Decode(BitOutputStream& data) override;
    void Finish(BitOutputStream& data) override;

private:
    std::shared_ptr<BitInputStream> inputStream;
    ChainedGenericDecoder ppmBwtDecoder;
};

#endif //DIPLOMA_MODELLING_BWT_DECODER_HPP
