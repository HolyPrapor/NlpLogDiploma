//
// Created by zeliboba on 5/22/24.
//

#include <catch2/catch_test_macros.hpp>
#include "utils/bwt.hpp"

std::vector<Token> toTokens(const std::string& str) {
    std::vector<Token> tokens;
    for (char c : str) {
        tokens.push_back(static_cast<Token>(c));
    }
    return tokens;
}

void encodeAndDecode(const std::string& originalString) {
    auto bwt = BWT(100000);
    auto tokens = toTokens(originalString);
    auto encoded = bwt.Encode(tokens);
    auto decoded = bwt.Decode(encoded);
    REQUIRE(tokens == decoded);
}

TEST_CASE("BWT + MTF + ZLE round-trip", "[BWT]") {
    SECTION("Test with a small string") {
        std::string smallString = "abcabcaabbccabc";
        encodeAndDecode(smallString);
    }

    SECTION("Test with a large string") {
        std::string largeString;
        for (int i = 0; i < 1000; ++i) {
            largeString += "abcalsdkjfhsakljdfhlaskdjfh";
        }
        encodeAndDecode(largeString);
    }

    SECTION("Another regression") {
        std::string logString = "2016-09-28 04:30:30, Info                  CBS    Starting TrustedInstaller initialization.\n2016-09-28 04:30:30, Info                  CBS    Loaded Servicing Stack v6.1.7601.23505 with Core: C:\\Windows\\winsxs\\amd64_microsoft-windows-servicingstack_31bf3856ad364e35_6.1.7601.23505_none_681aa442f6fed7f0\\cbscore.dll";
        encodeAndDecode(logString);
    }
}