//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_NAIVE_SECONDARY_LOG_ENCODER_HPP
#define DIPLOMA_NAIVE_SECONDARY_LOG_ENCODER_HPP

#include <memory>
#include "secondary_log_encoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"

class NaiveSecondaryLogEncoder : public SecondaryLogEncoder {
public:
    explicit NaiveSecondaryLogEncoder(std::shared_ptr<BitOutputStream> outputStream);

    void EncodeToken(const Token& token) override;

    void Feed(const std::vector<Token>& line, const int& start, const int& length) override;

    void FinishLine() override;
private:
    std::shared_ptr<BitOutputStream> outputStream;
};

#endif //DIPLOMA_NAIVE_SECONDARY_LOG_ENCODER_HPP
