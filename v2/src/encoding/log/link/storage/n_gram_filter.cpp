//
// Created by zeliboba on 5/23/24.
//

#include <complex>
#include "n_gram_filter.hpp"

NgramFilter::NgramFilter(int n, float acceptanceThreshold, bool useCosineSimilarity) : n(n), acceptanceThreshold(acceptanceThreshold), useCosineSimilarity(useCosineSimilarity) {}

bool NgramFilter::Filter(const std::vector<Token>& log) {
    // If the log is too short, we don't want to have it
    // This is a reasonable constant rather than a parameter
    if (log.size() < 50) {
        return true;
    }
    auto candidateNgrams = getNgrams(log);
    auto max = 0.0f;
    for (auto &existingNgrams : ngrams) {
        float index = 0;
        if (useCosineSimilarity) {
            index = cosine(existingNgrams.second, candidateNgrams);
        } else {
            index = jaccard(existingNgrams.second, candidateNgrams);
        }
        if (index > max) {
            max = index;
        }
    }
    return max > acceptanceThreshold;
}

void NgramFilter::Store(const std::vector<Token>& log) {
    ngrams[log] = getNgrams(log);
}

void NgramFilter::Remove(const std::vector<Token>& log) {
    ngrams.erase(log);
}

std::map<std::vector<Token>, int> NgramFilter::getNgrams(const std::vector<Token>& log) {
    std::map<std::vector<Token>, int> ngrams;
    for (int i = 0; i <= log.size() - n; i++) {
        std::vector<Token> ngram;
        for (int j = 0; j < n; j++) {
            ngram.push_back(log[i + j]);
        }
        ngrams[ngram]++;
    }
    return ngrams;
}

float NgramFilter::jaccard(const std::map<std::vector<Token>, int>& ngrams1, const std::map<std::vector<Token>, int>& ngrams2) {
    int intersection = 0;
    int unionSize = 0;

    for (const auto& ngram : ngrams1) {
        if (ngrams2.find(ngram.first) != ngrams2.end()) {
            intersection += std::min(ngram.second, ngrams2.at(ngram.first));
        }
        unionSize += ngram.second;
    }

    for (const auto& ngram : ngrams2) {
        if (ngrams1.find(ngram.first) == ngrams1.end()) {
            unionSize += ngram.second;
        }
    }
    return static_cast<float>(intersection) / unionSize;
}

float NgramFilter::cosine(const std::map<std::vector<Token>, int>& ngrams1, const std::map<std::vector<Token>, int>& ngrams2) {
    int dotProduct = 0;
    int sum1 = 0;
    int sum2 = 0;

    for (const auto& ngram : ngrams1) {
        if (ngrams2.find(ngram.first) != ngrams2.end()) {
            dotProduct += ngram.second * ngrams2.at(ngram.first);
        }
        sum1 += ngram.second * ngram.second;
    }

    for (const auto& ngram : ngrams2) {
        sum2 += ngram.second * ngram.second;
    }

    if (sum1 == 0 || sum2 == 0) {
        return 0.0f;
    }

    return static_cast<float>(dotProduct) / (std::sqrt(sum1) * std::sqrt(sum2));
}