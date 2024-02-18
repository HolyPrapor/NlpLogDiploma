//
// Created by zeliboba on 2/18/24.
//

#include "ppm_model.hpp"

class Node {
public:
    explicit Node(int length) : length(length), seen(0), different(0) {
        children_counts = std::vector<int>(length, 0);
        children = std::vector<Node*>(length, nullptr);
    }

    ~Node() {
        for (auto& child : children) {
            delete child;
        }
    }

    void Update(std::vector<Token>& buffer, int offset, int size) {
        if (size == 0) {
            seen++;
            if (children_counts[buffer[offset]] == 0) {
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

struct ContextLookupResult {
    bool character_exists_in_context;
    std::unique_ptr<std::vector<int>> frequencies;
};

class Trie {
public:
    explicit Trie(int length) : length(length) {
        root = new Node(length);
    }

    ~Trie() {
        delete root;
    }

    // NOTE: returns a vector of length + 1, where the last element is the escape token
    ContextLookupResult TryPath(const std::vector<Token>& buffer, int offset, int size, bool encode = true) {
        Node* node = root;
        for (int i = 0; i < size; ++i) {
            if (node->children[buffer[offset + i]] == nullptr) {
                return {false, nullptr};
            }
            node = node->children[buffer[offset + i]];
        }
        if (encode) {
            // we have the character in the context
            if (node->children_counts[buffer[offset + size]] > 0) {
                return {true, PreprocessCounts(node)};
            } // we have seen the context, but not the character
            else if (node->seen > 0) {
                return {false, PreprocessCounts(node)};
            } // we have not seen the context
            return {false, nullptr};
        }
        if (node->seen == 0) {
            return {false, nullptr};
        }
        return {true, PreprocessCounts(node)};
    }

    void Update(std::vector<Token>& buffer, int offset, int size) {
        for (int context_size = 0; context_size < size; ++context_size) {
            root->Update(buffer, offset + size - 1 - context_size, context_size);
        }
    }

    std::unique_ptr<std::vector<int>> PreprocessCounts(Node* parent) const {
        std::unique_ptr<std::vector<int>> result = std::make_unique<std::vector<int>>(length + 1, 2);
        auto prev = 0;
        for (int i = 0; i < length; ++i) {
            result->at(i) += parent->children_counts[i] + prev;
            prev = result->at(i);
        }
        int total = result->back();
        double q = GetEscapeProbability(parent);
        int y = /*(q < 1) ?*/ static_cast<int>(std::ceil(q * total / (1 - q))) /*: (2 >> 30) - total*/;
        result->back() = total + y;
        return result;
    }

    static double GetEscapeProbability(Node* parent) {
        return static_cast<double>(parent->different) / (parent->seen + parent->different);
    }

    int length;
    Node* root;
};

struct PPMBaseModel::Impl {
    explicit Impl(int context_size, int alphabet_size) : trie(Trie(alphabet_size)), uniform_frequencies(alphabet_size, 2) {
        context.reserve(context_size);
        std::partial_sum(uniform_frequencies.begin(), uniform_frequencies.end(), uniform_frequencies.begin());
    }

    Trie trie;
    std::vector<Token> context;
    std::vector<int> uniform_frequencies;
};

PPMBaseModel::PPMBaseModel(int context_size, int alphabet_size)
        : context_size(context_size), alphabet_size(alphabet_size), impl(std::make_unique<Impl>(context_size, alphabet_size)) {
}

Token PPMBaseModel::GetEndOfFileToken() {
    return Token(alphabet_size - 1);
}

std::vector<int> PPMBaseModel::GetUniformFrequencies() {
    return impl->uniform_frequencies;
}

PPMEncoderModel::PPMEncoderModel(ArithmeticEncoder& coder, int context_size, int alphabet_size)
        : PPMBaseModel(context_size, alphabet_size), coder(coder) {
}

// todo: avoid erasing from the beginning of the vector
void PPMEncoderModel::Feed(const Token& next_token) {
    impl->context.push_back(next_token);
    if (static_cast<int>(impl->context.size()) > context_size + 1) {
        impl->context.erase(impl->context.begin());
    }
}

void PPMEncoderModel::UpdateTrie() {
    impl->trie.Update(impl->context, 0, static_cast<int>(impl->context.size()));
}

Distribution PPMEncoderModel::GetCurrentDistribution() {
    for (int size = std::min(context_size, static_cast<int>(impl->context.size()) - 1); size >= 0; --size) {
        auto result = impl->trie.TryPath(impl->context, static_cast<int>(impl->context.size()) - size - 1, size);
        if (result.character_exists_in_context) {
            UpdateTrie();
            auto distribution = Distribution(std::move(result.frequencies));
            return distribution;
        }
        if (result.frequencies != nullptr) {
            coder.Write(*result.frequencies, GetEscapeToken());
        }
    }
    UpdateTrie();
    return Distribution(std::make_unique<std::vector<int>>(GetUniformFrequencies()));
}

PPMModelDecoder::PPMModelDecoder(ArithmeticDecoder& decoder, int context_size, int alphabet_size)
        : PPMBaseModel(context_size, alphabet_size), decoder(decoder) {
}

void PPMModelDecoder::Feed(const Token& next_token) {
    auto escaped_next_token = next_token;
    while (current_context_size >= 0 && escaped_next_token == GetEscapeToken()) {
        --current_context_size;
        auto result = GetCurrentDistribution();
        escaped_next_token = decoder.Read(result.Frequencies());
    }
    impl->context.push_back(escaped_next_token);
    if (static_cast<int>(impl->context.size()) > context_size) {
        impl->context.erase(impl->context.begin());
    }
    impl->trie.Update(impl->context, 0, static_cast<int>(impl->context.size()));
    current_context_size = std::min(context_size, static_cast<int>(impl->context.size()));
}

Distribution PPMModelDecoder::GetCurrentDistribution() {
    while (current_context_size >= 0) {
        auto result = impl->trie.TryPath(impl->context, static_cast<int>(impl->context.size()) - current_context_size,
                                        current_context_size, false);
        if (result.character_exists_in_context) {
            return Distribution(std::move(result.frequencies));
        }
        --current_context_size;
    }
    return Distribution(std::make_unique<std::vector<int>>(GetUniformFrequencies()));
}
