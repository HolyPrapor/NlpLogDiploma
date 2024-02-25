//
// Created by zeliboba on 2/18/24.
//

#ifndef DIPLOMA_PPM_MODEL_HPP
#define DIPLOMA_PPM_MODEL_HPP

#include <vector>
#include "base_model.hpp"

class PPMBaseModel : public BaseModel {
private:
    std::vector<int> GetUniformFrequencies();

protected:
    struct Impl;
    std::unique_ptr<Impl> impl;

    Token GetEscapeToken() const;
    void UpdateTrie();

public:
    int context_size, alphabet_size;
    explicit PPMBaseModel(int context_size, int alphabet_size = BinaryAlphabetSize);
    ~PPMBaseModel();
    Token GetEndOfFileToken() override;
    Distribution GetCurrentDistribution() override;
    void Feed(const Token& next_token) override;
};

class PPMEncoderModel : public PPMBaseModel {
public:
    explicit PPMEncoderModel(int context_size, int alphabet_size = BinaryAlphabetSize);
    void EncodeNextToken(ArithmeticEncoder& encoder, const Token& token) override;
    Token DecodeNextToken(ArithmeticDecoder& decoder) override;
};

class PPMModelDecoder : public PPMBaseModel {
public:
    explicit PPMModelDecoder(int context_size, int alphabet_size = BinaryAlphabetSize);
    void EncodeNextToken(ArithmeticEncoder& encoder, const Token& token) override;
    Token DecodeNextToken(ArithmeticDecoder& decoder) override;

private:
    int current_context_size = 0;
};


#endif //DIPLOMA_PPM_MODEL_HPP
