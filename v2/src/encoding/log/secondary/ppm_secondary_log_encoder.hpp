//
// Created by zeliboba on 3/16/24.
//

#ifndef DIPLOMA_PPM_SECONDARY_LOG_ENCODER_HPP
#define DIPLOMA_PPM_SECONDARY_LOG_ENCODER_HPP

#include "secondary_log_encoder.hpp"
#include "encoding/generic/modelling_encoder.hpp"

class PPMSecondaryLogEncoder : public SecondaryLogEncoder {
public:
    explicit PPMSecondaryLogEncoder(const std::shared_ptr<BitOutputStream>& outputStream);
    ~PPMSecondaryLogEncoder() override = default;

    void EncodeToken(const Token& token) override;
    void Feed(const std::vector<Token>& line, const int& start, const int& length) override;
    void FinishLine() override;
    void Finish() override;

private:
    std::shared_ptr<BitOutputStream> outputStream;
    std::unique_ptr<ModellingEncoder> modellingEncoder;
};

#endif //DIPLOMA_PPM_SECONDARY_LOG_ENCODER_HPP
