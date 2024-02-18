//
// Created by zeliboba on 2/18/24.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "modelling/naive_model.hpp"

TEST_CASE("Distribution correctly converts to probabilities", "[Distribution]") {
    std::vector<int> frequencies = {1, 3, 6, 10};
    const double total = frequencies.back();
    Distribution distribution(std::make_unique<std::vector<int>>(frequencies));
    auto probabilities = distribution.Probabilities();
    REQUIRE(probabilities.size() == frequencies.size());
    auto prev = 0;
    for (int i = 0; i < frequencies.size(); ++i) {
        REQUIRE_THAT(probabilities[i], Catch::Matchers::WithinRel((frequencies[i] - prev) / total));
        prev = frequencies[i];
    }
}

TEST_CASE("Distribution correctly converts to frequencies", "[Distribution]") {
    std::vector<double> probabilities = {0.1, 0.2, 0.3, 0.4};
    Distribution distribution(std::make_unique<std::vector<double>>(probabilities));
    auto frequencies = distribution.Frequencies();
    const auto total = frequencies.back();
    REQUIRE(frequencies.size() == probabilities.size());
    auto prev = 0;
    for (int i = 0; i < probabilities.size(); ++i) {
        double range = frequencies[i] - prev;
        REQUIRE(range >= 2);
        REQUIRE_THAT(range / total, Catch::Matchers::WithinRel(probabilities[i], 1e-8));
        prev = frequencies[i];
    }
}