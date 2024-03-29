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

static std::vector<int> generate_uniform_frequencies() {
    std::vector<int> frequencies(256, 2);
    for (int i = 1; i < 256; ++i) {
        frequencies[i] += frequencies[i - 1];
    }
    return frequencies;
}

static std::vector<int> generate_non_uniform_frequencies() {
    std::vector<int> frequencies(256, 2);
    frequencies['a']++;
    frequencies['b']++;
    frequencies['c']++;
    for (int i = 1; i < 256; ++i) {
        frequencies[i] += frequencies[i - 1];
    }
    return frequencies;
}

static const std::uint64_t num_bits = 32;
static const unsigned char eof = '@';

static void encodeAndDecode(const std::string& originalString, const std::vector<int>& frequencies, const fs::path& tempFilePath) {
    {
        auto outputFileStream = std::make_shared<std::ofstream>(tempFilePath, std::ios::binary);
        auto bitOutputStream = std::make_shared<BitOutputStream>(outputFileStream);
        ArithmeticEncoder encoder(num_bits, bitOutputStream);

        for (unsigned char symbol : originalString) {
            encoder.Write(frequencies, symbol);
        }

        encoder.Finish();
    }

    std::string decodedString;
    {
        bool isFinished = false;
        auto inputFileStream = std::make_shared<std::ifstream>(tempFilePath, std::ios::binary);
        auto bitInputStream = std::make_shared<BitInputStream>(inputFileStream);
        ArithmeticDecoder decoder(num_bits, bitInputStream);

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
        SECTION("with uniform frequencies") {
            encodeAndDecode(smallString, generate_uniform_frequencies(), tempFilePath);
        }
        SECTION("with non-uniform frequencies") {
            encodeAndDecode(smallString, generate_non_uniform_frequencies(), tempFilePath);
        }
    }

    SECTION("Test with a large string") {
        std::string largeString;
        for (int i = 0; i < 1000; ++i) {
            largeString += "abc";
        }
        largeString.append(1, static_cast<char>(eof));
        SECTION("with uniform frequencies") {
            encodeAndDecode(largeString, generate_uniform_frequencies(), tempFilePath);
        }
        SECTION("with non-uniform frequencies") {
            encodeAndDecode(largeString, generate_non_uniform_frequencies(), tempFilePath);
        }
    }

    fs::remove(tempFilePath);
}