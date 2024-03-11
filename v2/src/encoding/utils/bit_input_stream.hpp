//
// Created by zeliboba on 2/17/24.
//

#ifndef NLPLOGDIPLOMA_BIT_INPUT_STREAM_H
#define NLPLOGDIPLOMA_BIT_INPUT_STREAM_H

#include <fstream>
#include <optional>
#include <memory>
#include <vector>

class BitInputStream {
private:
    std::shared_ptr<std::istream> input_stream_;
    size_t num_bits_remaining_;
    unsigned char current_byte_;
    bool is_closed_;

public:
    explicit BitInputStream(std::shared_ptr<std::istream> input_stream);
    ~BitInputStream() {
        if (!is_closed_) {
            Close();
        }
    }
    void Close();
    std::optional<bool> Read();
    bool ReadWithoutEOF();

    unsigned char ReadByte() {
        unsigned char result = 0;
        for (auto i = 0; i < 8; i++) {
            result = (result << 1) | ReadWithoutEOF();
        }
        return result;
    }

    void ReadAll(std::vector<unsigned char>& buffer) {
        input_stream_->clear();
        while (!IsEOF()) {
            buffer.push_back(ReadByte());
        }
    }

    [[nodiscard]] bool IsEOF() const {
        // todo: optimize peeks and readings
        // peak and then check eof to avoid reading from closed stream
        input_stream_->peek();
        auto eof = input_stream_->eof();
        if (eof) {
            input_stream_->clear();
        }
        return eof;
    }
};

#endif //NLPLOGDIPLOMA_BIT_INPUT_STREAM_H
