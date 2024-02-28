//
// Created by zeliboba on 2/28/24.
//

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <memory>
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/log/link/residue_link_encoder.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"
#include "encoding/log/secondary/secondary_log_encoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/log/secondary/naive_secondary_log_encoder.hpp"
#include "encoding/log/primary_log_encoder.hpp"
#include "encoding/log/link/residue_link_decoder.hpp"
#include "encoding/log/secondary/naive_secondary_log_decoder.hpp"
#include "encoding/log/primary_log_decoder.hpp"

static std::vector<Token> toTokens(const std::string& line) {
    std::vector<Token> tokens;
    for (char c : line) {
        tokens.push_back(static_cast<Token>(c));
    }
    return tokens;
}

static void encodeAndDecode(const std::vector<std::string>& originalLines) {
    std::vector<std::vector<Token>> originalLogs;
    originalLogs.reserve(originalLines.size());
    for (const auto& line : originalLines) {
        originalLogs.push_back(toTokens(line));
    }

    std::stringstream mainOutputStream;
    std::stringstream secondaryOutputStream;
    std::stringstream markupOutputStream;
    {
        auto mainBitOutputStream = std::make_unique<BitOutputStream>(mainOutputStream);
        auto secondaryBitOutputStream = std::make_unique<BitOutputStream>(secondaryOutputStream);
        auto markupBitOutputStream = std::make_unique<BitOutputStream>(markupOutputStream);

        ResidueLinkEncoder linkEncoder;
        GreedyLogStorage storage(50);
        NaiveSecondaryLogEncoder secondaryLogEncoder(std::move(secondaryBitOutputStream));
        PrimaryLogEncoder primaryLogEncoder(linkEncoder, storage, secondaryLogEncoder, std::move(mainBitOutputStream), std::move(markupBitOutputStream), 2);

        for (const auto& line : originalLogs) {
            primaryLogEncoder.EncodeLine(line);
        }
    }

    {
        auto mainBitInputStream = std::make_unique<BitInputStream>(mainOutputStream);
        auto secondaryBitInputStream = std::make_unique<BitInputStream>(secondaryOutputStream);
        auto markupBitInputStream = std::make_unique<BitInputStream>(markupOutputStream);

        ResidueLinkDecoder linkDecoder;
        GreedyLogStorage storage(50);
        NaiveSecondaryLogDecoder secondaryLogDecoder(std::move(secondaryBitInputStream));
        PrimaryLogDecoder primaryLogDecoder(linkDecoder, storage, secondaryLogDecoder, std::move(mainBitInputStream), std::move(markupBitInputStream));

        for (const auto & originalLog : originalLogs) {
            auto decodedLine = primaryLogDecoder.DecodeLine();
            REQUIRE(decodedLine == originalLog);
        }
    }
}

TEST_CASE("Primary log coding round-trip", "[PrimaryLogCoder]") {
    SECTION("Single line") {
        encodeAndDecode({"abc"});
    }

    SECTION("Multiple lines, no links") {
        encodeAndDecode({"abc", "def", "ghi"});
    }

    SECTION("Multiple lines, with links") {
        encodeAndDecode({"abc", "def", "abd", "dek", "abcdef", "defabc"});
    }

    SECTION("Single long line") {
        encodeAndDecode({"abc" + std::string(1000, 'd') + "efg"});
    }

    SECTION("Multiple long lines, no links") {
        encodeAndDecode({std::string(1000, 'a'), std::string(1000, 'b'), std::string(1000, 'c')});
    }

    // todo: fix
//    SECTION("Multiple long lines, with links") {
//        encodeAndDecode({std::string(1000, 'a'), std::string(1000, 'b'), std::string(1000, 'a') + std::string(1000, 'b'), std::string(1000, 'b') + std::string(1000, 'c'), std::string(1000, 'a') + std::string(1000, 'b') + std::string(1000, 'c'), std::string(1000, 'b') + std::string(1000, 'c') + std::string(1000, 'a')});
//    }
}