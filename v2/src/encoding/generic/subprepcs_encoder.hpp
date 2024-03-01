//
// Created by zeliboba on 3/1/24.
//

#ifndef DIPLOMA_SUBPREPCS_ENCODER_HPP
#define DIPLOMA_SUBPREPCS_ENCODER_HPP

#include <memory>
#include "generic_encoder.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/log/link/log_link_encoder.hpp"
#include "encoding/log/link/storage/log_storage.hpp"
#include "encoding/log/secondary/secondary_log_encoder.hpp"
#include "encoding/log/primary_log_encoder.hpp"

class SubPrePcsEncoder : GenericEncoder {
public:
    explicit SubPrePcsEncoder(std::unique_ptr<PrimaryLogEncoder>&& primaryEncoder, std::unique_ptr<SecondaryLogEncoder>&& secondaryEncoder,
                              std::unique_ptr<BitInputStream>&& primary, std::unique_ptr<BitInputStream>&& secondary, std::unique_ptr<BitInputStream>&& markup,
                              std::unique_ptr<GenericEncoder>&& primaryGenericEncoder, std::unique_ptr<GenericEncoder>&& secondaryGenericEncoder, std::unique_ptr<GenericEncoder>&& markupGenericEncoder);
    void Encode(BitInputStream& inputStream) override;

    static SubPrePcsEncoder CreateDefault(std::unique_ptr<BitOutputStream>&& primary, std::unique_ptr<BitOutputStream>&& secondary, std::unique_ptr<BitOutputStream>&& markup);

private:
    std::unique_ptr<PrimaryLogEncoder> primaryEncoder;
    std::unique_ptr<SecondaryLogEncoder> secondaryEncoder;
    std::unique_ptr<BitInputStream> primary;
    std::unique_ptr<BitInputStream> secondary;
    std::unique_ptr<BitInputStream> markup;
    std::unique_ptr<GenericEncoder> primaryGenericEncoder;
    std::unique_ptr<GenericEncoder> secondaryGenericEncoder;
    std::unique_ptr<GenericEncoder> markupGenericEncoder;
};

#endif //DIPLOMA_SUBPREPCS_ENCODER_HPP
