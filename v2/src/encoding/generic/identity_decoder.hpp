//
// Created by zeliboba on 3/17/24.
//

#ifndef DIPLOMA_IDENTITY_DECODER_HPP
#define DIPLOMA_IDENTITY_DECODER_HPP

#include "generic_decoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/utils/bit_input_stream.hpp"

class IdentityDecoder : public GenericDecoder {
public:
    explicit IdentityDecoder(const std::shared_ptr<BitInputStream>& inputStream);

    void Decode(BitOutputStream& data) override;
    void Finish() override;

private:
    std::shared_ptr<BitInputStream> inputStream_;
};

#endif //DIPLOMA_IDENTITY_DECODER_HPP
