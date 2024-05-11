//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_NAIVE_SECONDARY_LOG_DECODER_HPP
#define DIPLOMA_NAIVE_SECONDARY_LOG_DECODER_HPP

#include <memory>
#include <vector>
#include "encoding/utils/bit_input_stream.hpp"
#include "defs.hpp"
#include "secondary_log_decoder.hpp"

class NaiveSecondaryLogDecoder : public SecondaryLogDecoder {
public:
    explicit NaiveSecondaryLogDecoder(std::shared_ptr<BitInputStream> inputStream);
    ~NaiveSecondaryLogDecoder() override = default;

    Token DecodeToken() override;

    void Feed(const std::vector<Token>& line, const int& start, const int& length) override;

private:
    std::shared_ptr<BitInputStream> inputStream;
};

#endif //DIPLOMA_NAIVE_SECONDARY_LOG_DECODER_HPP
