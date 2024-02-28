//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_PRIMARY_LOG_DECODER_HPP
#define DIPLOMA_PRIMARY_LOG_DECODER_HPP

#include <memory>
#include "encoding/log/link/log_link_decoder.hpp"
#include "encoding/log/link/storage/log_storage.hpp"
#include "secondary_log_decoder.hpp"

class PrimaryLogDecoder {
public:
    explicit PrimaryLogDecoder(LogLinkDecoder& linkDecoder, LogStorage& storage, SecondaryLogDecoder& secondaryLogDecoder, std::unique_ptr<BitInputStream> mainInputStream, std::unique_ptr<BitInputStream> markupInputStream);
    std::vector<Token> DecodeLine();

private:
    LogLinkDecoder &linkDecoder;
    LogStorage &storage;
    SecondaryLogDecoder &secondaryLogDecoder;
    std::unique_ptr<BitInputStream> mainInputStream;
    std::unique_ptr<BitInputStream> markupInputStream;
};

#endif //DIPLOMA_PRIMARY_LOG_DECODER_HPP
