#pragma once
#include "bit_input_stream.cpp"

class ArithmeticDecoder : public BaseCoder {
private:
    std::unique_ptr<BitInputStream> input_stream_;
    size_t code_;

    size_t ReadCodeBit();
public:
    ArithmeticDecoder(const size_t& num_bits, std::unique_ptr<BitInputStream> input_stream);
    size_t Read(const std::vector<int>& frequencies);
    void Shift() override;
    void Underflow() override;
};