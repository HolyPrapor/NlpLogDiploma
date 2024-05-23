//
// Created by zeliboba on 5/23/24.
//

#ifndef DIPLOMA_DELTA_LINK_DECODER_HPP
#define DIPLOMA_DELTA_LINK_DECODER_HPP

#include "log_link_decoder.hpp"

class DeltaLinkDecoder : public LogLinkDecoder {
public:
    explicit DeltaLinkDecoder(int maxValue = 255);
    ~DeltaLinkDecoder() override = default;
    LogLink DecodeLink(BitInputStream& inputStream) override;
    Token DecodeToken(BitInputStream& inputStream) override;

private:
    int maxValue;
    LogLink prevLink{0, 0, 0};
};

#endif //DIPLOMA_DELTA_LINK_DECODER_HPP
