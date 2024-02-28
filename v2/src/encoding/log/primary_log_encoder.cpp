//
// Created by zeliboba on 2/28/24.
//

#include "primary_log_encoder.hpp"

PrimaryLogEncoder::PrimaryLogEncoder(LogLinkEncoder& linkEncoder, LogStorage& storage, SecondaryLogEncoder& secondaryLogEncoder, std::unique_ptr<BitOutputStream> mainOutputStream, std::unique_ptr<BitOutputStream> markupOutputStream, int minLinkLength)
    : linkEncoder(linkEncoder), storage(storage), secondaryLogEncoder(secondaryLogEncoder), mainOutputStream(std::move(mainOutputStream)), markupOutputStream(std::move(markupOutputStream)), minLinkLength(minLinkLength) {}

void PrimaryLogEncoder::EncodeLine(const std::vector<Token>& line) {
    // record the length of the line
    // todo: line length often fits in one byte, so we can optimize it
    auto lineLength = int(line.size());
    markupOutputStream->WriteByte(lineLength);
    markupOutputStream->WriteByte(lineLength >> 8);
    for (auto i = 0; i < line.size();) {
        auto link = storage.TryLink(line, i);
        if (link.has_value() && link.value().Length >= minLinkLength) {
            // 0 means link
            markupOutputStream->Write(0);
            linkEncoder.EncodeLink(*mainOutputStream, link.value());
            i += link.value().Length;
            secondaryLogEncoder.Feed(line, i, link.value().Length);
        }
        else {
            // 1 means single token
            markupOutputStream->Write(1);
            secondaryLogEncoder.EncodeToken(line[i]);
            i++;
        }
    }
    storage.Store(line);
    secondaryLogEncoder.FinishLine();
}