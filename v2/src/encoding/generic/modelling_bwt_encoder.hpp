//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_MODELLING_BWT_ENCODER_HPP
#define DIPLOMA_MODELLING_BWT_ENCODER_HPP

#include "generic_encoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "modelling_encoder.hpp"
#include "bwt_encoder.hpp"
#include "chained_generic_encoder.hpp"

class ModellingBwtEncoder : public GenericEncoder {
public:
    explicit ModellingBwtEncoder(const std::shared_ptr<BitOutputStream> outputStream, int context_size = 3, int chunkSize = 900000);
    ~ModellingBwtEncoder() override = default;

    void Encode(BitInputStream& data) override;
    void Finish() override;
private:
    std::shared_ptr<BitOutputStream> outputStream;
    ChainedGenericEncoder ppmBwtEncoder;
};

#endif //DIPLOMA_MODELLING_BWT_ENCODER_HPP
