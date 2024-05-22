//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_CHAINED_GENERIC_ENCODER_HPP
#define DIPLOMA_CHAINED_GENERIC_ENCODER_HPP

#include "generic_encoder.hpp"
#include "subprepcs_encoder.hpp"

class ChainedGenericEncoder : public GenericEncoder {
public:
    explicit ChainedGenericEncoder(GenericEncoderFactory factory1, GenericEncoderFactory factory2, const std::shared_ptr<BitOutputStream> outputStream);
    ~ChainedGenericEncoder() override = default;

    void Encode(BitInputStream& data) override;
    void Finish() override;
private:
    std::unique_ptr<GenericEncoder> encoder1;
    std::shared_ptr<BitOutputStream> proxyOutputStream;
    std::shared_ptr<BitInputStream> proxyInputStream;
    std::unique_ptr<GenericEncoder> encoder2;
};

#endif //DIPLOMA_CHAINED_GENERIC_ENCODER_HPP
