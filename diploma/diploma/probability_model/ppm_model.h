//
// Created by zeliboba on 5/29/23.
//

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <string>
#include <stdexcept>
#include "model.h"
#include "../arithmetic_coding/arithmetic_encoder.h"
#include "../arithmetic_coding/arithmetic_decoder.h"

class Node {
public:
    static const int minimum_count = 0;

    explicit Node(int length) : length(length), seen(0), different(0) {
        children_counts = std::vector<int>(length + 1, minimum_count);
        children = std::vector<Node*>(length, nullptr);
    }

    ~Node() {
        for (auto& child : children) {
            delete child;
        }
    }

    void Update(std::vector<int>& buffer, int offset, int size) {
        if (size == 0) {
            seen++;
            if (children_counts[buffer[offset]] == minimum_count) {
                different++;
            }
            children_counts[buffer[offset]]++;
        } else {
            if (children[buffer[offset]] == nullptr) {
                children[buffer[offset]] = new Node(length);
            }
            children[buffer[offset]]->Update(buffer, offset + 1, size - 1);
        }
    }

    int length, seen, different;
    std::vector<int> children_counts;
    std::vector<Node*> children;
};

class Trie {
public:
    explicit Trie(int length) : length(length) {
        root = new Node(length);
    }

    ~Trie() {
        delete root;
    }

    std::pair<bool, std::vector<int>> TryPath(std::vector<int>& buffer, int offset, int size, bool encode = true) {
        Node* node = root;
        for (int i = 0; i < size; ++i) {
            if (node->children[buffer[offset + i]] == nullptr) {
                return std::make_pair(false, std::vector<int>());
            }
            node = node->children[buffer[offset + i]];
        }
        if (encode) {
            if (node->children_counts[buffer[offset + size]] > Node::minimum_count) {
                return std::make_pair(true, PreprocessCounts(node));
            } else if (node->seen > 0) {
                return std::make_pair(false, PreprocessCounts(node));
            }
            return std::make_pair(false, std::vector<int>());
        }
        if (node->seen == 0) {
            return std::make_pair(false, std::vector<int>());
        }
        return std::make_pair(true, PreprocessCounts(node));
    }

    void Update(std::vector<int>& buffer, int offset, int size) {
        for (int context_size = 0; context_size < size; ++context_size) {
            root->Update(buffer, offset + size - 1 - context_size, context_size);
        }
    }

    static std::vector<int> PreprocessCounts(Node* parent) {
        std::vector<int> cum(parent->children_counts);
        std::partial_sum(cum.begin(), cum.end(), cum.begin());
        int total = cum.back();
        double q = GetEscapeProbability(parent);
        int y = (q < 1) ? static_cast<int>(std::ceil(q * total / (1 - q))) : (2 >> 30) - total;
        cum.back() = total + y;
        return cum;
    }

    static double GetEscapeProbability(Node* parent) {
        return static_cast<double>(parent->different) / (parent->seen + parent->different);
    }

    int length;
    Node* root;
};

class PPMBaseModel : public Model {
public:
    int context_size, alphabet_size;

    explicit PPMBaseModel(int context_size, int alphabet_size = 256)
            : context_size(context_size), alphabet_size(alphabet_size + 1), trie(Trie(alphabet_size)), uniform_frequencies(alphabet_size, 2) {
        std::partial_sum(uniform_frequencies.begin(), uniform_frequencies.end(), uniform_frequencies.begin());
    }

    std::vector<Token> Preprocess(const std::string& text) override {
        std::vector<Token> result;
        for (char c : text) {
            result.emplace_back(static_cast<int>(c));
        }
        return result;
    }

    std::string Postprocess(const std::vector<Token>& tokens) override {
        std::string result;
        for (const Token& token : tokens) {
            result.push_back(static_cast<char>(token.value));
        }
        return result;
    }

    Token GetEndOfFileToken() override {
        return Token(alphabet_size - 1);
    }

    std::vector<int> GetUniformFrequencies() {
        return uniform_frequencies;
    }

protected:
    Trie trie;
    std::vector<int> context;
    std::vector<int> uniform_frequencies;
};

class PPMEncoderModel : public PPMBaseModel {
public:
    PPMEncoderModel(ArithmeticEncoder& coder, int context_size, int alphabet_size = 256)
            : PPMBaseModel(context_size, alphabet_size), coder(coder) {}

    void Feed(const std::vector<Token>& next_tokens) override {
        if (next_tokens.size() != 1) {
            throw std::runtime_error("Multiple tokens are not supported");
        }
        context.push_back(next_tokens[0].value);
        if (static_cast<int>(context.size()) > context_size + 1) {
            context.erase(context.begin());
        }
    }

    void UpdateTrie() {
        trie.Update(context, 0, static_cast<int>(context.size()));
    }

    std::vector<int> GetFrequencies() override {
        for (int size = std::min(context_size, static_cast<int>(context.size()) - 1); size >= 0; --size) {
            auto [ok, freqs] = trie.TryPath(context, static_cast<int>(context.size()) - size - 1, size);
            if (ok) {
                UpdateTrie();
                return freqs;
            }
            if (!freqs.empty()) {
                coder.Write(freqs, alphabet_size);
            }
        }
        UpdateTrie();
        return GetUniformFrequencies();
    }

private:
    ArithmeticEncoder& coder;
};

class PPMModelDecoder : public PPMBaseModel {
public:
    PPMModelDecoder(ArithmeticDecoder& decoder, int context_size, std::vector<Token>& tokens, int alphabet_size = 256)
            : PPMBaseModel(context_size, alphabet_size), decoder(decoder), current_context(0), tokens(tokens) {}

    void Feed(const std::vector<Token>& next_tokens) override {
        if (next_tokens.size() != 1) {
            throw std::runtime_error("Multiple tokens are not supported");
        }
        int token = next_tokens[0].value;
        while (current_context >= 0 && token == alphabet_size) {
            --current_context;
            auto freqs = GetFrequencies();
            token = decoder.Read(freqs);
        }
        tokens.emplace_back(token);
        if (token == alphabet_size) {
            throw std::runtime_error("This is surely a bug in code");
        }
        context.push_back(token);
        if (static_cast<int>(context.size()) > context_size + 1) {
            context.erase(context.begin());
        }
        trie.Update(context, 0, static_cast<int>(context.size()));
        current_context = std::min(context_size, static_cast<int>(context.size()));
    }

    std::vector<int> GetFrequencies() override {
        while (current_context >= 0) {
            auto [ok, freqs] = trie.TryPath(context, static_cast<int>(context.size()) - current_context,
                                            current_context, false);
            if (ok) {
                return freqs;
            }
            --current_context;
        }
        return GetUniformFrequencies();
    }

private:
    ArithmeticDecoder& decoder;
    int current_context;
    std::vector<Token>& tokens;
};
