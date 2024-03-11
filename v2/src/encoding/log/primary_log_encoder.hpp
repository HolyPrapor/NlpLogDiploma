//
// Created by zeliboba on 2/25/24.
//

#ifndef DIPLOMA_PRIMARY_LOG_ENCODER_HPP
#define DIPLOMA_PRIMARY_LOG_ENCODER_HPP

#include <memory>
#include "encoding/log/link/log_link_encoder.hpp"
#include "encoding/log/link/storage/log_storage.hpp"
#include "encoding/log/secondary/secondary_log_encoder.hpp"

class PrimaryLogEncoder {
public:
    explicit PrimaryLogEncoder(std::unique_ptr<LogLinkEncoder>&& linkEncoder, std::unique_ptr<LogStorage>&& storage, std::unique_ptr<SecondaryLogEncoder>&& secondaryLogEncoder, std::shared_ptr<BitOutputStream> mainOutputStream, std::shared_ptr<BitOutputStream> markupOutputStream, int minLinkLength = 4);
    void EncodeLine(const std::vector<Token>& line);

private:
    std::unique_ptr<LogLinkEncoder> linkEncoder;
    std::unique_ptr<LogStorage> storage;
    std::unique_ptr<SecondaryLogEncoder> secondaryLogEncoder;
    std::shared_ptr<BitOutputStream> mainOutputStream;
    std::shared_ptr<BitOutputStream> markupOutputStream;

    int minLinkLength;
};

#endif //DIPLOMA_PRIMARY_LOG_ENCODER_HPP
