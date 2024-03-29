//
// Created by zeliboba on 2/17/24.
//

#include "arithmetic_encoder.hpp"

ArithmeticEncoder::ArithmeticEncoder(const std::uint64_t &num_bits,
                                            std::shared_ptr<BitOutputStream> output_stream)
        : BaseCoder(num_bits), num_underflow_(0), output_stream_{std::move(output_stream)} {
}

void ArithmeticEncoder::Write(const std::vector<int> &frequencies, const Token &symbol) {
    Update(frequencies, symbol);
}

void ArithmeticEncoder::Finish() {
    output_stream_->Write(1);
    output_stream_->Close();
}

void ArithmeticEncoder::Shift() {
    auto bit = (low_ >> (num_bits_ - 1));
    output_stream_->Write(bit);
    while (num_underflow_) {
        output_stream_->Write(bit ^ 1);
        num_underflow_--;
    }
}

void ArithmeticEncoder::Underflow() {
    num_underflow_ += 1;
}