//
// Created by zeliboba on 2/18/24.
//

#include <catch2/catch_test_macros.hpp>
#include "modelling/naive_model.hpp"

TEST_CASE("Naive model has sensible default frequencies", "[NaiveModel]") {
    NaiveModel model;
    auto distribution = model.GetCurrentDistribution();
    auto frequencies = distribution.Frequencies();
    for (int i = 0; i < BinaryAlphabetSize; ++i) {
        REQUIRE(frequencies[i] > 0);
        if (i > 0) {
            REQUIRE(frequencies[i] > frequencies[i - 1]);
        }
    }
}
