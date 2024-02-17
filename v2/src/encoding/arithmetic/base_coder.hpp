//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_BASE_CODER_H
#define NLPLOGDIPLOMA_BASE_CODER_H

#include <cstdio>
#include <vector>

class BaseCoder {
protected:
    size_t num_bits_;
    size_t full_range_;
    size_t half_range_;
    size_t quarter_range_;
    size_t minimum_range_;
    size_t maximum_total_;
    size_t state_mask_;
    size_t low_;
    size_t high_;

    virtual void Shift() = 0;
    virtual void Underflow() = 0;
public:
    BaseCoder();
    explicit BaseCoder(const size_t& num_bits);
    void Update(const std::vector<int>& frequencies, const unsigned char& symbol);
};

#endif //NLPLOGDIPLOMA_BASE_CODER_H
