//
// Created by zeliboba on 5/23/24.
//

#include "n_gram_filter.hpp"

NgramFilter::NgramFilter(int n, float acceptanceThreshold) : n(n), acceptanceThreshold(acceptanceThreshold) {}

bool NgramFilter::Filter(const std::vector<Token>& log) {
    // If the log is too short, we don't want to have it
    // This is a reasonable constant rather than a parameter
    if (log.size() < 50) {
        return true;
    }
    auto candidateNgrams = getNgrams(log);
    auto max = 0.0f;
    for (auto &existingNgrams : ngrams) {
        auto jaccardIndex = jaccard(existingNgrams.second, candidateNgrams);
        if (jaccardIndex > max) {
            max = jaccardIndex;
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
    for (int i = 0; i < log.size() - n; i++) {
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
    for (auto &ngram : ngrams1) {
        if (ngrams2.find(ngram.first) != ngrams2.end()) {
            intersection += std::min(ngram.second, ngrams2.at(ngram.first));
        }
    }
    for (auto &ngram : ngrams1) {
        unionSize += ngram.second;
    }
    for (auto &ngram : ngrams2) {
        unionSize += ngram.second;
    }
    return static_cast<float>(intersection) / unionSize;
}