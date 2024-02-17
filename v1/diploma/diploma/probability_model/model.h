#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <string>

struct Token {
    int value;

    explicit Token(int v) : value(v) {}
};

class Model {
public:
    virtual std::vector<Token> Preprocess(const std::string& text) = 0;
    virtual void Feed(const std::vector<Token>& next_tokens) = 0;
    virtual std::string Postprocess(const std::vector<Token>& tokens) = 0;
    virtual Token GetEndOfFileToken() = 0;

    // NOTE(HolyPrapor): either one of these two functions has to be implemented
    virtual std::vector<double> GetProbabilities() {
        auto frequencies = this->GetFrequencies();
        return frequencies_to_probabilities(frequencies, 32);
    }

    virtual std::vector<int> GetFrequencies() {
        auto probabilities = this->GetProbabilities();
        return probabilities_to_frequencies(probabilities, 32);
    }

private:
    static std::vector<int> probabilities_to_frequencies(std::vector<double>& probabilities, int num_bits) {
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

    static std::vector<double> frequencies_to_probabilities(std::vector<int>& frequencies, int num_bits) {
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
};