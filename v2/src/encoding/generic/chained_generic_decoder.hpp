//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_CHAINED_GENERIC_DECODER_HPP
#define DIPLOMA_CHAINED_GENERIC_DECODER_HPP

#include "generic_decoder.hpp"
#include "subprepcs_decoder.hpp"

class ChainedGenericDecoder : public GenericDecoder {
public:
    explicit ChainedGenericDecoder(GenericDecoderFactory factory1, GenericDecoderFactory factory2, const std::shared_ptr<BitInputStream> inputStream);
    ~ChainedGenericDecoder() override = default;

    void Decode(BitOutputStream& data) override;
    void Finish(BitOutputStream& data) override;
private:
    std::unique_ptr<GenericDecoder> decoder1;
    std::shared_ptr<BitOutputStream> proxyOutputStream;
    std::unique_ptr<GenericDecoder> decoder2;
};

#endif //DIPLOMA_CHAINED_GENERIC_DECODER_HPP
