//
// Created by zeliboba on 2/18/24.
//

#ifndef DIPLOMA_DISTRIBUTION_HPP
#define DIPLOMA_DISTRIBUTION_HPP

#include <utility>
#include <vector>
#include <optional>
#include <complex>
#include <algorithm>
#include <numeric>

struct Distribution {
private:
    static const int num_bits = 32;

    std::optional<std::vector<int>> frequencies;
    std::optional<std::vector<double>> probabilities;

    static std::vector<int> probabilities_to_frequencies(std::vector<double> &probabilities) {
        std::vector<int> frequencies(probabilities.size(), 0);
        double factor = std::pow(2, num_bits) - 3;
        double cumulative = 0;
        std::transform(probabilities.begin(), probabilities.end(), frequencies.begin(),
                       [&cumulative, &factor](double prob) {
                           cumulative += std::ceil(prob * factor) + 2;
                           return static_cast<int>(cumulative);
                       }
        );
        return frequencies;
    }

    static std::vector<double> frequencies_to_probabilities(std::vector<int> &frequencies) {
        std::vector<double> probabilities(frequencies.size(), 0);
        double num_values = std::pow(2, num_bits);
        std::adjacent_difference(frequencies.begin(), frequencies.end(), probabilities.begin());
        probabilities[0] = frequencies[0] / num_values;  // adjust first element
        std::transform(probabilities.begin(), probabilities.end(), probabilities.begin(),
                       [&num_values](double freq) {
                           return freq / num_values;
                       }
        );
        return probabilities;
    }
public:
    explicit Distribution(std::vector<int>&& frequencies) : frequencies(std::move(frequencies)) {}
    explicit Distribution(std::vector<double>&& probabilities) : probabilities(std::move(probabilities)) {}

    std::vector<int> Frequencies() {
        if (frequencies.has_value()) {
            return frequencies.value();
        }
        return probabilities_to_frequencies(probabilities.value());
    }

    std::vector<double> Probabilities() {
        if (probabilities.has_value()) {
            return probabilities.value();
        }
        return frequencies_to_probabilities(frequencies.value());
    }
};

#endif //DIPLOMA_DISTRIBUTION_HPP
