#pragma once

#include <fstream>

class BitInputStream {
private:
    std::ifstream input_stream_;
    size_t num_bits_remaining_;
    unsigned char current_byte_;
    bool is_closed_;

public:
    explicit BitInputStream(std::ifstream& input_stream);
    void Close();
    // TODO: think about return type
    int Read();
    int ReadWithoutEOF();
};