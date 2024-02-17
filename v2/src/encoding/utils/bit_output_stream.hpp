//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H
#define NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H

#include <fstream>

class BitOutputStream {
    std::ofstream output_stream_;
    size_t num_bits_filled_;
    unsigned char current_byte_;
    bool is_closed_;

public:
    explicit BitOutputStream(std::ofstream& output_stream);
    void Write(const bool& bit);
    void Close();
};

#endif //NLPLOGDIPLOMA_BIT_OUTPUT_STREAM_H
