//
// Created by zeliboba on 2/25/24.
//

#include <catch2/catch_test_macros.hpp>
#include "modelling/naive_model.hpp"
#include "encoding/log/link/residue_link_decoder.hpp"
#include "encoding/log/link/residue_link_encoder.hpp"

TEST_CASE("ResidueLinkEncoder correctly encodes", "[ResidueLinkEncoder]") {
    std::stringstream stream;
    BitOutputStream outputStream(stream);
    ResidueLinkEncoder encoder;
    std::string expected;

    SECTION("Encodes link") {
        encoder.EncodeLink(outputStream, {1, 2, 3});
        expected = "\x01\x02\x03";
    }

    SECTION("Encodes token") {
        encoder.EncodeToken(outputStream, 4);
        expected = "\x04";
    }

    SECTION("Encodes big token") {
        encoder.EncodeToken(outputStream, 255 + 1);
        expected = "\xff\x01";
    }

    SECTION("Encodes very big token") {
        encoder.EncodeToken(outputStream, 255 + 128 + 4);
        expected = "\xff\x80\x04";
    }

    SECTION("Encodes very big link") {
        encoder.EncodeLink(outputStream, {255 + 1, 255 + 2, 255 + 3});
        expected = "\xff\x01\xff\x02\xff\x03";
    }

    outputStream.Close();
    REQUIRE(stream.str() == expected);
}

TEST_CASE("ResidueLogDecoder correctly decodes", "[ResidueLogDecoder]") {
    std::stringstream stream;
    BitInputStream inputStream(stream);
    ResidueLinkDecoder decoder;
    std::string input;

    SECTION("Decodes link") {
        input = "\x01\x02\x03";
        stream.write(input.c_str(), input.size());
        stream.seekg(0);
        auto link = decoder.DecodeLink(inputStream);
        REQUIRE(link.RecordIndex == 1);
        REQUIRE(link.StartIndex == 2);
        REQUIRE(link.Length == 3);
    }

    SECTION("Decodes token") {
        input = "\x04";
        stream.write(input.c_str(), input.size());
        stream.seekg(0);
        auto token = decoder.DecodeToken(inputStream);
        REQUIRE(token == 4);
    }

    SECTION("Decodes big token") {
        input = "\xff\x01";
        stream.write(input.c_str(), input.size());
        stream.seekg(0);
        auto token = decoder.DecodeToken(inputStream);
        REQUIRE(token == 255 + 1);
    }

    SECTION("Decodes very big token") {
        input = "\xff\x80\x04";
        stream.write(input.c_str(), input.size());
        stream.seekg(0);
        auto token = decoder.DecodeToken(inputStream);
        REQUIRE(token == 255 + 128 + 4);
    }
}

TEST_CASE("Residue round trip", "[ResidueLinkCoder]") {
    std::stringstream stream;
    BitOutputStream outputStream(stream);
    ResidueLinkEncoder encoder;
    ResidueLinkDecoder decoder;
    LogLink link = {1, 2, 3};
    Token token = 4;

    encoder.EncodeLink(outputStream, link);
    encoder.EncodeToken(outputStream, token);
    outputStream.Close();

    std::string input = stream.str();
    stream.seekg(0);
    BitInputStream inputStream(stream);

    auto decodedLink = decoder.DecodeLink(inputStream);
    auto decodedToken = decoder.DecodeToken(inputStream);

    REQUIRE(decodedLink.RecordIndex == link.RecordIndex);
    REQUIRE(decodedLink.StartIndex == link.StartIndex);
    REQUIRE(decodedLink.Length == link.Length);
    REQUIRE(decodedToken == token);
}