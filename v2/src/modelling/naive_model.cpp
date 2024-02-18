//
// Created by zeliboba on 2/18/24.
//

#include "naive_model.hpp"

static std::vector<int> GetUniformFrequencies() {
    std::vector<int> distribution(BinaryAlphabetSize, 2);
    for (int i = 1; i < BinaryAlphabetSize; ++i) {
        distribution[i] += distribution[i - 1];
    }
    return distribution;
}

NaiveModel::NaiveModel() : NaiveModel(GetUniformFrequencies()) {
}

NaiveModel::NaiveModel(const std::vector<int>& frequencies) : frequencies_(frequencies) {
}

void NaiveModel::Feed(const Token& next_token) {
}

Distribution NaiveModel::GetCurrentDistribution() {
    return Distribution(std::make_unique<std::vector<int>>(frequencies_));
}