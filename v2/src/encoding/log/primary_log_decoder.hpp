//
// Created by zeliboba on 2/28/24.
//

#ifndef DIPLOMA_PRIMARY_LOG_DECODER_HPP
#define DIPLOMA_PRIMARY_LOG_DECODER_HPP

#include <memory>
#include "encoding/log/link/log_link_decoder.hpp"
#include "encoding/log/link/storage/log_storage.hpp"
#include "encoding/log/secondary/secondary_log_decoder.hpp"

class PrimaryLogDecoder {
public:
    explicit PrimaryLogDecoder(std::unique_ptr<LogLinkDecoder>&& linkDecoder, std::unique_ptr<LogStorage>&& storage, std::unique_ptr<SecondaryLogDecoder>&& secondaryLogDecoder, std::shared_ptr<BitInputStream> mainInputStream, std::shared_ptr<BitInputStream> markupInputStream);
    std::vector<Token> DecodeLine();

private:
    std::unique_ptr<LogLinkDecoder> linkDecoder;
    std::unique_ptr<LogStorage> storage;
    std::unique_ptr<SecondaryLogDecoder> secondaryLogDecoder;
    std::shared_ptr<BitInputStream> mainInputStream;
    std::shared_ptr<BitInputStream> markupInputStream;
};

#endif //DIPLOMA_PRIMARY_LOG_DECODER_HPP
