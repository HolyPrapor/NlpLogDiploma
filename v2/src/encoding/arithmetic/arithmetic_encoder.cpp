//
// Created by zeliboba on 2/17/24.
//

#include "arithmetic_encoder.hpp"

ArithmeticEncoder::ArithmeticEncoder(const std::uint64_t &num_bits,
                                            std::unique_ptr<BitOutputStream> output_stream)
        : BaseCoder(num_bits), num_underflow_(0) {
    output_stream_ = std::move(output_stream);
}

void ArithmeticEncoder::Write(const std::vector<int> &frequencies, const Token &symbol) {
    Update(frequencies, symbol);
}

void ArithmeticEncoder::Finish() {
    output_stream_->Write(1);
    output_stream_->Close();
}

void ArithmeticEncoder::Shift() {
    output_stream_->Write(low_ >> (num_bits_ - 1));
    while (num_underflow_) {
        output_stream_->Write(0);
        num_underflow_--;
    }
}

void ArithmeticEncoder::Underflow() {
    num_underflow_ += 1;
}