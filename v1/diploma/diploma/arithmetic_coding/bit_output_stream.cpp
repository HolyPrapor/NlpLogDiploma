#include "bit_output_stream.h"

inline BitOutputStream::BitOutputStream(std::ofstream &output_stream) : num_bits_filled_{0},
                                                                        current_byte_{0} {
    output_stream_.swap(output_stream);
}

inline void BitOutputStream::Write(const size_t& bit) {
    if (is_closed_) {
        throw std::runtime_error("Write in closed stream");
    }
    if (bit != 0 && bit != 1) {
        throw std::runtime_error("Incorrect input");
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

inline void BitOutputStream::Close() {
    while (num_bits_filled_) {
        Write(0);
    }
    output_stream_.close();
    is_closed_ = true;
}