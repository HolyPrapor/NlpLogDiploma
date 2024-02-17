#include "arithmetic_encoder.h"

inline ArithmeticEncoder::ArithmeticEncoder(const size_t &num_bits,
                                            std::unique_ptr<BitOutputStream> output_stream)
        : BaseCoder(num_bits) {
    output_stream_ = std::move(output_stream);
}

inline void
ArithmeticEncoder::Write(const std::vector<int> &frequencies, const unsigned char &symbol) {
    Update(frequencies, symbol);
}

inline void ArithmeticEncoder::Finish() {
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