//
// Created by zeliboba on 3/1/26.
//

#include <vector>
#include <sstream>
#include "subprepcs_encoder.hpp"
#include "encoding/log/link/residue_link_encoder.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"
#include "encoding/log/secondary/naive_secondary_log_encoder.hpp"
#include "modelling_encoder.hpp"
#include "encoding/log/secondary/residue_secondary_log_encoder.hpp"
#include "encoding/log/secondary/ppm_secondary_log_encoder.hpp"
#include "identity_encoder.hpp"

SubPrePcsEncoder SubPrePcsEncoder::Create(std::shared_ptr<BitOutputStream> primary,
                                          std::shared_ptr<BitOutputStream> secondary,
                                          std::shared_ptr<BitOutputStream> markup,
                                          PrimaryLogEncoderFactory primaryFactory,
                                          SecondaryLogEncoderFactory secondaryFactory,
                                          GenericEncoderFactory primaryGenericFactory,
                                          GenericEncoderFactory secondaryGenericFactory,
                                          GenericEncoderFactory markupGenericFactory,
                                          std::unique_ptr<LogLinkEncoder> linkEncoder,
                                          std::unique_ptr<LogStorage> storage) {
    if (linkEncoder == nullptr) {
        linkEncoder = std::make_unique<ResidueLinkEncoder>(255);
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

    auto secondaryEncoder = secondaryFactory(secondaryInMemoryOut);
    auto primaryEncoder = primaryFactory(std::move(linkEncoder), std::move(storage), std::move(secondaryEncoder), primaryInMemoryOut, markupInMemoryOut);

    auto primaryGenericEncoder = primaryGenericFactory(primary);
    auto secondaryGenericEncoder = secondaryGenericFactory(secondary);
    auto markupGenericEncoder = markupGenericFactory(markup);

    return SubPrePcsEncoder(
            std::move(primaryEncoder),
            std::move(primaryInMemoryIn),
            std::move(secondaryInMemoryIn),
            std::move(markupInMemoryIn),
            std::move(primaryGenericEncoder),
            std::move(secondaryGenericEncoder),
            std::move(markupGenericEncoder)
    );
}

SubPrePcsEncoder SubPrePcsEncoder::CreateNaive(
        std::shared_ptr<BitOutputStream> primary,
        std::shared_ptr<BitOutputStream> secondary,
        std::shared_ptr<BitOutputStream> markup,
        std::unique_ptr<LogLinkEncoder> linkEncoder,
        std::unique_ptr<LogStorage> storage)
{
    return Create(
            primary,
            secondary,
            markup,
            [](std::unique_ptr<LogLinkEncoder> linkEncoder, std::unique_ptr<LogStorage> storage, std::unique_ptr<SecondaryLogEncoder> secondaryEncoder, std::shared_ptr<BitOutputStream> primaryOut, std::shared_ptr<BitOutputStream> markupOut) {
                return std::make_unique<PrimaryLogEncoder>(std::move(linkEncoder), std::move(storage), std::move(secondaryEncoder), primaryOut, markupOut);
            },
            [](std::shared_ptr<BitOutputStream> out) { return std::make_unique<NaiveSecondaryLogEncoder>(out); },
            [](std::shared_ptr<BitOutputStream> out) { return ModellingEncoder::CreateDefault(out); },
            [](std::shared_ptr<BitOutputStream> out) { return ModellingEncoder::CreateDefault(out); },
            [](std::shared_ptr<BitOutputStream> out) { return ModellingEncoder::CreateDefault(out); },
            std::move(linkEncoder),
            std::move(storage)
    );
}

SubPrePcsEncoder SubPrePcsEncoder::CreateResidue(
        std::shared_ptr<BitOutputStream> primary,
        std::shared_ptr<BitOutputStream> secondary,
        std::shared_ptr<BitOutputStream> markup,
        std::unique_ptr<LogLinkEncoder> linkEncoder,
        std::unique_ptr<LogStorage> storage)
{
    return Create(
            primary,
            secondary,
            markup,
            [](std::unique_ptr<LogLinkEncoder> linkEncoder, std::unique_ptr<LogStorage> storage, std::unique_ptr<SecondaryLogEncoder> secondaryEncoder, std::shared_ptr<BitOutputStream> primaryOut, std::shared_ptr<BitOutputStream> markupOut) {
                return std::make_unique<PrimaryLogEncoder>(std::move(linkEncoder), std::move(storage), std::move(secondaryEncoder), primaryOut, markupOut);
            },
            [](std::shared_ptr<BitOutputStream> out) { return std::make_unique<ResidueSecondaryLogEncoder>(out); },
            [](std::shared_ptr<BitOutputStream> out) { return ModellingEncoder::CreateDefault(out); },
            [](std::shared_ptr<BitOutputStream> out) { return ModellingEncoder::CreateDefault(out); },
            [](std::shared_ptr<BitOutputStream> out) { return ModellingEncoder::CreateDefault(out); },
            std::move(linkEncoder),
            std::move(storage)
    );
}

SubPrePcsEncoder SubPrePcsEncoder::CreatePPM(
        std::shared_ptr<BitOutputStream> primary,
        std::shared_ptr<BitOutputStream> secondary,
        std::shared_ptr<BitOutputStream> markup,
        std::unique_ptr<LogLinkEncoder> linkEncoder,
        std::unique_ptr<LogStorage> storage)
{
    return Create(
            primary,
            secondary,
            markup,
            [](std::unique_ptr<LogLinkEncoder> linkEncoder, std::unique_ptr<LogStorage> storage, std::unique_ptr<SecondaryLogEncoder> secondaryEncoder, std::shared_ptr<BitOutputStream> primaryOut, std::shared_ptr<BitOutputStream> markupOut) {
                return std::make_unique<PrimaryLogEncoder>(std::move(linkEncoder), std::move(storage), std::move(secondaryEncoder), primaryOut, markupOut);
            },
            [](std::shared_ptr<BitOutputStream> out) { return std::make_unique<PPMSecondaryLogEncoder>(out); },
            [](std::shared_ptr<BitOutputStream> out) { return ModellingEncoder::CreateDefault(out); },
            [](std::shared_ptr<BitOutputStream> out) { return std::make_unique<IdentityEncoder>(out); },
            [](std::shared_ptr<BitOutputStream> out) { return ModellingEncoder::CreateDefault(out); },
            std::move(linkEncoder),
            std::move(storage)
    );
}

SubPrePcsEncoder::SubPrePcsEncoder(std::unique_ptr<PrimaryLogEncoder>&& primaryEncoder,
                                   std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> secondary, std::shared_ptr<BitInputStream> markup,
                                   std::unique_ptr<GenericEncoder>&& primaryGenericEncoder, std::unique_ptr<GenericEncoder>&& secondaryGenericEncoder, std::unique_ptr<GenericEncoder>&& markupGenericEncoder) {
    this->primaryEncoder = std::move(primaryEncoder);
    this->primary = std::move(primary);
    this->secondary = std::move(secondary);
    this->markup = std::move(markup);
    this->primaryGenericEncoder = std::move(primaryGenericEncoder);
    this->secondaryGenericEncoder = std::move(secondaryGenericEncoder);
    this->markupGenericEncoder = std::move(markupGenericEncoder);
}

static std::vector<Token> readLine(BitInputStream& inputStream) {
    std::vector<Token> line;
    auto byte = inputStream.ReadByte();
    while (byte != '\n') {
        line.push_back(byte);
        if (inputStream.IsEOF()) {
            break;
        }
        byte = inputStream.ReadByte();
    }
    return line;
}

void SubPrePcsEncoder::Encode(BitInputStream& input) {
    while (!input.IsEOF()) {
        auto line = readLine(input);
        primaryEncoder->EncodeLine(line);
        primaryGenericEncoder->Encode(*primary);
        secondaryGenericEncoder->Encode(*secondary);
        markupGenericEncoder->Encode(*markup);
    }
}

void SubPrePcsEncoder::Finish() {
    primaryEncoder->Finish();
    primaryGenericEncoder->Encode(*primary);
    secondaryGenericEncoder->Encode(*secondary);
    markupGenericEncoder->Encode(*markup);

    primaryGenericEncoder->Finish();
    secondaryGenericEncoder->Finish();
    markupGenericEncoder->Finish();
}