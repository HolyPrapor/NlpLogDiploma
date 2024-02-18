//
// Created by zeliboba on 2/18/24.
//

#ifndef DIPLOMA_PPM_MODEL_HPP
#define DIPLOMA_PPM_MODEL_HPP

#include <vector>
#include "base_model.hpp"
#include "encoding/arithmetic/arithmetic_encoder.hpp"
#include "encoding/arithmetic/arithmetic_decoder.hpp"

class PPMBaseModel : public BaseModel {
protected:
    struct Impl;
    std::unique_ptr<Impl> impl;

    std::vector<int> GetUniformFrequencies();
    Token GetEscapeToken();

public:
    int context_size, alphabet_size;
    explicit PPMBaseModel(int context_size, int alphabet_size = BinaryAlphabetSize);
    Token GetEndOfFileToken() override;
};

class PPMEncoderModel : public PPMBaseModel {
public:
    PPMEncoderModel(ArithmeticEncoder& coder, int context_size, int alphabet_size = BinaryAlphabetSize);
    void Feed(const Token& next_token) override;
    void UpdateTrie();
    Distribution GetCurrentDistribution() override;

private:
    ArithmeticEncoder& coder;
};

class PPMModelDecoder : public PPMBaseModel {
public:
    PPMModelDecoder(ArithmeticDecoder& decoder, int context_size, int alphabet_size = BinaryAlphabetSize);
    void Feed(const Token& next_token) override;
    Distribution GetCurrentDistribution() override;

private:
    ArithmeticDecoder& decoder;
    int current_context_size = 0;
};


#endif //DIPLOMA_PPM_MODEL_HPP
