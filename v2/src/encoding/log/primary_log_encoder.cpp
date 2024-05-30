//
// Created by zeliboba on 2/28/24.
//

#include "primary_log_encoder.hpp"
#include "encoding/residue_coder.hpp"

PrimaryLogEncoder::PrimaryLogEncoder(std::unique_ptr<LogLinkEncoder>&& linkEncoder, std::unique_ptr<LogStorage>&& storage,
                                     std::unique_ptr<SecondaryLogEncoder>&& secondaryLogEncoder, std::shared_ptr<BitOutputStream> mainOutputStream,
                                     std::shared_ptr<BitOutputStream> markupOutputStream, int minLinkLength, bool useOptimalPartitioning) :
                                     linkEncoder(std::move(linkEncoder)), storage(std::move(storage)),
                                     secondaryLogEncoder(std::move(secondaryLogEncoder)), mainOutputStream(std::move(mainOutputStream)),
                                     markupOutputStream(std::move(markupOutputStream)), minLinkLength(minLinkLength), useOptimalPartitioning(useOptimalPartitioning) {}

void PrimaryLogEncoder::EncodeLine(const std::vector<Token>& line) {
    // record the length of the line
    // todo: line length often fits in one byte, so we can optimize it
    auto lineLength = int(line.size());
    ResidueCoder::EncodeInt(*markupOutputStream, lineLength, 255);
    if (useOptimalPartitioning) {
        EncodeOptimally(line);
    }
    else {
        EncodeGreedily(line);
    }
    storage->Store(line);
    secondaryLogEncoder->FinishLine();

    mainOutputStream->Flush();
    markupOutputStream->Flush();
}

void PrimaryLogEncoder::Finish() {
    // no more lines
    ResidueCoder::EncodeInt(*markupOutputStream, 0, 255);

    mainOutputStream->Close();
    secondaryLogEncoder->Finish();
    markupOutputStream->Close();
}

void PrimaryLogEncoder::EncodeOptimally(const std::vector<Token>& line) {
    // dp[i] = min number of steps (either links or single tokens) to encode first i tokens
    std::vector<int> dp(line.size() + 1, INT32_MAX);
    std::vector<std::optional<LogLink>> prev(line.size() + 1, std::nullopt);

    dp[0] = 0;
    for (auto i = 0; i < line.size(); i++) {
        auto maxContinuation = storage->TryLink(line, i, minLinkLength);
        if (maxContinuation.has_value()) {
            auto length = maxContinuation.value().Length;
            for (auto j = minLinkLength; j <= length; j++) {
                if (dp[i] + 1 < dp[i + j]) {
                    dp[i + j] = dp[i] + 1;
                    prev[i + j] = LogLink{maxContinuation.value().RecordIndex, maxContinuation.value().StartIndex, j};
                }
            }
        }
        if (dp[i] + 1 < dp[i + 1]) {
            dp[i + 1] = dp[i] + 1;
            prev[i + 1] = std::nullopt;
        }
    }

    std::vector<std::optional<LogLink>> bestLinksReversed;
    int cur = line.size();
    while (cur > 0) {
        if (prev[cur].has_value()) {
            bestLinksReversed.push_back(prev[cur]);
            cur -= prev[cur].value().Length;
        }
        else {
            bestLinksReversed.emplace_back(std::nullopt);
            cur--;
        }
    }

    std::vector<std::optional<LogLink>> bestLinks(bestLinksReversed.rbegin(), bestLinksReversed.rend());

    auto curIndex = 0;
    for (auto i = 0; i < bestLinks.size();i++) {
        auto link = bestLinks[i];
        if (link.has_value()) {
            // 0 means link
            markupOutputStream->Write(0);
            linkEncoder->EncodeLink(*mainOutputStream, link.value());
            secondaryLogEncoder->Feed(line, curIndex, link.value().Length);
            curIndex += link.value().Length;
        }
        else {
            // 1 means single token
            markupOutputStream->Write(1);
            secondaryLogEncoder->EncodeToken(line[curIndex]);
            curIndex++;
        }
    }
}

void PrimaryLogEncoder::EncodeGreedily(const std::vector<Token>& line) {
    for (auto i = 0; i < line.size();) {
        auto link = storage->TryLink(line, i, minLinkLength);
        if (link.has_value()) {
            // 0 means link
            markupOutputStream->Write(0);
            linkEncoder->EncodeLink(*mainOutputStream, link.value());
            secondaryLogEncoder->Feed(line, i, link.value().Length);
            i += link.value().Length;
        }
        else {
            // 1 means single token
            markupOutputStream->Write(1);
            secondaryLogEncoder->EncodeToken(line[i]);
            i++;
        }
    }
}