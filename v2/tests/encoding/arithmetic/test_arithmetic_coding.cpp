//
// Created by zeliboba on 2/17/24.
//

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/arithmetic/arithmetic_encoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/arithmetic/arithmetic_decoder.hpp"

namespace fs = std::filesystem;

static std::vector<int> generate_frequencies() {
    std::vector<int> frequencies(256, 2);
    for (int i = 1; i < 256; ++i) {
        frequencies[i] += frequencies[i - 1];
    }
    return frequencies;

}

static const std::uint64_t num_bits = 32;
static const unsigned char eof = '@';
static std::vector<int> frequencies = generate_frequencies();

static void encodeAndDecode(const std::string& originalString, const fs::path& tempFilePath) {
    {
        std::ofstream outputFileStream(tempFilePath, std::ios::binary);
        auto bitOutputStream = std::make_unique<BitOutputStream>(outputFileStream);
        ArithmeticEncoder encoder(num_bits, std::move(bitOutputStream));

        for (unsigned char symbol : originalString) {
            encoder.Write(frequencies, symbol);
        }

        encoder.Finish();
    }

    std::string decodedString;
    {
        bool isFinished = false;
        std::ifstream inputFileStream(tempFilePath, std::ios::binary);
        auto bitInputStream = std::make_unique<BitInputStream>(inputFileStream);
        ArithmeticDecoder decoder(num_bits, std::move(bitInputStream));

        for (auto i = 0; i < originalString.size(); i++) {
            unsigned char symbol = decoder.Read(frequencies);
            decodedString.append(1, static_cast<char>(symbol));
            if (symbol == eof) {
                isFinished = true;
                break;
            }
        }

        REQUIRE(isFinished);
    }

    REQUIRE(originalString == decodedString);
}

TEST_CASE("Arithmetic coding round-trip", "[ArithmeticCoder]") {
    const auto tempDir = fs::temp_directory_path();
    const auto tempFilePath = tempDir / "temp_arithmetic_coding_test.bin";

    SECTION("Test with a small string") {
        std::string smallString = "abc";
        smallString.append(1, static_cast<char>(eof));
        encodeAndDecode(smallString, tempFilePath);
    }

    SECTION("Test with a large string") {
        std::string largeString;
        for (int i = 0; i < 1000; ++i) {
            largeString += "abc";
        }
        largeString.append(1, static_cast<char>(eof));
        encodeAndDecode(largeString, tempFilePath);
    }

    fs::remove(tempFilePath);
}