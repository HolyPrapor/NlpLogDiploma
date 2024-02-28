//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_GENERIC_CODER_HPP
#define DIPLOMA_GENERIC_CODER_HPP

#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/utils/bit_output_stream.hpp"

class GenericEncoder {
    virtual void Encode(BitInputStream& data) = 0;
};

class GenericDecoder {
    virtual void Decode(BitOutputStream& data) = 0;
};

#endif //DIPLOMA_GENERIC_CODER_HPP
