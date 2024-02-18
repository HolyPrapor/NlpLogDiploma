//
// Created by zeliboba on 2/18/24.
//

#ifndef DIPLOMA_BASE_MODEL_HPP
#define DIPLOMA_BASE_MODEL_HPP

#include <vector>
#include <string>
#include "distribution.hpp"
#include "defs.hpp"

class BaseModel {
public:
    virtual std::vector<Token> TokenizeChunk(const std::vector<unsigned char>& chunk) {
        std::vector<Token> tokens;
        tokens.reserve(chunk.size());
        for (auto& byte : chunk) {
            tokens.push_back(byte);
        }
        return tokens;
    };

    virtual std::vector<Token> EndTokenization() {
        return {};
    }

    virtual std::vector<unsigned char> DetokenizeChunk(const std::vector<Token>& tokens) {
        std::vector<unsigned char> result;
        result.reserve(tokens.size());
        for (auto& token : tokens) {
            result.push_back(static_cast<unsigned char>(token));
        }
        return result;
    }

    virtual std::vector<unsigned char> EndDetokenization() {
        return {};
    }

    virtual Token GetEndOfFileToken() {
        return BinaryAlphabetSize - 1;
    }

    virtual void Feed(const Token& next_token) = 0;
    virtual Distribution GetCurrentDistribution() = 0;
};

#endif //DIPLOMA_BASE_MODEL_HPP
