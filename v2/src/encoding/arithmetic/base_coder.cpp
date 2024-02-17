//
// Created by zeliboba on 2/17/24.
//

#include <stdexcept>
#include <limits>
#include "base_coder.hpp"

BaseCoder::BaseCoder() : BaseCoder(32) {
}

BaseCoder::BaseCoder(const std::uint64_t& num_bits) {
    if (num_bits < 1) {
        throw std::runtime_error("State size out of range");
    }
    num_bits_ = num_bits;
    full_range_ = static_cast<decltype(full_range_)>(1) << num_bits_;
    half_range_ = full_range_ >> 1;
    quarter_range_ = half_range_ >> 1;
    minimum_range_ = quarter_range_ + 2;
    maximum_total_ = std::min(std::numeric_limits<decltype(full_range_)>::max() / full_range_, minimum_range_);
    state_mask_ = full_range_ - 1;
    low_ = 0;
    high_ = state_mask_;
}

void BaseCoder::Update(const std::vector<int> &frequencies, const unsigned char &symbol) {
    const auto range_length = high_ - low_ + 1;
    const auto total = frequencies.back();
    const auto symbol_code = static_cast<std::uint64_t>(symbol);
    const auto low = low_;

    low_ = low + (symbol_code ? frequencies[symbol_code - 1] : 0) * range_length / total;
    high_ = low + (frequencies[symbol_code] * range_length / total) - 1;

    // While the highest bits are equal, shift them out
    while (((low_ ^ high_) & half_range_) == 0) {
        Shift();
        low_ = ((low_ << 1) & state_mask_);
        high_ = ((high_ << 1) & state_mask_) | 1;
    }

    // While the second highest bit of low is 1 and the second highest bit of high is 0, delete the second highest bit
    while ((low_ & ~high_ & quarter_range_) != 0) {
        Underflow();
        low_ = (low_ << 1) ^ half_range_;
        high_ = ((high_ ^ half_range_) << 1) | half_range_ | 1;
    }
}