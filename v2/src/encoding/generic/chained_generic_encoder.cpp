//
// Created by zeliboba on 5/22/24.
//

#include <sstream>
#include "chained_generic_encoder.hpp"

ChainedGenericEncoder::ChainedGenericEncoder(GenericEncoderFactory factory1, GenericEncoderFactory factory2, const std::shared_ptr<BitOutputStream> outputStream) {
    auto proxyStream = std::make_shared<std::stringstream>();
    proxyOutputStream = std::make_shared<BitOutputStream>(proxyStream);
    proxyInputStream = std::make_shared<BitInputStream>(proxyStream);

    encoder1 = factory1(proxyOutputStream);
    encoder2 = factory2(outputStream);
}

void ChainedGenericEncoder::Encode(BitInputStream& data) {
    encoder1->Encode(data);
    proxyOutputStream->Flush();
    encoder2->Encode(*proxyInputStream);
}

void ChainedGenericEncoder::Finish() {
    encoder1->Finish();
    proxyOutputStream->Flush();
    // this is needed to flush the bufferized data
    encoder2->Encode(*proxyInputStream);
    encoder2->Finish();
}