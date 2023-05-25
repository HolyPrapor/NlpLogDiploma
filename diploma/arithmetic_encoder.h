#pragma once

#include <memory>
#include "base_coder.cpp"
#include "bit_output_stream.cpp"

class ArithmeticEncoder : public BaseCoder {
private:
    std::unique_ptr<BitOutputStream> output_stream_;
    size_t num_underflow_;
public:
    ArithmeticEncoder(const size_t &num_bits, std::unique_ptr<BitOutputStream> output_stream);

    void Write(const std::vector<int> &frequencies, const unsigned char &symbol);

    void Finish();

    void Shift() override;

    void Underflow() override;
};
