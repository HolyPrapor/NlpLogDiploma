//
// Created by zeliboba on 3/11/24.
//

#include <sstream>
#include "subprepcs_decoder.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"
#include "encoding/log/secondary/naive_secondary_log_decoder.hpp"
#include "modelling_decoder.hpp"
#include "encoding/log/link/residue_link_decoder.hpp"

SubPrePcsDecoder SubPrePcsDecoder::CreateDefault(std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup) {
    auto linkEncoder = std::make_unique<ResidueLinkDecoder>(255);
    auto storage = std::make_unique<GreedyLogStorage>(50);

    auto primaryInMemory = std::make_shared<std::stringstream>();
    auto secondaryInMemory = std::make_shared<std::stringstream>();
    auto markupInMemory = std::make_shared<std::stringstream>();

    auto primaryInMemoryIn = std::make_shared<BitInputStream>(primaryInMemory);
    auto primaryInMemoryOut = std::make_shared<BitOutputStream>(primaryInMemory);

    auto secondaryInMemoryIn = std::make_shared<BitInputStream>(secondaryInMemory);
    auto secondaryInMemoryOut = std::make_shared<BitOutputStream>(secondaryInMemory);

    auto markupInMemoryIn = std::make_shared<BitInputStream>(markupInMemory);
    auto markupInMemoryOut = std::make_shared<BitOutputStream>(markupInMemory);

    auto secondaryDecoder = std::make_unique<NaiveSecondaryLogDecoder>(secondaryInMemoryIn);

    auto primaryDecoder = std::make_unique<PrimaryLogDecoder>(std::move(linkEncoder), std::move(storage), std::move(secondaryDecoder), primaryInMemoryIn, markupInMemoryIn);

    auto primaryGenericDecoder = std::make_unique<ModellingDecoder>(ModellingDecoder::CreateDefault(std::move(primary)));
    auto secondaryGenericDecoder = std::make_unique<ModellingDecoder>(ModellingDecoder::CreateDefault(std::move(secondary)));
    auto markupGenericDecoder = std::make_unique<ModellingDecoder>(ModellingDecoder::CreateDefault(std::move(markup)));

    return SubPrePcsDecoder(std::move(primaryDecoder), std::move(secondaryDecoder), std::move(primaryInMemoryOut), std::move(secondaryInMemoryOut), std::move(markupInMemoryOut),
                            std::move(primaryGenericDecoder), std::move(secondaryGenericDecoder), std::move(markupGenericDecoder));
}

SubPrePcsDecoder::SubPrePcsDecoder(std::unique_ptr<PrimaryLogDecoder>&& primaryDecoder, std::unique_ptr<SecondaryLogDecoder>&& secondaryDecoder,
                                   std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup,
                                   std::unique_ptr<GenericDecoder>&& primaryGenericDecoder, std::unique_ptr<GenericDecoder>&& secondaryGenericDecoder, std::unique_ptr<GenericDecoder>&& markupGenericDecoder)
    : primaryDecoder(std::move(primaryDecoder)), secondaryDecoder(std::move(secondaryDecoder)),
      primaryGenericDecoder(std::move(primaryGenericDecoder)), secondaryGenericDecoder(std::move(secondaryGenericDecoder)), markupGenericDecoder(std::move(markupGenericDecoder)),
      primary(std::move(primary)), secondary(std::move(secondary)), markup(std::move(markup)) {}

static void writeLine(BitOutputStream& outputStream, const std::vector<Token>& line) {
    for (Token token : line) {
        outputStream.WriteByte(token);
    }
    outputStream.WriteByte('\n');
}

void SubPrePcsDecoder::Decode(BitOutputStream& data) {
    // todo: we don't know how much to read so that we can decode the line, so for now we just decode everything
    // in memory. This is very inefficient, we must make lazy BitInputStream that reads when needed.
    // We should probably do the same for BitOutputStream.
    primaryGenericDecoder->Decode(*primary);
    secondaryGenericDecoder->Decode(*secondary);
    markupGenericDecoder->Decode(*markup);
    primaryGenericDecoder->Finish();
    secondaryGenericDecoder->Finish();
    markupGenericDecoder->Finish();

    while (true) {
        auto tokens = primaryDecoder->DecodeLine();
        if (tokens.empty()) {
            break;
        }
        writeLine(data, tokens);
    }

    data.Flush();
}

void SubPrePcsDecoder::Finish() {
}