#include "bit_input_stream.h"

inline BitInputStream::BitInputStream(std::ifstream &input_stream) : num_bits_remaining_{0},
                                                                     current_byte_{0},
                                                                     is_closed_{false} {
    input_stream_.swap(input_stream);
}

inline int BitInputStream::Read() {
    if (is_closed_) {
        return -1;
    }
    if (num_bits_remaining_ == 0) {
        char buffer[1];
        input_stream_.read(buffer, 1);
        if (input_stream_.eof()) {
            return -1;
        }
        current_byte_ = static_cast<unsigned char>(buffer[0]);
        num_bits_remaining_ = 8;
    }
    num_bits_remaining_ -= 1;
    return (current_byte_ >> num_bits_remaining_) & 1;
}

inline int BitInputStream::ReadWithoutEOF() {
    auto bit = Read();
    if (bit != -1) {
        return bit;
    }
    throw std::runtime_error("Read from close stream");
}

inline void BitInputStream::Close() {
    input_stream_.close();
    is_closed_ = true;
    num_bits_remaining_ = 0;
}