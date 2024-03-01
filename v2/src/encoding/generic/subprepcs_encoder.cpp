//
// Created by zeliboba on 3/1/24.
//

#include <vector>
#include <sstream>
#include "subprepcs_encoder.hpp"
#include "defs.hpp"
#include "encoding/log/link/residue_link_encoder.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"
#include "encoding/log/secondary/naive_secondary_log_encoder.hpp"

SubPrePcsEncoder SubPrePcsEncoder::CreateDefault(std::unique_ptr<BitOutputStream>&& primary, std::unique_ptr<BitOutputStream>&& secondary, std::unique_ptr<BitOutputStream>&& markup) {
    auto linkEncoder = std::make_unique<ResidueLinkEncoder>(255);
    auto storage = std::make_unique<GreedyLogStorage>(50);

    // todo: change to unique pointers?
    auto primaryInMemory = std::stringstream();
    auto secondaryInMemory = std::stringstream();
    auto markupInMemory = std::stringstream();

    auto primaryInMemoryIn = std::make_unique<BitInputStream>(primaryInMemory);
    auto primaryInMemoryOut = std::make_unique<BitOutputStream>(primaryInMemory);

    auto secondaryInMemoryIn = std::make_unique<BitInputStream>(secondaryInMemory);
    auto secondaryInMemoryOut = std::make_unique<BitOutputStream>(secondaryInMemory);

    auto markupInMemoryIn = std::make_unique<BitInputStream>(markupInMemory);
    auto markupInMemoryOut = std::make_unique<BitOutputStream>(markupInMemory);

    auto secondaryEncoder = std::make_unique<NaiveSecondaryLogEncoder>(std::move(secondaryInMemoryOut));
    // todo: change to unique pointers
    auto primaryEncoder = std::make_unique<PrimaryLogEncoder>(*linkEncoder, *storage, *secondaryEncoder, std::move(primaryInMemoryOut), std::move(markupInMemoryOut), 4);

    // todo: change to specific encoders
    auto primaryGenericEncoder = std::make_unique<GenericEncoder>(std::move(primaryInMemoryIn));
    auto secondaryGenericEncoder = std::make_unique<GenericEncoder>(std::move(secondaryInMemoryIn));
    auto markupGenericEncoder = std::make_unique<GenericEncoder>(std::move(markupInMemoryIn));

    return SubPrePcsEncoder(std::move(primaryEncoder), std::move(secondaryEncoder), std::move(primaryInMemoryIn), std::move(secondaryInMemoryIn), std::move(markupInMemoryIn),
                            std::move(primaryGenericEncoder), std::move(secondaryGenericEncoder), std::move(markupGenericEncoder));
}

SubPrePcsEncoder::SubPrePcsEncoder(std::unique_ptr<PrimaryLogEncoder>&& primaryEncoder, std::unique_ptr<SecondaryLogEncoder>&& secondaryEncoder,
                                   std::unique_ptr<BitInputStream>&& primary, std::unique_ptr<BitInputStream>&& secondary, std::unique_ptr<BitInputStream>&& markup,
                                   std::unique_ptr<GenericEncoder>&& primaryGenericEncoder, std::unique_ptr<GenericEncoder>&& secondaryGenericEncoder, std::unique_ptr<GenericEncoder>&& markupGenericEncoder) {
    this->primaryEncoder = std::move(primaryEncoder);
    this->secondaryEncoder = std::move(secondaryEncoder);
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