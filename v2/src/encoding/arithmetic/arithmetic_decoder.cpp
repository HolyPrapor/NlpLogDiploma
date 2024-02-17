//
// Created by zeliboba on 2/17/24.
//

#include "arithmetic_decoder.hpp"
ArithmeticDecoder::ArithmeticDecoder(const std::uint64_t &num_bits,
                                            std::unique_ptr<BitInputStream> input_stream)
        : BaseCoder(num_bits), code_{0} {
    input_stream_ = std::move(input_stream);
    for (auto i = 0; i < num_bits_; ++i) {
        code_ = code_ << 1 | ReadCodeBit();
    }
}

unsigned char ArithmeticDecoder::Read(const std::vector<int> &frequencies) {
    const auto value = ((code_ - low_ + 1) * frequencies.back() - 1) / (high_ - low_ + 1);
    std::uint64_t start = 0;
    auto end = frequencies.size();
    while (end - start > 1) {
        auto middle = (start + end) >> 1;
        auto low = middle > 0 ? frequencies[middle - 1] : 0;
        if (low > value) {
            end = middle;
        } else {
            start = middle;
        }
    }
    Update(frequencies, start);
    return static_cast<unsigned char>(start);
}

void ArithmeticDecoder::Shift() {
    code_ = ((code_ << 1) & state_mask_) | ReadCodeBit();
}

void ArithmeticDecoder::Underflow() {
    code_ = (code_ & half_range_) | ((code_ << 1) & (state_mask_ >> 1)) | ReadCodeBit();
}

bool ArithmeticDecoder::ReadCodeBit() {
    auto bit = input_stream_->Read();
    if (bit.has_value())
        return bit.value();
    return 0;
}