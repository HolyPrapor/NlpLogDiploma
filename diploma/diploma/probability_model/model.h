#include <vector>
#include <string>

struct Token {
    int value;
};

class Model {
public:
    virtual std::vector<int> Preprocess(const std::string& text) = 0;
    virtual void Feed(const std::vector<Token>& next_tokens) = 0;
    // TODO: add default implementation
    virtual std::vector<std::vector<double>> GetFrequencies() = 0;
    virtual std::string Postprocess(const std::vector<Token>& tokens) = 0;
    virtual Token GetEndOfFileToken() = 0;
};