//
// Created by zeliboba on 2/18/24.
//

#ifndef DIPLOMA_DISTRIBUTION_HPP
#define DIPLOMA_DISTRIBUTION_HPP

#include <vector>
#include <optional>
#include <complex>
#include <algorithm>
#include <numeric>

// TODO(HolyPrapor): avoid copying of vectors
class Distribution {
public:
    explicit Distribution(const std::vector<int>& frequencies) : frequencies(frequencies) {}
    explicit Distribution(const std::vector<double>& probabilities) : probabilities(probabilities) {}

    const std::vector<int>& Frequencies() {
        if (!frequencies.has_value()) {
            frequencies = probabilities_to_frequencies(probabilities.value());
        }
        return frequencies.value();
    }

    const std::vector<double>& Probabilities() {
        if (!probabilities.has_value()) {
            probabilities = frequencies_to_probabilities(frequencies.value());
        }
        return probabilities.value();
    }

private:
    std::optional<std::vector<int>> frequencies;
    std::optional<std::vector<double>> probabilities;

    static std::vector<int> probabilities_to_frequencies(const std::vector<double> &probabilities) {
        std::vector<int> frequencies(probabilities.size(), 0);
        // 2 is added to each frequency to avoid zero frequencies, 1 is added to avoid ceiling errors
        int factor = INT32_MAX - 3 * probabilities.size();
        std::transform(probabilities.begin(), probabilities.end(), frequencies.begin(),
                       [&factor](double prob) {
                           return std::ceil(prob * factor) + 2;
                       }
        );
        std::partial_sum(frequencies.begin(), frequencies.end(), frequencies.begin());
        return frequencies;
    }

    static std::vector<double> frequencies_to_probabilities(const std::vector<int> &frequencies) {
        std::vector<double> probabilities(frequencies.size(), 0);
        double num_values = frequencies.back();
        std::adjacent_difference(frequencies.begin(), frequencies.end(), probabilities.begin());
        std::transform(probabilities.begin(), probabilities.end(), probabilities.begin(),
                       [&num_values](double freq) {
                           return freq / num_values;
                       }
        );
        return probabilities;
    }
};

#endif //DIPLOMA_DISTRIBUTION_HPP
