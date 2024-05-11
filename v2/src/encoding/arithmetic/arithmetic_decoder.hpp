//
// Created by zeliboba on 2/17/24.
//

#ifndef DIPLOMA_ARITHMETIC_DECODER_HPP
#define DIPLOMA_ARITHMETIC_DECODER_HPP

#include <memory>
#include "encoding/utils/bit_input_stream.hpp"
#include "base_coder.hpp"

class ArithmeticDecoder : public BaseCoder {
private:
    std::shared_ptr<BitInputStream> input_stream_;
    std::uint64_t code_;
    bool initialized_ = false;
    bool ReadCodeBit();

protected:
    void Shift() override;
    void Underflow() override;

public:
    ArithmeticDecoder(const std::uint64_t& num_bits, std::shared_ptr<BitInputStream> input_stream);
    ~ArithmeticDecoder() override = default;
    Token Read(const std::vector<int>& frequencies);
};

#endif //DIPLOMA_ARITHMETIC_DECODER_HPP
