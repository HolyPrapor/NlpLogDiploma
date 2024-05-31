//
// Created by zeliboba on 5/23/24.
//

#ifndef DIPLOMA_N_GRAM_FILTER_HPP
#define DIPLOMA_N_GRAM_FILTER_HPP

#include <vector>
#include <map>
#include "log_filter.hpp"

/*
 * These are calculated by sampling 1000 pairs of log lines.
 * +-------------+-------+-------+-------+-------+-------+-------+
 * | Metric      |  n    |  p10  |  p25  |  p35  |  p50  |  p75  |
 * +-------------+-------+-------+-------+-------+-------+-------+
 * | Jaccard     |   1   | 0.511 | 0.563 | 0.587 | 0.615 | 0.676 |
 * | Jaccard     |   2   | 0.122 | 0.144 | 0.155 | 0.171 | 0.205 |
 * | Jaccard     |   3   | 0.046 | 0.061 | 0.069 | 0.082 | 0.124 |
 * | Cosine      |   1   | 0.668 | 0.741 | 0.763 | 0.796 | 0.843 |
 * | Cosine      |   2   | 0.193 | 0.240 | 0.269 | 0.306 | 0.371 |
 * | Cosine      |   3   | 0.068 | 0.098 | 0.115 | 0.142 | 0.215 |
 * +-------------+-------+-------+-------+-------+-------+-------+
 */

class NgramFilter : public LogFilter {
public:
    NgramFilter(int n = 2, float acceptanceThreshold = 0.6f, bool useCosineSimilarity = false);
    ~NgramFilter() override = default;

    bool Filter(const std::vector<Token>& log) override;
    void Store(const std::vector<Token>& log) override;
    void Remove(const std::vector<Token>& log) override;

private:
    std::map<std::vector<Token>, std::map<std::vector<Token>, int>> ngrams;
    int n;
    float acceptanceThreshold;
    bool useCosineSimilarity;

    std::map<std::vector<Token>, int> getNgrams(const std::vector<Token>& log);
    static float jaccard(const std::map<std::vector<Token>, int>& ngrams1, const std::map<std::vector<Token>, int>& ngrams2);
    static float cosine(const std::map<std::vector<Token>, int>& ngrams1, const std::map<std::vector<Token>, int>& ngrams2);
};

#endif //DIPLOMA_N_GRAM_FILTER_HPP
