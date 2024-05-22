//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_BWT_ENCODER_HPP
#define DIPLOMA_BWT_ENCODER_HPP

#include "generic_encoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "utils/bwt.hpp"

class BwtEncoder : public GenericEncoder {
public:
    explicit BwtEncoder(const std::shared_ptr<BitOutputStream> outputStream, int chunkSize = 900000);
    ~BwtEncoder() override = default;

    void Encode(BitInputStream& data) override;
    void Finish() override;
private:
    int chunkSize;
    BWT bwt;
    std::shared_ptr<BitOutputStream> outputStream;
    std::vector<Token> buffer;
};

#endif //DIPLOMA_BWT_ENCODER_HPP
