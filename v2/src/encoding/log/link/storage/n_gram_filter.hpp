//
// Created by zeliboba on 5/23/24.
//

#ifndef DIPLOMA_N_GRAM_FILTER_HPP
#define DIPLOMA_N_GRAM_FILTER_HPP

#include <vector>
#include <map>
#include "log_filter.hpp"

class NgramFilter : public LogFilter {
public:
    NgramFilter(int n = 2, float acceptanceThreshold = 0.6f);
    ~NgramFilter() override = default;

    bool Filter(const std::vector<Token>& log) override;
    void Store(const std::vector<Token>& log) override;
    void Remove(const std::vector<Token>& log) override;

private:
    std::map<std::vector<Token>, std::map<std::vector<Token>, int>> ngrams;
    int n;
    float acceptanceThreshold;

    std::map<std::vector<Token>, int> getNgrams(const std::vector<Token>& log);
    static float jaccard(const std::map<std::vector<Token>, int>& ngrams1, const std::map<std::vector<Token>, int>& ngrams2);
};

#endif //DIPLOMA_N_GRAM_FILTER_HPP
