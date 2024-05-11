//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_ARITHMETIC_ENCODER_H
#define NLPLOGDIPLOMA_ARITHMETIC_ENCODER_H

#include <memory>
#include "base_coder.hpp"
#include "encoding/utils/bit_output_stream.hpp"

class ArithmeticEncoder : public BaseCoder {
private:
    std::shared_ptr<BitOutputStream> output_stream_;
    std::uint64_t num_underflow_;

protected:
    void Shift() override;
    void Underflow() override;

public:
    ArithmeticEncoder(const std::uint64_t &num_bits, std::shared_ptr<BitOutputStream> output_stream);
    ~ArithmeticEncoder() override = default;

    void Write(const std::vector<int> &frequencies, const Token &symbol);
    void Finish();
};

#endif //NLPLOGDIPLOMA_ARITHMETIC_ENCODER_H
