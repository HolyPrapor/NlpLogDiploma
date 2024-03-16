//
// Created by zeliboba on 3/16/24.
//

#ifndef DIPLOMA_RESIDUE_SECONDARY_LOG_ENCODER_HPP
#define DIPLOMA_RESIDUE_SECONDARY_LOG_ENCODER_HPP

#include "secondary_log_encoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/generic/modelling_encoder.hpp"

class ResidueSecondaryLogEncoder : public SecondaryLogEncoder {
public:
    explicit ResidueSecondaryLogEncoder(std::shared_ptr<BitOutputStream> outputStream);

    void EncodeToken(const Token& token) override;

    void Feed(const std::vector<Token>& line, const int& start, const int& length) override;

    void FinishLine() override;

    void Finish() override;
private:
    std::shared_ptr<ModellingEncoder> encoder;
    std::shared_ptr<BitOutputStream> outputStream;
};

#endif //DIPLOMA_RESIDUE_SECONDARY_LOG_ENCODER_HPP
