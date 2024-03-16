//
// Created by zeliboba on 3/16/24.
//

#include "residue_secondary_log_decoder.hpp"
#include "encoding/residue_coder.hpp"

ResidueSecondaryLogDecoder::ResidueSecondaryLogDecoder(std::shared_ptr<BitInputStream> inputStream) : inputStream(std::move(inputStream)) {
}

Token ResidueSecondaryLogDecoder::DecodeToken() {
    return ResidueCoder::DecodeInt(*inputStream, 255);
}

void ResidueSecondaryLogDecoder::Feed(const std::vector<Token>& line, const int& start, const int& length) {
}

void ResidueSecondaryLogDecoder::FinishLine() {

}