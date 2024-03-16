//
// Created by zeliboba on 3/11/24.
//

#ifndef DIPLOMA_MODELLING_DECODER_HPP
#define DIPLOMA_MODELLING_DECODER_HPP

#include "generic_decoder.hpp"
#include "encoding/arithmetic/arithmetic_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "modelling/ppm_model.hpp"

class ModellingDecoder : public GenericDecoder {
public:
    explicit ModellingDecoder(std::unique_ptr<PPMDecoderModel>&& model, std::unique_ptr<ArithmeticDecoder>&& decoder);
    void Decode(BitOutputStream& data) override;
    Token Decode();
    void Feed(const Token& token);
    void Finish() override;

    static ModellingDecoder CreateDefault(const std::shared_ptr<BitInputStream> &inputStream);

private:
    std::unique_ptr<PPMDecoderModel> model;
    std::unique_ptr<ArithmeticDecoder> decoder;
};

#endif //DIPLOMA_MODELLING_DECODER_HPP
