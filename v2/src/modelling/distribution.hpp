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
#include <memory>

class Distribution {
public:
    explicit Distribution(std::unique_ptr<const std::vector<int>>&& frequencies) : frequencies(std::move(frequencies)) {}
    explicit Distribution(std::unique_ptr<const std::vector<double>>&& probabilities) : probabilities(std::move(probabilities)) {}

    const std::vector<int>& Frequencies() {
        if (frequencies == nullptr) {
            frequencies = std::make_unique<std::vector<int>>(probabilities_to_frequencies(*probabilities));
        }
        return *frequencies;
    }

    const std::vector<double>& Probabilities() {
        if (probabilities == nullptr) {
            probabilities = std::make_unique<std::vector<double>>(frequencies_to_probabilities(*frequencies));
        }
        return *probabilities;
    }

private:
    std::unique_ptr<const std::vector<int>> frequencies;
    std::unique_ptr<const std::vector<double>> probabilities;

    static std::vector<int> probabilities_to_frequencies(const std::vector<double>& probabilities) {
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
