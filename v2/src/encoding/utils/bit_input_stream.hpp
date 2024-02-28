//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_BIT_INPUT_STREAM_H
#define NLPLOGDIPLOMA_BIT_INPUT_STREAM_H

#include <fstream>
#include <optional>

class BitInputStream {
private:
    std::istream& input_stream_;
    size_t num_bits_remaining_;
    unsigned char current_byte_;
    bool is_closed_;

public:
    explicit BitInputStream(std::istream& input_stream);
    ~BitInputStream() {
        if (!is_closed_) {
            Close();
        }
    }
    void Close();
    std::optional<bool> Read();
    bool ReadWithoutEOF();

    int ReadBytes(const int& bitCount) {
        int buffer = 0;
        for (auto i = 0; i < bitCount; i++)
            buffer = buffer << 1 | ReadWithoutEOF();
        return buffer;
    }

    unsigned char ReadByte() {
        return static_cast<unsigned char>(ReadBytes(8));
    }
};

#endif //NLPLOGDIPLOMA_BIT_INPUT_STREAM_H
