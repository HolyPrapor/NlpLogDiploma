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

class SubPrePcsEncoder : public GenericEncoder {
public:
    explicit SubPrePcsEncoder(std::unique_ptr<PrimaryLogEncoder>&& primaryEncoder,
                              std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup,
                              std::unique_ptr<GenericEncoder>&& primaryGenericEncoder, std::unique_ptr<GenericEncoder>&& secondaryGenericEncoder, std::unique_ptr<GenericEncoder>&& markupGenericEncoder);
    void Encode(BitInputStream& inputStream) override;
    void Finish() override;

    static SubPrePcsEncoder CreateNaive(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup);
    static SubPrePcsEncoder CreateResidue(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup);

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
