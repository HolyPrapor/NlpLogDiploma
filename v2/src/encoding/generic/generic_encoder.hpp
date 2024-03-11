//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_GENERIC_ENCODER_HPP
#define DIPLOMA_GENERIC_ENCODER_HPP

#include "encoding/utils/bit_input_stream.hpp"

class GenericEncoder {
public:
    virtual void Encode(BitInputStream& data) = 0;
    virtual void Finish() {}
};

#endif //DIPLOMA_GENERIC_ENCODER_HPP
