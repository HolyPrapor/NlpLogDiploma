//
// Created by zeliboba on 2/18/24.
//

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/arithmetic/arithmetic_encoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/arithmetic/arithmetic_decoder.hpp"
#include "modelling/ppm_model.hpp"

namespace fs = std::filesystem;

static const std::uint64_t num_bits = 32;
static const int context_size = 4;

static void encodeAndDecode(const std::string& originalString, const fs::path& tempFilePath) {
    {
        auto outputFileStream = std::make_shared<std::ofstream>(tempFilePath, std::ios::binary);
        auto bitOutputStream = std::make_shared<BitOutputStream>(outputFileStream);
        ArithmeticEncoder encoder(num_bits, std::move(bitOutputStream));
        PPMEncoderModel model(context_size);

        auto tokens = model.TokenizeChunk(std::vector<unsigned char>(originalString.begin(), originalString.end()));

        for (Token token : tokens) {
            model.Feed(token);
            model.EncodeNextToken(encoder, token);
        }

        auto lastToken = model.GetEndOfFileToken();
        model.Feed(lastToken);
        model.EncodeNextToken(encoder, lastToken);

        encoder.Finish();
    }

    std::vector<Token> decodedTokens;
    {
        bool isFinished = false;
        auto inputFileStream = std::make_shared<std::ifstream>(tempFilePath, std::ios::binary);
        auto bitInputStream = std::make_shared<BitInputStream>(inputFileStream);
        ArithmeticDecoder decoder(num_bits, std::move(bitInputStream));
        PPMDecoderModel model(context_size);

        for (auto i = 0; i < 2 * originalString.size(); i++) {
            auto token = model.DecodeNextToken(decoder);
            if (token == model.GetEndOfFileToken()) {
                isFinished = true;
                break;
            }
            model.Feed(token);
            decodedTokens.push_back(token);
        }

        REQUIRE(isFinished);

        std::vector<unsigned char> decodedBytes = model.DetokenizeChunk(decodedTokens);
        std::string decodedString(decodedBytes.begin(), decodedBytes.end());
        REQUIRE(originalString == decodedString);
    }
}

TEST_CASE("PPM arithmetic coding round-trip", "[PPMCoder]") {
    const auto tempDir = fs::temp_directory_path();
    const auto tempFilePath = tempDir / "temp_arithmetic_coding_test.bin";

    SECTION("Test with a small string") {
        std::string smallString = "abcabcaabbccabc";
        encodeAndDecode(smallString, tempFilePath);
    }

    SECTION("Test with a large string") {
        std::string largeString;
        for (int i = 0; i < 1000; ++i) {
            largeString += "abcalsdkjfhsakljdfhlaskdjfh";
        }
        encodeAndDecode(largeString, tempFilePath);
    }

    SECTION("Weird bug regression") {
        // 84 byte, 0 byte, 255 byte, 255 byte, 255 byte, 255 byte
        std::string weirdString = std::string{
                static_cast<unsigned char>(84),
                static_cast<unsigned char>(0),
                static_cast<char>(255),
                static_cast<char>(255),
                static_cast<char>(255),
                static_cast<char>(255),
                static_cast<char>(255),
                static_cast<char>(255)
        };
        encodeAndDecode(weirdString, tempFilePath);
    }

    SECTION("Another regression") {
        std::string logString = "2016-09-28 04:30:30, Info                  CBS    Starting TrustedInstaller initialization.\n2016-09-28 04:30:30, Info                  CBS    Loaded Servicing Stack v6.1.7601.23505 with Core: C:\\Windows\\winsxs\\amd64_microsoft-windows-servicingstack_31bf3856ad364e35_6.1.7601.23505_none_681aa442f6fed7f0\\cbscore.dll";
        encodeAndDecode(logString, tempFilePath);
    }

    fs::remove(tempFilePath);
}