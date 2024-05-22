//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_BWT_DECODER_HPP
#define DIPLOMA_BWT_DECODER_HPP

#include "generic_decoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "utils/bwt.hpp"
#include "defs.hpp"

class BwtDecoder : public GenericDecoder {
public:
    explicit BwtDecoder(const std::shared_ptr<BitInputStream> inputStream, int chunkSize = 900000);
    ~BwtDecoder() override = default;

    void Decode(BitOutputStream& data) override;
    void Finish(BitOutputStream& data) override;
private:
    int chunkSize;
    BWT bwt;
    std::shared_ptr<BitInputStream> inputStream;
    std::vector<Token> buffer;
};

#endif //DIPLOMA_BWT_DECODER_HPP
