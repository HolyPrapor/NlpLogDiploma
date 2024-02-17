#include "arithmetic_decoder.h"

inline ArithmeticDecoder::ArithmeticDecoder(const size_t &num_bits,
                                            std::unique_ptr<BitInputStream> input_stream)
        : BaseCoder(num_bits), code_{0} {
    input_stream_ = std::move(input_stream);
    for (auto i = 0; i < num_bits_; ++i) {
        code_ = code_ << 1 | ReadCodeBit();
    }
}

inline size_t ArithmeticDecoder::Read(const std::vector<int> &frequencies) {
    const auto value = ((code_ - low_ + 1) * frequencies.back() - 1) / (high_ - low_ + 1);
    size_t start = 0;
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
    return start;
}

inline void ArithmeticDecoder::Shift() {
    code_ = ((code_ << 1) & state_mask_) | ReadCodeBit();
}

inline void ArithmeticDecoder::Underflow() {
    code_ = (code_ & half_range_) | ((code_ << 1) & (state_mask_ >> 1)) | ReadCodeBit();
}

inline size_t ArithmeticDecoder::ReadCodeBit() {
    auto bit = input_stream_->Read();
    return bit == -1 ? 0 : bit;
}