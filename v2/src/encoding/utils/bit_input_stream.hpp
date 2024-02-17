//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_BIT_INPUT_STREAM_H
#define NLPLOGDIPLOMA_BIT_INPUT_STREAM_H

#include <fstream>
#include <optional>

class BitInputStream {
private:
    std::ifstream input_stream_;
    size_t num_bits_remaining_;
    unsigned char current_byte_;
    bool is_closed_;

public:
    explicit BitInputStream(std::ifstream& input_stream);
    void Close();
    std::optional<bool> Read();
    bool ReadWithoutEOF();
};

#endif //NLPLOGDIPLOMA_BIT_INPUT_STREAM_H
