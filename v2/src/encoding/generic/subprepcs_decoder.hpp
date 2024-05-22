//
// Created by zeliboba on 3/11/24.
//

#ifndef DIPLOMA_SUBPREPCS_DECODER_HPP
#define DIPLOMA_SUBPREPCS_DECODER_HPP

#include <memory>
#include <functional>
#include "generic_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/log/secondary/secondary_log_decoder.hpp"
#include "encoding/log/primary_log_decoder.hpp"
#include "encoding/log/link/log_link_encoder.hpp"

using PrimaryLogDecoderFactory = std::function<std::unique_ptr<PrimaryLogDecoder>(
        std::unique_ptr<LogLinkDecoder>, std::unique_ptr<LogStorage>, std::unique_ptr<SecondaryLogDecoder>,
        std::shared_ptr<BitInputStream>, std::shared_ptr<BitInputStream>)>;

using SecondaryLogDecoderFactory = std::function<std::unique_ptr<SecondaryLogDecoder>(std::shared_ptr<BitInputStream>)>;

using GenericDecoderFactory = std::function<std::unique_ptr<GenericDecoder>(std::shared_ptr<BitInputStream>)>;


class SubPrePcsDecoder : public GenericDecoder {
public:
    explicit SubPrePcsDecoder(std::unique_ptr<PrimaryLogDecoder>&& primaryDecoder, std::unique_ptr<SecondaryLogDecoder>&& secondaryDecoder,
                              std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup,
                              std::unique_ptr<GenericDecoder>&& primaryGenericDecoder, std::unique_ptr<GenericDecoder>&& secondaryGenericDecoder, std::unique_ptr<GenericDecoder>&& markupGenericDecoder);
    ~SubPrePcsDecoder() override = default;

    void Decode(BitOutputStream& data) override;

    static SubPrePcsDecoder Create(std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup,
                                   PrimaryLogDecoderFactory primaryFactory, SecondaryLogDecoderFactory secondaryFactory, GenericDecoderFactory primaryGenericFactory,
                                   GenericDecoderFactory secondaryGenericFactory, GenericDecoderFactory markupGenericFactory, std::unique_ptr<LogLinkDecoder> linkDecoder = nullptr,
                                   std::unique_ptr<LogStorage> storage = nullptr);

    static SubPrePcsDecoder CreateNaive(std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup,
                                        std::unique_ptr<LogLinkDecoder> linkDecoder = nullptr, std::unique_ptr<LogStorage> storage = nullptr);

    static SubPrePcsDecoder CreateResidue(std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup,
                                          std::unique_ptr<LogLinkDecoder> linkDecoder = nullptr, std::unique_ptr<LogStorage> storage = nullptr);

    static SubPrePcsDecoder CreatePPM(std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup,
                                      std::unique_ptr<LogLinkDecoder> linkDecoder = nullptr, std::unique_ptr<LogStorage> storage = nullptr);
    static SubPrePcsDecoder CreateBWTPPM(std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup,
                                         std::unique_ptr<LogLinkDecoder> linkDecoder = nullptr, std::unique_ptr<LogStorage> storage = nullptr);

private:
    std::unique_ptr<PrimaryLogDecoder> primaryDecoder;
    std::unique_ptr<SecondaryLogDecoder> secondaryDecoder;
    std::unique_ptr<GenericDecoder> primaryGenericDecoder;
    std::unique_ptr<GenericDecoder> secondaryGenericDecoder;
    std::unique_ptr<GenericDecoder> markupGenericDecoder;

    std::shared_ptr<BitOutputStream> primary;
    std::shared_ptr<BitOutputStream> secondary;
    std::shared_ptr<BitOutputStream> markup;
};

#endif //DIPLOMA_SUBPREPCS_DECODER_HPP
