//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_BASE_CODER_H
#define NLPLOGDIPLOMA_BASE_CODER_H

#include <vector>
#include <cstdint>
#include "defs.hpp"

class BaseCoder {
protected:
    std::uint64_t num_bits_;
    std::uint64_t full_range_;
    std::uint64_t half_range_;
    std::uint64_t quarter_range_;
    std::uint64_t minimum_range_;
    std::uint64_t maximum_total_;
    std::uint64_t state_mask_;
    std::uint64_t low_;
    std::uint64_t high_;

    virtual void Shift() = 0;
    virtual void Underflow() = 0;
public:
    BaseCoder();
    explicit BaseCoder(const std::uint64_t& num_bits);
    void Update(const std::vector<int>& frequencies, const Token& symbol);
};

#endif //NLPLOGDIPLOMA_BASE_CODER_H
