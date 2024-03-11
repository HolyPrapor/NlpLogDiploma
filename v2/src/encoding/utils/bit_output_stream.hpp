//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H
#define NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H

#include <fstream>
#include <memory>
#include <vector>

class BitOutputStream {
    std::shared_ptr<std::ostream> output_stream_;
    size_t num_bits_filled_;
    unsigned char current_byte_;
    bool is_closed_;

public:
    explicit BitOutputStream(std::shared_ptr<std::ostream> output_stream);
    ~BitOutputStream() {
        if (!is_closed_) {
            Close();
        }
    }
    void Write(const bool& bit);
    void Close();

    void WriteByte(const unsigned char& byte) {
        for (auto i = 0; i < 8; i++) {
            Write((byte >> (7 - i)) & 1);
        }
    }
    void Flush() {
        output_stream_->flush();
    }

    void WriteAll(const std::vector<unsigned char>& bytes) {
        for (auto byte : bytes) {
            WriteByte(byte);
        }
    }
};

#endif //NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H
