#include "base_coder.h"


inline BaseCoder::BaseCoder() : BaseCoder(32) {
}

inline BaseCoder::BaseCoder(const size_t& num_bits) {
    if (num_bits < 1) {
        throw std::runtime_error("State size out of range");
    }
    num_bits_ = num_bits;
    full_range_ = 1 << num_bits_;
    half_range_ = full_range_ >> 1;
    quarter_range_ = half_range_ >> 1;
    minimum_range_ = quarter_range_ + 2;
    maximum_total_ = minimum_range_;
    state_mask_ = full_range_ - 1;
    low_ = 0;
    high_ = state_mask_;
}

inline void BaseCoder::Update(const std::vector<int> &frequencies, const unsigned char &symbol) {
    const auto range_length = high_ - low_ + 1;
    const auto total = frequencies.back();
    const auto symbol_code = static_cast<size_t>(symbol);
    const auto low = low_;

    low_ = low + symbol_code ? frequencies[symbol_code - 1] : 0 * range_length / total;
    high_ = low + (frequencies[symbol_code] * range_length / total) - 1;

    while (((low_ ^ high_) & half_range_) == 0) {
        Shift();
        low_ = ((low_ << 1) & state_mask_);
        high_ = ((high_ << 1) & state_mask_) | 1;
    }

    while ((low_ & ~high_ & quarter_range_) != 0) {
        Underflow();
        low_ = (low_ << 1) ^ half_range_;
        high_ = ((high_ ^ half_range_) << 1) | half_range_ | 1;
    }
}