//
// Created by zeliboba on 3/16/24.
//

#include "residue_secondary_log_encoder.hpp"
#include "encoding/residue_coder.hpp"

ResidueSecondaryLogEncoder::ResidueSecondaryLogEncoder(std::shared_ptr<BitOutputStream> outputStream) : outputStream(std::move(outputStream)) {
}

void ResidueSecondaryLogEncoder::EncodeToken(const Token& token) {
    ResidueCoder::EncodeInt(*outputStream, token, 255);
}

void ResidueSecondaryLogEncoder::Feed(const std::vector<Token>& line, const int& start, const int& length) {
}

void ResidueSecondaryLogEncoder::FinishLine() {
    outputStream->Flush();
}

void ResidueSecondaryLogEncoder::Finish() {
    outputStream->Close();
}