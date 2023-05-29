#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <string>

struct Token {
    int value;
};

class Model {
public:
    virtual std::vector<Token> Preprocess(const std::string& text) = 0;
    virtual void Feed(const std::vector<Token>& next_tokens) = 0;
    virtual std::vector<std::vector<double>> GetProbabilities() = 0;
    virtual std::string Postprocess(const std::vector<Token>& tokens) = 0;
    virtual Token GetEndOfFileToken() = 0;

    std::vector<std::vector<int>> GetFrequencies() {
        auto probabilities = this->GetProbabilities();
        std::vector<std::vector<int>> frequencies(probabilities.size());
        for (auto& prob: probabilities) {
            frequencies.push_back(probabilities_to_frequencies(prob, 32));
        }
        return frequencies;
    }

private:
    static std::vector<int> probabilities_to_frequencies(std::vector<double> probabilities, int num_bits) {
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
};