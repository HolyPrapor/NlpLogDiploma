//
// Created by zeliboba on 5/30/24.
//

#ifndef DIPLOMA_ZSTD_ENCODER_HPP
#define DIPLOMA_ZSTD_ENCODER_HPP

#include "encoding/generic/generic_encoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "zstdstream.hpp"

class ZstdEncoder : public GenericEncoder {
public:
    explicit ZstdEncoder(const std::shared_ptr<BitOutputStream>& outputStream);
    ~ZstdEncoder() override = default;

    void Encode(BitInputStream& data) override;
    void Finish() override;
private:
    zstd::ostream zstdStream_;
    std::shared_ptr<BitOutputStream> outputStream_;
};

#endif //DIPLOMA_ZSTD_ENCODER_HPP
