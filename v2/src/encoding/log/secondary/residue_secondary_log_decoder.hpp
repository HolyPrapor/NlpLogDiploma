//
// Created by zeliboba on 3/16/24.
//

#ifndef DIPLOMA_RESIDUE_SECONDARY_LOG_DECODER_HPP
#define DIPLOMA_RESIDUE_SECONDARY_LOG_DECODER_HPP

#include <vector>
#include "secondary_log_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"

class ResidueSecondaryLogDecoder : public SecondaryLogDecoder {
public:
    explicit ResidueSecondaryLogDecoder(std::shared_ptr<BitInputStream> inputStream);
    ~ResidueSecondaryLogDecoder() override = default;

    Token DecodeToken() override;

    void Feed(const std::vector<Token>& line, const int& start, const int& length) override;

    void FinishLine() override;
private:

    std::shared_ptr<BitInputStream> inputStream;
};

#endif //DIPLOMA_RESIDUE_SECONDARY_LOG_DECODER_HPP
