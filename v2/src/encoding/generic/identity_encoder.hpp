//
// Created by zeliboba on 3/17/24.
//

#ifndef DIPLOMA_IDENTITY_ENCODER_HPP
#define DIPLOMA_IDENTITY_ENCODER_HPP

#include "generic_encoder.hpp"
#include "defs.hpp"
#include "encoding/utils/bit_output_stream.hpp"

class IdentityEncoder : public GenericEncoder {
public:
    explicit IdentityEncoder(const std::shared_ptr<BitOutputStream>& outputStream);

    void Encode(BitInputStream& data) override;
    void Finish() override;
private:
    std::shared_ptr<BitOutputStream> outputStream_;
};

#endif //DIPLOMA_IDENTITY_ENCODER_HPP
