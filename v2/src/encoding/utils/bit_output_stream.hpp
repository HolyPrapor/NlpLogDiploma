//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H
#define NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H

#include <fstream>

class BitOutputStream {
    std::ostream& output_stream_;
    size_t num_bits_filled_;
    unsigned char current_byte_;
    bool is_closed_;

public:
    explicit BitOutputStream(std::ostream& output_stream);
    ~BitOutputStream() {
        if (!is_closed_) {
            Close();
        }
    }
    void Write(const bool& bit);
    void Close();

    void WriteBytes(const int& value, const int& bitCount) {
        for (auto i = bitCount - 1; i >= 0; i--)
            Write((value >> i) & 1);
    }

    void WriteByte(const unsigned char& byte) {
        WriteBytes(byte, 8);
    }
};

#endif //NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H
