//
// Created by zeliboba on 3/1/24.
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

SubPrePcsEncoder SubPrePcsEncoder::CreateNaive(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup) {
    auto linkEncoder = std::make_unique<ResidueLinkEncoder>(255);
    auto storage = std::make_unique<GreedyLogStorage>(255);

    auto primaryInMemory = std::make_shared<std::stringstream>();
    auto secondaryInMemory = std::make_shared<std::stringstream>();
    auto markupInMemory = std::make_shared<std::stringstream>();

    auto primaryInMemoryIn = std::make_shared<BitInputStream>(primaryInMemory);
    auto primaryInMemoryOut = std::make_shared<BitOutputStream>(primaryInMemory);

    auto secondaryInMemoryIn = std::make_shared<BitInputStream>(secondaryInMemory);
    auto secondaryInMemoryOut = std::make_shared<BitOutputStream>(secondaryInMemory);

    auto markupInMemoryIn = std::make_shared<BitInputStream>(markupInMemory);
    auto markupInMemoryOut = std::make_shared<BitOutputStream>(markupInMemory);

    auto secondaryEncoder = std::make_unique<NaiveSecondaryLogEncoder>(secondaryInMemoryOut);
    auto primaryEncoder = std::make_unique<PrimaryLogEncoder>(std::move(linkEncoder), std::move(storage), std::move(secondaryEncoder), primaryInMemoryOut, markupInMemoryOut, 4);

    auto primaryGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(primary)));
    auto secondaryGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(secondary)));
    auto markupGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(markup)));

    return SubPrePcsEncoder(std::move(primaryEncoder), std::move(primaryInMemoryIn), std::move(secondaryInMemoryIn), std::move(markupInMemoryIn),
                            std::move(primaryGenericEncoder), std::move(secondaryGenericEncoder), std::move(markupGenericEncoder));
}

SubPrePcsEncoder SubPrePcsEncoder::CreateResidue(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary, std::shared_ptr<BitOutputStream> markup) {
    auto linkEncoder = std::make_unique<ResidueLinkEncoder>(255);
    auto storage = std::make_unique<GreedyLogStorage>(255);

    auto primaryInMemory = std::make_shared<std::stringstream>();
    auto secondaryInMemory = std::make_shared<std::stringstream>();
    auto markupInMemory = std::make_shared<std::stringstream>();

    auto primaryInMemoryIn = std::make_shared<BitInputStream>(primaryInMemory);
    auto primaryInMemoryOut = std::make_shared<BitOutputStream>(primaryInMemory);

    auto secondaryInMemoryIn = std::make_shared<BitInputStream>(secondaryInMemory);
    auto secondaryInMemoryOut = std::make_shared<BitOutputStream>(secondaryInMemory);

    auto markupInMemoryIn = std::make_shared<BitInputStream>(markupInMemory);
    auto markupInMemoryOut = std::make_shared<BitOutputStream>(markupInMemory);

    auto secondaryEncoder = std::make_unique<ResidueSecondaryLogEncoder>(secondaryInMemoryOut);
    auto primaryEncoder = std::make_unique<PrimaryLogEncoder>(std::move(linkEncoder), std::move(storage), std::move(secondaryEncoder), primaryInMemoryOut, markupInMemoryOut, 4);

    auto primaryGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(primary)));
    auto secondaryGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(secondary)));
    auto markupGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(markup)));

    return SubPrePcsEncoder(std::move(primaryEncoder), std::move(primaryInMemoryIn), std::move(secondaryInMemoryIn), std::move(markupInMemoryIn),
                            std::move(primaryGenericEncoder), std::move(secondaryGenericEncoder), std::move(markupGenericEncoder));
}

SubPrePcsEncoder
SubPrePcsEncoder::CreatePPM(std::shared_ptr<BitOutputStream> primary, std::shared_ptr<BitOutputStream> secondary,
                            std::shared_ptr<BitOutputStream> markup) {
    auto linkEncoder = std::make_unique<ResidueLinkEncoder>(255);
    auto storage = std::make_unique<GreedyLogStorage>(255);

    auto primaryInMemory = std::make_shared<std::stringstream>();
    auto secondaryInMemory = std::make_shared<std::stringstream>();
    auto markupInMemory = std::make_shared<std::stringstream>();

    auto primaryInMemoryIn = std::make_shared<BitInputStream>(primaryInMemory);
    auto primaryInMemoryOut = std::make_shared<BitOutputStream>(primaryInMemory);

    auto secondaryInMemoryIn = std::make_shared<BitInputStream>(secondaryInMemory);
    auto secondaryInMemoryOut = std::make_shared<BitOutputStream>(secondaryInMemory);

    auto markupInMemoryIn = std::make_shared<BitInputStream>(markupInMemory);
    auto markupInMemoryOut = std::make_shared<BitOutputStream>(markupInMemory);

    auto secondaryEncoder = std::make_unique<PPMSecondaryLogEncoder>(secondaryInMemoryOut);
    auto primaryEncoder = std::make_unique<PrimaryLogEncoder>(std::move(linkEncoder), std::move(storage), std::move(secondaryEncoder), primaryInMemoryOut, markupInMemoryOut, 4);

    auto primaryGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(primary)));
    auto secondaryGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(secondary)));
    auto markupGenericEncoder = std::make_unique<ModellingEncoder>(ModellingEncoder::CreateDefault(std::move(markup)));

    return SubPrePcsEncoder(std::move(primaryEncoder), std::move(primaryInMemoryIn), std::move(secondaryInMemoryIn), std::move(markupInMemoryIn),
                            std::move(primaryGenericEncoder), std::move(secondaryGenericEncoder), std::move(markupGenericEncoder));
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