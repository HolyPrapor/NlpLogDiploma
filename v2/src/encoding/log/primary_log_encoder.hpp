//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_PRIMARY_LOG_ENCODER_HPP
#define DIPLOMA_PRIMARY_LOG_ENCODER_HPP

#include <memory>
#include "encoding/log/link/log_link_encoder.hpp"
#include "encoding/log/link/storage/log_storage.hpp"
#include "secondary_log_encoder.hpp"

class PrimaryLogEncoder {
public:
    explicit PrimaryLogEncoder(LogLinkEncoder& linkEncoder, LogStorage& storage, SecondaryLogEncoder& secondaryLogEncoder, std::unique_ptr<BitOutputStream> mainOutputStream, std::unique_ptr<BitOutputStream> markupOutputStream, int minLinkLength = 4);
    void EncodeLine(const std::vector<Token>& line);

private:
    LogLinkEncoder &linkEncoder;
    LogStorage &storage;
    SecondaryLogEncoder &secondaryLogEncoder;
    std::unique_ptr<BitOutputStream> mainOutputStream;
    int minLinkLength;
    std::unique_ptr<BitOutputStream> markupOutputStream;
};

#endif //DIPLOMA_PRIMARY_LOG_ENCODER_HPP
