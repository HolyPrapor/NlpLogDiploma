//
// Created by zeliboba on 3/1/24.
//

#ifndef DIPLOMA_GENERIC_DECODER_HPP
#define DIPLOMA_GENERIC_DECODER_HPP

#include "encoding/utils/bit_output_stream.hpp"

class GenericDecoder {
public:
    virtual ~GenericDecoder() = default;

    virtual void Decode(BitOutputStream& data) = 0;
    virtual void Finish(BitOutputStream& data) {}
};

#endif //DIPLOMA_GENERIC_DECODER_HPP
