//
// Created by zeliboba on 5/30/24.
//

#ifndef DIPLOMA_ZSTD_DECODER_HPP
#define DIPLOMA_ZSTD_DECODER_HPP

#include "encoding/generic/generic_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "zstdstream.hpp"

class ZstdDecoder : public GenericDecoder {
public:
    explicit ZstdDecoder(const std::shared_ptr<BitInputStream>& inputStream);
    ~ZstdDecoder() override = default;

    void Decode(BitOutputStream& data) override;
    void Finish(BitOutputStream& data) override;
private:
    zstd::istream zstdStream_;
    std::shared_ptr<BitInputStream> inputStream_;
};

#endif //DIPLOMA_ZSTD_DECODER_HPP
