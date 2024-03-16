//
// Created by zeliboba on 3/16/24.
//

#ifndef DIPLOMA_PPM_SECONDARY_LOG_DECODER_HPP
#define DIPLOMA_PPM_SECONDARY_LOG_DECODER_HPP

#include <memory>
#include "secondary_log_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/generic/modelling_decoder.hpp"

class PPMSecondaryLogDecoder : public SecondaryLogDecoder {
public:
    explicit PPMSecondaryLogDecoder(const std::shared_ptr<BitInputStream>& inputStream);
    Token DecodeToken() override;
    void Feed(const std::vector<Token>& line, const int& start, const int& length) override;
    void FinishLine() override;
private:
    std::unique_ptr<ModellingDecoder> modellingDecoder;
};

#endif //DIPLOMA_PPM_SECONDARY_LOG_DECODER_HPP
