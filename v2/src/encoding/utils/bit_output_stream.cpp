//
// Created by zeliboba on 2/17/24.
//

#include "bit_output_stream.hpp"

BitOutputStream::BitOutputStream(std::ofstream &output_stream) : num_bits_filled_{0}, current_byte_{0}, is_closed_{false} {
    output_stream_.swap(output_stream);
}

void BitOutputStream::Write(const bool& bit) {
    if (is_closed_) {
        throw std::runtime_error("Write in closed stream");
    }
    current_byte_ = (current_byte_ << 1) | bit;
    num_bits_filled_ += 1;
    if (num_bits_filled_ == 8) {
        char buffer[1];
        buffer[0] = static_cast<char>(current_byte_);
        output_stream_.write(buffer, 1);
        num_bits_filled_ = 0;
    }
}

void BitOutputStream::Close() {
    while (num_bits_filled_ != 0) {
        Write(0);
    }
    output_stream_.close();
    is_closed_ = true;
}