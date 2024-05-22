//
// Created by zeliboba on 5/22/24.
//

#include <sstream>
#include "chained_generic_decoder.hpp"

ChainedGenericDecoder::ChainedGenericDecoder(GenericDecoderFactory factory1, GenericDecoderFactory factory2, const std::shared_ptr<BitInputStream> inputStream) {
    auto proxyStream = std::make_shared<std::stringstream>();
    auto proxyInputStream = std::make_shared<BitInputStream>(proxyStream);
    proxyOutputStream = std::make_shared<BitOutputStream>(proxyStream);

    decoder1 = factory1(inputStream);
    decoder2 = factory2(proxyInputStream);
}

void ChainedGenericDecoder::Decode(BitOutputStream& data) {
    decoder1->Decode(*proxyOutputStream);
    proxyOutputStream->Flush();
    decoder2->Decode(data);
    data.Flush();
}

void ChainedGenericDecoder::Finish(BitOutputStream& data) {
    decoder1->Finish(*proxyOutputStream);
    proxyOutputStream->Flush();
    decoder2->Finish(data);
    data.Flush();
}