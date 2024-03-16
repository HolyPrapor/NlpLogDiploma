//
// Created by zeliboba on 3/11/24.
//

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/generic/subprepcs_encoder.hpp"
#include "encoding/generic/subprepcs_decoder.hpp"

namespace fs = std::filesystem;

bool areFilesEqual(std::ifstream& file1, std::ifstream& file2) {
    std::istreambuf_iterator<char> iter1(file1), iter2(file2);
    std::istreambuf_iterator<char> end;

    while (iter1 != end && iter2 != end) {
        if (*iter1 != *iter2) {
            return false; // Files differ
        }
        ++iter1;
        ++iter2;
    }
    return iter1 == end && iter2 == end; // Files are equal if both are at the end
}

void compressAndDecompressFile(const std::string& basePath, const std::string& logSize, const std::string& logType) {
    std::string testFilePath = basePath + "test_files/logs/" + logSize + "/" + logType + ".log";
    const std::string prefix = "subprepcs_" + logSize + "_" + logType;

    const auto tempDir = fs::temp_directory_path();
    const auto decodedFilePath = tempDir / (prefix + ".log");
    const auto tempPrimaryFilePath = tempDir / (prefix + "primary.bin");
    const auto tempSecondaryFilePath = tempDir / (prefix + "secondary.bin");
    const auto tempMarkupFilePath = tempDir / (prefix + "markup.bin");

    SECTION("Encode: " + logSize + "-" + logType) {
        auto tempPrimary = std::make_shared<std::ofstream>(tempPrimaryFilePath, std::ios::binary);
        auto tempBitPrimary = std::make_shared<BitOutputStream>(tempPrimary);
        auto tempSecondary = std::make_shared<std::ofstream>(tempSecondaryFilePath, std::ios::binary);
        auto tempBitSecondary = std::make_shared<BitOutputStream>(tempSecondary);
        auto tempMarkup = std::make_shared<std::ofstream>(tempMarkupFilePath, std::ios::binary);
        auto tempBitMarkup = std::make_shared<BitOutputStream>(tempMarkup);

        auto subPrePcsEncoder = SubPrePcsEncoder::CreateDefault(tempBitPrimary, tempBitSecondary, tempBitMarkup);
        auto logData = std::make_shared<std::ifstream>(testFilePath, std::ios::binary);
        auto logBitData = std::make_shared<BitInputStream>(logData);
        subPrePcsEncoder.Encode(*logBitData);
        subPrePcsEncoder.Finish();
    }

    SECTION("Decode: " + logSize + "-" + logType) {
        auto tempPrimary = std::make_shared<std::ifstream>(tempPrimaryFilePath, std::ios::binary);
        auto tempBitPrimary = std::make_shared<BitInputStream>(tempPrimary);
        auto tempSecondary = std::make_shared<std::ifstream>(tempSecondaryFilePath, std::ios::binary);
        auto tempBitSecondary = std::make_shared<BitInputStream>(tempSecondary);
        auto tempMarkup = std::make_shared<std::ifstream>(tempMarkupFilePath, std::ios::binary);
        auto tempBitMarkup = std::make_shared<BitInputStream>(tempMarkup);

        auto subPrePcsDecoder = SubPrePcsDecoder::CreateDefault(tempBitPrimary, tempBitSecondary, tempBitMarkup);
        auto logData = std::make_shared<std::ofstream>(decodedFilePath, std::ios::binary);
        auto logBitData = std::make_shared<BitOutputStream>(logData);
        subPrePcsDecoder.Decode(*logBitData);
        subPrePcsDecoder.Finish();
    }

    SECTION("Compare: " + logSize + "-" + logType) {
        auto originalSize = fs::file_size(testFilePath);
        auto compressedSize = fs::file_size(tempPrimaryFilePath) + fs::file_size(tempSecondaryFilePath) + fs::file_size(tempMarkupFilePath);
        std::cout << "Compressed log size: " << compressedSize << " bytes" << std::endl;
        std::cout << "Original log size: " << originalSize << " bytes" << std::endl;
        REQUIRE(compressedSize < originalSize);
        REQUIRE(areFilesEqual(*std::make_shared<std::ifstream>(testFilePath), *std::make_shared<std::ifstream>(decodedFilePath)));
    }

    // keep the files for debugging
    //    fs::remove(tempFilePath);
}

TEST_CASE("SubPrePCS coding", "[SubPrePCS]") {
    std::string basePath;
    if (const char* envBasePath = std::getenv("TEST_BASE_PATH")) {
        basePath = envBasePath; // Use the environment variable if it's set
    } else {
        basePath = "../../"; // Default to local running path
    }

    for (auto& p: fs::directory_iterator(basePath + "test_files/logs/")) {
        auto dirName = p.path().filename().string();
        for (auto& file: fs::directory_iterator(p.path())) {
            auto fileName = file.path().stem().string();
            compressAndDecompressFile(basePath, dirName, fileName);
        }
    }
}