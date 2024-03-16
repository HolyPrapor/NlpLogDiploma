//
// Created by zeliboba on 3/11/24.
//

#ifndef DIPLOMA_MODELLING_ENCODER_HPP
#define DIPLOMA_MODELLING_ENCODER_HPP

#include <memory>
#include "encoding/arithmetic/arithmetic_encoder.hpp"
#include "generic_encoder.hpp"
#include "modelling/ppm_model.hpp"

class ModellingEncoder : public GenericEncoder {
public:
    explicit ModellingEncoder(std::unique_ptr<PPMEncoderModel>&& model, std::unique_ptr<ArithmeticEncoder>&& encoder);
    void Encode(BitInputStream& inputStream) override;
    void Encode(const Token& token);
    void Feed(const Token& token);
    void Finish() override;

    static ModellingEncoder CreateDefault(const std::shared_ptr<BitOutputStream> &outputStream);

private:
    std::vector<unsigned char> buffer;
    std::unique_ptr<PPMEncoderModel> model;
    std::unique_ptr<ArithmeticEncoder> encoder;
};

#endif //DIPLOMA_MODELLING_ENCODER_HPP
