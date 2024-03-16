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

void saveToCsv(const std::string& logSize, const std::string& logType, ulong originalSize, ulong compressedSize, long compressionMs, long decompressionMs) {
    const auto resultsPath = fs::temp_directory_path() / "subprepcs_results.csv";
    std::ofstream csvFile;
    csvFile.open(resultsPath, std::ios::app);

    if (csvFile.tellp() == 0) {
        csvFile << "logSize,logType,originalSize,compressedSize,compressionMs,decompressionMs" << std::endl;
    }

    csvFile << logSize << "," << logType << "," << originalSize << "," << compressedSize << "," << compressionMs << "," << decompressionMs << std::endl;
    csvFile.close();
}

void encode(const std::string& testFile, const std::string& primary, const std::string& secondary, const std::string& markup) {
    auto primaryFile = std::make_shared<std::ofstream>(primary, std::ios::binary);
    auto primaryStream = std::make_shared<BitOutputStream>(primaryFile);
    auto secondaryFile = std::make_shared<std::ofstream>(secondary, std::ios::binary);
    auto secondaryStream = std::make_shared<BitOutputStream>(secondaryFile);
    auto markupFile = std::make_shared<std::ofstream>(markup, std::ios::binary);
    auto markupStream = std::make_shared<BitOutputStream>(markupFile);

    auto subPrePcsEncoder = SubPrePcsEncoder::CreateDefault(primaryStream, secondaryStream, markupStream);
    auto logData = std::make_shared<std::ifstream>(testFile, std::ios::binary);
    auto logBitData = std::make_shared<BitInputStream>(logData);
    subPrePcsEncoder.Encode(*logBitData);
    subPrePcsEncoder.Finish();
}

void decode(const std::string& primary, const std::string& secondary, const std::string& markup, const std::string& decoded) {
    auto primaryFile = std::make_shared<std::ifstream>(primary, std::ios::binary);
    auto primaryStream = std::make_shared<BitInputStream>(primaryFile);
    auto secondaryFile = std::make_shared<std::ifstream>(secondary, std::ios::binary);
    auto secondaryStream = std::make_shared<BitInputStream>(secondaryFile);
    auto markupFile = std::make_shared<std::ifstream>(markup, std::ios::binary);
    auto markupStream = std::make_shared<BitInputStream>(markupFile);

    auto subPrePcsDecoder = SubPrePcsDecoder::CreateDefault(primaryStream, secondaryStream, markupStream);
    auto logData = std::make_shared<std::ofstream>(decoded, std::ios::binary);
    auto logBitData = std::make_shared<BitOutputStream>(logData);
    subPrePcsDecoder.Decode(*logBitData);
    subPrePcsDecoder.Finish();
}

void compressAndDecompressFile(const std::string& basePath, const std::string& logSize, const std::string& logType) {
    std::string testFilePath = basePath + "test_files/logs/" + logSize + "/" + logType + ".log";
    const std::string prefix = "subprepcs_" + logSize + "_" + logType + "_";

    const auto baseDir = fs::temp_directory_path();
    const auto decoded = baseDir / (prefix + ".log");
    const auto primary = baseDir / (prefix + "primary.bin");
    const auto secondary = baseDir / (prefix + "secondary.bin");
    const auto markup = baseDir / (prefix + "markup.bin");

    auto start = std::chrono::high_resolution_clock::now();
    encode(testFilePath, primary, secondary, markup);
    auto encodeEnd = std::chrono::high_resolution_clock::now();
    auto encodeMs = std::chrono::duration_cast<std::chrono::milliseconds>(encodeEnd - start).count();

    auto startDecode = std::chrono::high_resolution_clock::now();
    decode(primary, secondary, markup, decoded);
    auto decodeEnd = std::chrono::high_resolution_clock::now();
    auto decodeMs = std::chrono::duration_cast<std::chrono::milliseconds>(decodeEnd - startDecode).count();

    auto originalSize = fs::file_size(testFilePath);
    auto compressedSize = fs::file_size(primary) + fs::file_size(secondary) + fs::file_size(markup);
    REQUIRE(compressedSize < originalSize);
    REQUIRE(areFilesEqual(*std::make_shared<std::ifstream>(testFilePath), *std::make_shared<std::ifstream>(decoded)));
    std::cout << "Compressed " << logSize << "-" << logType << " size: " << compressedSize << " bytes" << std::endl;
    std::cout << "Original " << logSize << "-" << logType <<  " size: "  << originalSize << " bytes" << std::endl;
    saveToCsv(logSize, logType, originalSize, compressedSize, encodeMs, decodeMs);

    // keep the files for debugging
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
            SECTION(dirName + "/" + fileName) {
                compressAndDecompressFile(basePath, dirName, fileName);
            }
        }
    }
}