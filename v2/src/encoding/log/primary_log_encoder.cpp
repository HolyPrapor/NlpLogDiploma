//
// Created by zeliboba on 2/28/24.
//

#include "primary_log_encoder.hpp"
#include "encoding/residue_coder.hpp"

PrimaryLogEncoder::PrimaryLogEncoder(std::unique_ptr<LogLinkEncoder>&& linkEncoder, std::unique_ptr<LogStorage>&& storage,
                                     std::unique_ptr<SecondaryLogEncoder>&& secondaryLogEncoder, std::shared_ptr<BitOutputStream> mainOutputStream,
                                     std::shared_ptr<BitOutputStream> markupOutputStream, int minLinkLength) :
                                     linkEncoder(std::move(linkEncoder)), storage(std::move(storage)),
                                     secondaryLogEncoder(std::move(secondaryLogEncoder)), mainOutputStream(std::move(mainOutputStream)),
                                     markupOutputStream(std::move(markupOutputStream)), minLinkLength(minLinkLength) {}

void PrimaryLogEncoder::EncodeLine(const std::vector<Token>& line) {
    // record the length of the line
    // todo: line length often fits in one byte, so we can optimize it
    auto lineLength = int(line.size());
    ResidueCoder::EncodeInt(*markupOutputStream, lineLength, 255);
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