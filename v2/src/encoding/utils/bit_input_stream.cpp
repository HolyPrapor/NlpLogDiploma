//
// Created by zeliboba on 2/17/24.
//

#include "bit_input_stream.hpp"

BitInputStream::BitInputStream(std::ifstream &input_stream) : num_bits_remaining_{0},
                                                                     current_byte_{0},
                                                                     is_closed_{false} {
    input_stream_.swap(input_stream);
}

std::optional<bool> BitInputStream::Read() {
    if (is_closed_) {
        return std::nullopt;
    }
    if (num_bits_remaining_ == 0) {
        char buffer[1];
        input_stream_.read(buffer, 1);
        if (input_stream_.gcount() == 0) {
            return std::nullopt;
        }
        current_byte_ = static_cast<unsigned char>(buffer[0]);
        num_bits_remaining_ = 8;
    }
    num_bits_remaining_ -= 1;
    bool bit = (current_byte_ >> num_bits_remaining_) & 1;
    return bit;
}

bool BitInputStream::ReadWithoutEOF() {
    auto bit = Read();
    if (bit.has_value()) {
        return bit.value();
    }
    throw std::runtime_error("Read from closed stream");
}

void BitInputStream::Close() {
    input_stream_.close();
    is_closed_ = true;
    num_bits_remaining_ = 0;
}