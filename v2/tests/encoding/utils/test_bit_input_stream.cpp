//
// Created by zeliboba on 2/17/24.
//

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <encoding/utils/bit_input_stream.hpp>

namespace fs = std::filesystem;

void writeAndTestBits(const fs::path& tempFilePath, unsigned char writtenByte) {
    {
        std::ofstream out(tempFilePath, std::ios::binary);
        REQUIRE(out.is_open());
        out.write(reinterpret_cast<const char*>(&writtenByte), sizeof(writtenByte));
        REQUIRE(out.good());
    }

    std::ifstream inputFile(tempFilePath, std::ios::binary);
    REQUIRE(inputFile.is_open());

    BitInputStream bitInputStream(inputFile);

    for (int i = 7; i >= 0; --i) {
        std::optional<bool> bit = bitInputStream.Read();
        REQUIRE(bit.has_value());
        bool expectedBit = (writtenByte >> i) & 1;
        REQUIRE(bit.value() == expectedBit);
    }
}

TEST_CASE("BitInputStream correctly reads bits from a file", "[BitInputStream]") {
    fs::path tempDir = fs::temp_directory_path() / "tmp";
    fs::create_directories(tempDir);
    auto tempFilePath = tempDir / "test.bin";

    SECTION("Test with alternating bits starting with 1") {
        unsigned char writtenByte = 0b10101010;
        writeAndTestBits(tempFilePath, writtenByte);
    }

    SECTION("Test with alternating bits starting with 0") {
        unsigned char writtenByte = 0b01010101;
        writeAndTestBits(tempFilePath, writtenByte);
    }

    SECTION("Test with all bits set") {
        unsigned char writtenByte = 0b11111111;
        writeAndTestBits(tempFilePath, writtenByte);
    }

    SECTION("Test with no bits set") {
        unsigned char writtenByte = 0b00000000;
        writeAndTestBits(tempFilePath, writtenByte);
    }

    fs::remove(tempFilePath);
}

