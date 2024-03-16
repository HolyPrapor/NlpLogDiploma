//
// Created by zeliboba on 3/11/24.
//

#ifndef DIPLOMA_SUBPREPCS_DECODER_HPP
#define DIPLOMA_SUBPREPCS_DECODER_HPP

#include <memory>
#include "generic_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/log/secondary/secondary_log_decoder.hpp"
#include "encoding/log/primary_log_decoder.hpp"

class SubPrePcsDecoder : public GenericDecoder {
public:
    explicit SubPrePcsDecoder(std::unique_ptr<PrimaryLogDecoder>&& primaryDecoder, std::unique_ptr<SecondaryLogDecoder>&& secondaryDecoder,
                              std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup,
                              std::unique_ptr<GenericDecoder>&& primaryGenericDecoder, std::unique_ptr<GenericDecoder>&& secondaryGenericDecoder, std::unique_ptr<GenericDecoder>&& markupGenericDecoder);

    void Decode(BitOutputStream& data) override;
    void Finish() override;

    static SubPrePcsDecoder CreateNaive(std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup);
    static SubPrePcsDecoder CreateResidue(std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup);

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
