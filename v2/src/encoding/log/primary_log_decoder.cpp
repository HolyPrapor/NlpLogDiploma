//
// Created by zeliboba on 2/28/24.
//

#include "primary_log_decoder.hpp"

PrimaryLogDecoder::PrimaryLogDecoder(LogLinkDecoder& linkDecoder, LogStorage& storage, SecondaryLogDecoder& secondaryLogDecoder, std::unique_ptr<BitInputStream> mainInputStream, std::unique_ptr<BitInputStream> markupInputStream) : linkDecoder(linkDecoder), storage(storage), secondaryLogDecoder(secondaryLogDecoder), mainInputStream(std::move(mainInputStream)), markupInputStream(std::move(markupInputStream)) {}

std::vector<Token> PrimaryLogDecoder::DecodeLine() {
    // todo: line length often fits in one byte, so we can optimize it
    int lineLength = int(markupInputStream->ReadByte()) | int(markupInputStream->ReadByte()) << 8;
    std::vector<Token> line;
    line.reserve(lineLength);
    for (auto i = 0; i < lineLength;) {
        auto isToken = markupInputStream->ReadWithoutEOF();
        if (isToken) {
            line.push_back(secondaryLogDecoder.DecodeToken());
            i++;
        }
        else {
            auto link = linkDecoder.DecodeLink(*mainInputStream);
            auto storedLine = storage.GetLog(link.RecordIndex);
            for (auto j = 0; j < link.Length; j++) {
                line.push_back(storedLine[link.StartIndex + j]);
            }
            i += link.Length;
            secondaryLogDecoder.Feed(line, i, link.Length);
        }
    }
    storage.Store(line);
    secondaryLogDecoder.FinishLine();
    return line;
}