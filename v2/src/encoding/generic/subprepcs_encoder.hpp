//
// Created by zeliboba on 3/1/24.
//

#ifndef DIPLOMA_SUBPREPCS_ENCODER_HPP
#define DIPLOMA_SUBPREPCS_ENCODER_HPP

#include <memory>
#include <functional>
#include "generic_encoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/log/link/log_link_encoder.hpp"
#include "encoding/log/link/storage/log_storage.hpp"
#include "encoding/log/secondary/secondary_log_encoder.hpp"
#include "encoding/log/primary_log_encoder.hpp"

using PrimaryLogEncoderFactory = std::function<std::unique_ptr<PrimaryLogEncoder>(
        std::unique_ptr<LogLinkEncoder>, std::unique_ptr<LogStorage>, std::unique_ptr<SecondaryLogEncoder>,
        std::shared_ptr<BitOutputStream>, std::shared_ptr<BitOutputStream>, int)>;

using SecondaryLogEncoderFactory = std::function<std::unique_ptr<SecondaryLogEncoder>(std::shared_ptr<BitOutputStream>)>;

using GenericEncoderFactory = std::function<std::unique_ptr<GenericEncoder>(std::shared_ptr<BitOutputStream>)>;


class SubPrePcsEncoder : public GenericEncoder {
public:
    explicit SubPrePcsEncoder(std::unique_ptr<PrimaryLogEncoder>&& primaryEncoder,
                              std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup,
                              std::unique_ptr<GenericEncoder>&& primaryGenericEncoder, std::unique_ptr<GenericEncoder>&& secondaryGenericEncoder, std::unique_ptr<GenericEncoder>&& markupGenericEncoder);
    ~SubPrePcsEncoder() override = default;
    void Encode(BitInputStream& inputStream) override;
    void Finish() override;

    static SubPrePcsEncoder Create(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup,
                                   PrimaryLogEncoderFactory primaryFactory, SecondaryLogEncoderFactory secondaryFactory, GenericEncoderFactory primaryGenericFactory, GenericEncoderFactory secondaryGenericFactory, GenericEncoderFactory markupGenericFactory,
                                   std::unique_ptr<LogLinkEncoder> linkEncoder = nullptr, std::unique_ptr<LogStorage> storage = nullptr);
    static SubPrePcsEncoder CreateNaive(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup, std::unique_ptr<LogLinkEncoder> linkEncoder = nullptr,
                                        std::unique_ptr<LogStorage> storage = nullptr);
    static SubPrePcsEncoder CreateResidue(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup, std::unique_ptr<LogLinkEncoder> linkEncoder = nullptr,
                                          std::unique_ptr<LogStorage> storage = nullptr);
    static SubPrePcsEncoder CreatePPM(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup, std::unique_ptr<LogLinkEncoder> linkEncoder = nullptr,
                                      std::unique_ptr<LogStorage> storage = nullptr);

private:
    std::unique_ptr<PrimaryLogEncoder> primaryEncoder;
    std::unique_ptr<GenericEncoder> primaryGenericEncoder;
    std::unique_ptr<GenericEncoder> secondaryGenericEncoder;
    std::unique_ptr<GenericEncoder> markupGenericEncoder;

    std::shared_ptr<BitInputStream> primary;
    std::shared_ptr<BitInputStream> secondary;
    std::shared_ptr<BitInputStream> markup;
};

#endif //DIPLOMA_SUBPREPCS_ENCODER_HPP
