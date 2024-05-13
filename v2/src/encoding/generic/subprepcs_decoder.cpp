//
// Created by zeliboba on 3/11/24.
//

#include <sstream>
#include "subprepcs_decoder.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"
#include "encoding/log/secondary/naive_secondary_log_decoder.hpp"
#include "modelling_decoder.hpp"
#include "encoding/log/link/residue_link_decoder.hpp"
#include "encoding/log/secondary/residue_secondary_log_decoder.hpp"
#include "encoding/log/secondary/ppm_secondary_log_decoder.hpp"
#include "encoding/log/link/residue_link_encoder.hpp"
#include "identity_decoder.hpp"

SubPrePcsDecoder SubPrePcsDecoder::Create(std::shared_ptr<BitInputStream> primary,
                                          std::shared_ptr<BitInputStream> secondary,
                                          std::shared_ptr<BitInputStream> markup,
                                          PrimaryLogDecoderFactory primaryFactory,
                                          SecondaryLogDecoderFactory secondaryFactory,
                                          GenericDecoderFactory primaryGenericFactory,
                                          GenericDecoderFactory secondaryGenericFactory,
                                          GenericDecoderFactory markupGenericFactory,
                                          std::unique_ptr<LogLinkDecoder> linkDecoder,
                                          std::unique_ptr<LogStorage> storage) {
    if (linkDecoder == nullptr) {
        linkDecoder = std::make_unique<ResidueLinkDecoder>(255);
    }
    if (storage == nullptr) {
        storage = std::make_unique<GreedyLogStorage>(255);
    }

    auto primaryInMemory = std::make_shared<std::stringstream>();
    auto secondaryInMemory = std::make_shared<std::stringstream>();
    auto markupInMemory = std::make_shared<std::stringstream>();

    auto primaryInMemoryIn = std::make_shared<BitInputStream>(primaryInMemory);
    auto primaryInMemoryOut = std::make_shared<BitOutputStream>(primaryInMemory);

    auto secondaryInMemoryIn = std::make_shared<BitInputStream>(secondaryInMemory);
    auto secondaryInMemoryOut = std::make_shared<BitOutputStream>(secondaryInMemory);

    auto markupInMemoryIn = std::make_shared<BitInputStream>(markupInMemory);
    auto markupInMemoryOut = std::make_shared<BitOutputStream>(markupInMemory);

    auto secondaryDecoder = secondaryFactory(secondaryInMemoryIn);
    auto primaryDecoder = primaryFactory(std::move(linkDecoder), std::move(storage), std::move(secondaryDecoder), primaryInMemoryIn, markupInMemoryIn);

    auto primaryGenericDecoder = primaryGenericFactory(primary);
    auto secondaryGenericDecoder = secondaryGenericFactory(secondary);
    auto markupGenericDecoder = markupGenericFactory(markup);

    return SubPrePcsDecoder(std::move(primaryDecoder), std::move(secondaryDecoder), std::move(primaryInMemoryOut), std::move(secondaryInMemoryOut), std::move(markupInMemoryOut),
                            std::move(primaryGenericDecoder), std::move(secondaryGenericDecoder), std::move(markupGenericDecoder));
}


SubPrePcsDecoder SubPrePcsDecoder::CreateNaive(std::shared_ptr<BitInputStream> primary,
                                               std::shared_ptr<BitInputStream> secondary,
                                               std::shared_ptr<BitInputStream> markup,
                                               std::unique_ptr<LogLinkDecoder> linkDecoder,
                                               std::unique_ptr<LogStorage> storage) {
    return Create(
            primary,
            secondary,
            markup,
            [](std::unique_ptr<LogLinkDecoder> linkDecoder, std::unique_ptr<LogStorage> storage, std::unique_ptr<SecondaryLogDecoder> secondaryDecoder, std::shared_ptr<BitInputStream> primaryIn, std::shared_ptr<BitInputStream> markupIn) {
                return std::make_unique<PrimaryLogDecoder>(std::move(linkDecoder), std::move(storage), std::move(secondaryDecoder), primaryIn, markupIn);
            },
            [](std::shared_ptr<BitInputStream> in) { return std::make_unique<NaiveSecondaryLogDecoder>(in); },
            ModellingDecoder::CreateDefault,
            ModellingDecoder::CreateDefault,
            ModellingDecoder::CreateDefault,
            std::move(linkDecoder),
            std::move(storage)
    );
}

SubPrePcsDecoder SubPrePcsDecoder::CreateResidue(std::shared_ptr<BitInputStream> primary,
                                                 std::shared_ptr<BitInputStream> secondary,
                                                 std::shared_ptr<BitInputStream> markup,
                                                 std::unique_ptr<LogLinkDecoder> linkDecoder,
                                                 std::unique_ptr<LogStorage> storage) {
    return Create(
            primary,
            secondary,
            markup,
            [](std::unique_ptr<LogLinkDecoder> linkDecoder, std::unique_ptr<LogStorage> storage, std::unique_ptr<SecondaryLogDecoder> secondaryDecoder, std::shared_ptr<BitInputStream> primaryIn, std::shared_ptr<BitInputStream> markupIn) {
                return std::make_unique<PrimaryLogDecoder>(std::move(linkDecoder), std::move(storage), std::move(secondaryDecoder), primaryIn, markupIn);
            },
            [](std::shared_ptr<BitInputStream> in) { return std::make_unique<ResidueSecondaryLogDecoder>(in); },
            ModellingDecoder::CreateDefault,
            ModellingDecoder::CreateDefault,
            ModellingDecoder::CreateDefault,
            std::move(linkDecoder),
            std::move(storage)
    );
}

SubPrePcsDecoder SubPrePcsDecoder::CreatePPM(std::shared_ptr<BitInputStream> primary,
                                             std::shared_ptr<BitInputStream> secondary,
                                             std::shared_ptr<BitInputStream> markup,
                                             std::unique_ptr<LogLinkDecoder> linkDecoder,
                                             std::unique_ptr<LogStorage> storage) {
    return Create(
            primary,
            secondary,
            markup,
            [](std::unique_ptr<LogLinkDecoder> linkDecoder, std::unique_ptr<LogStorage> storage, std::unique_ptr<SecondaryLogDecoder> secondaryDecoder, std::shared_ptr<BitInputStream> primaryIn, std::shared_ptr<BitInputStream> markupIn) {
                return std::make_unique<PrimaryLogDecoder>(std::move(linkDecoder), std::move(storage), std::move(secondaryDecoder), primaryIn, markupIn);
            },
            [](std::shared_ptr<BitInputStream> in) { return std::make_unique<PPMSecondaryLogDecoder>(in); },
            ModellingDecoder::CreateDefault,
            [](std::shared_ptr<BitInputStream> in) { return std::make_unique<IdentityDecoder>(in); },
            ModellingDecoder::CreateDefault,
            std::move(linkDecoder),
            std::move(storage)
    );
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