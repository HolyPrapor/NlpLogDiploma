//
// Created by zeliboba on 3/11/24.
//

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <functional>
#include "encoding/utils/bit_output_stream.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/generic/subprepcs_encoder.hpp"
#include "encoding/generic/subprepcs_decoder.hpp"
#include "encoding/log/link/storage/mtf_log_storage.hpp"

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

void saveToCsv(const std::string& coderName, const std::string& logSize, const std::string& logType, ulong originalSize, ulong compressedSize, long compressionMs, long decompressionMs) {
    const auto resultsPath = fs::temp_directory_path() / "subprepcs_results.csv";
    std::ofstream csvFile;
    csvFile.open(resultsPath, std::ios::app);

    if (csvFile.tellp() == 0) {
        csvFile << "coderName,logSize,logType,originalSize,compressedSize,compressionMs,decompressionMs" << std::endl;
    }

    csvFile << coderName << "," << logSize << "," << logType << "," << originalSize << "," << compressedSize << "," << compressionMs << "," << decompressionMs << std::endl;
    csvFile.close();
}

void encode(const std::string& testFile, const std::string& primary, const std::string& secondary, const std::string& markup,
            const std::function<SubPrePcsEncoder(std::shared_ptr<BitOutputStream>, std::shared_ptr<BitOutputStream>, std::shared_ptr<BitOutputStream>)>& createEncoder) {
    auto primaryFile = std::make_shared<std::ofstream>(primary, std::ios::binary);
    auto primaryStream = std::make_shared<BitOutputStream>(primaryFile);
    auto secondaryFile = std::make_shared<std::ofstream>(secondary, std::ios::binary);
    auto secondaryStream = std::make_shared<BitOutputStream>(secondaryFile);
    auto markupFile = std::make_shared<std::ofstream>(markup, std::ios::binary);
    auto markupStream = std::make_shared<BitOutputStream>(markupFile);

    auto subPrePcsEncoder = createEncoder(primaryStream, secondaryStream, markupStream);
    auto logData = std::make_shared<std::ifstream>(testFile, std::ios::binary);
    auto logBitData = std::make_shared<BitInputStream>(logData);
    subPrePcsEncoder.Encode(*logBitData);
    subPrePcsEncoder.Finish();
}

void decode(const std::string& primary, const std::string& secondary, const std::string& markup, const std::string& decoded,
            const std::function<SubPrePcsDecoder(std::shared_ptr<BitInputStream>, std::shared_ptr<BitInputStream>, std::shared_ptr<BitInputStream>)>& createDecoder) {
    auto primaryFile = std::make_shared<std::ifstream>(primary, std::ios::binary);
    auto primaryStream = std::make_shared<BitInputStream>(primaryFile);
    auto secondaryFile = std::make_shared<std::ifstream>(secondary, std::ios::binary);
    auto secondaryStream = std::make_shared<BitInputStream>(secondaryFile);
    auto markupFile = std::make_shared<std::ifstream>(markup, std::ios::binary);
    auto markupStream = std::make_shared<BitInputStream>(markupFile);

    auto subPrePcsDecoder = createDecoder(primaryStream, secondaryStream, markupStream);
    auto logData = std::make_shared<std::ofstream>(decoded, std::ios::binary);
    auto logBitData = std::make_shared<BitOutputStream>(logData);
    subPrePcsDecoder.Decode(*logBitData);
    subPrePcsDecoder.Finish(*logBitData);
}

void compressAndDecompressFile(const std::string &basePath, const std::string &logSize, const std::string &logType,
                               const std::string& coderName,
                               const std::function<SubPrePcsEncoder(std::shared_ptr<BitOutputStream>,
                                                                    std::shared_ptr<BitOutputStream>,
                                                                    std::shared_ptr<BitOutputStream>)> &createEncoder,
                               const std::function<SubPrePcsDecoder(std::shared_ptr<BitInputStream>,
                                                                    std::shared_ptr<BitInputStream>,
                                                                    std::shared_ptr<BitInputStream>)> &createDecoder) {
    std::string testFilePath = basePath + "test_files/logs/" + logSize + "/" + logType + ".log";
    const std::string prefix = "subprepcs_" + coderName + "_" + logSize + "_" + logType;

    const auto baseDir = fs::temp_directory_path();
    const auto decoded = baseDir / (prefix + ".log");
    const auto primary = baseDir / (prefix + "_primary.bin");
    const auto secondary = baseDir / (prefix + "_secondary.bin");
    const auto markup = baseDir / (prefix + "_markup.bin");

    auto start = std::chrono::high_resolution_clock::now();
    encode(testFilePath, primary, secondary, markup, createEncoder);
    auto encodeEnd = std::chrono::high_resolution_clock::now();
    auto encodeMs = std::chrono::duration_cast<std::chrono::milliseconds>(encodeEnd - start).count();

    auto startDecode = std::chrono::high_resolution_clock::now();
    decode(primary, secondary, markup, decoded, createDecoder);
    auto decodeEnd = std::chrono::high_resolution_clock::now();
    auto decodeMs = std::chrono::duration_cast<std::chrono::milliseconds>(decodeEnd - startDecode).count();

    auto originalSize = fs::file_size(testFilePath);
    auto compressedSize = fs::file_size(primary) + fs::file_size(secondary) + fs::file_size(markup);
    REQUIRE(compressedSize < originalSize);
    REQUIRE(areFilesEqual(*std::make_shared<std::ifstream>(testFilePath), *std::make_shared<std::ifstream>(decoded)));
    std::cout << coderName << " " << logSize << "-" << logType << " compression ratio: " << (double)originalSize / compressedSize  << std::endl;
    std::cout << "Compressed " << logSize << "-" << logType << " size: " << compressedSize << " bytes" << std::endl;
    std::cout << "Original " << logSize << "-" << logType <<  " size: "  << originalSize << " bytes" << std::endl;
    saveToCsv(coderName, logSize, logType, originalSize, compressedSize, encodeMs, decodeMs);

    // keep the files for debugging
}

void runAgainstTestFiles(const std::string& basePath,
                         const std::string& coderName,
                         const std::function<SubPrePcsEncoder(std::shared_ptr<BitOutputStream>, std::shared_ptr<BitOutputStream>, std::shared_ptr<BitOutputStream>)>& createEncoder,
                         const std::function<SubPrePcsDecoder(std::shared_ptr<BitInputStream>, std::shared_ptr<BitInputStream>, std::shared_ptr<BitInputStream>)>& createDecoder) {
    for (auto& p: fs::directory_iterator(basePath + "test_files/logs/")) {
        auto dirName = p.path().filename().string();
//        if (dirName != "small") {
//            continue; // Skip non-small logs
//        }
        for (auto& file: fs::directory_iterator(p.path())) {
            auto fileName = file.path().stem().string();
//            if (fileName != "android" && fileName != "java")
//                continue;
            SECTION(dirName + "/" + fileName) {
                compressAndDecompressFile(basePath, dirName, fileName, coderName, createEncoder, createDecoder);
            }
        }
    }
}
TEST_CASE("SubPrePCS coding", "[SubPrePCS]") {
    std::string basePath;
    if (const char* envBasePath = std::getenv("TEST_BASE_PATH")) {
        basePath = envBasePath; // Use the environment variable if it's set
    } else {
        basePath = "../../"; // Default to local running path
    }

    SECTION("naive") {
        runAgainstTestFiles(basePath, "naive", [](std::shared_ptr<BitOutputStream> p, std::shared_ptr<BitOutputStream> s, std::shared_ptr<BitOutputStream> m) -> SubPrePcsEncoder {return SubPrePcsEncoder::CreateNaive(p, s, m); },
                            [](std::shared_ptr<BitInputStream> p, std::shared_ptr<BitInputStream> s, std::shared_ptr<BitInputStream> m) -> SubPrePcsDecoder {return SubPrePcsDecoder::CreateNaive(p, s, m); });
    }

    SECTION("residue") {
        runAgainstTestFiles(basePath, "residue", [](std::shared_ptr<BitOutputStream> p, std::shared_ptr<BitOutputStream> s, std::shared_ptr<BitOutputStream> m) -> SubPrePcsEncoder {return SubPrePcsEncoder::CreateResidue(p, s, m); },
                            [](std::shared_ptr<BitInputStream> p, std::shared_ptr<BitInputStream> s, std::shared_ptr<BitInputStream> m) -> SubPrePcsDecoder {return SubPrePcsDecoder::CreateResidue(p, s, m); });
    }

    SECTION("ppm") {
        runAgainstTestFiles(basePath, "ppm",
                            [](std::shared_ptr<BitOutputStream> p, std::shared_ptr<BitOutputStream> s, std::shared_ptr<BitOutputStream> m) -> SubPrePcsEncoder {return SubPrePcsEncoder::CreatePPM(p, s, m); },
                            [](std::shared_ptr<BitInputStream> p, std::shared_ptr<BitInputStream> s, std::shared_ptr<BitInputStream> m) -> SubPrePcsDecoder {return SubPrePcsDecoder::CreatePPM(p, s, m); });
    }

    SECTION("ppm with mtf") {
        runAgainstTestFiles(basePath, "ppm-mtf",
                            [](std::shared_ptr<BitOutputStream> p, std::shared_ptr<BitOutputStream> s, std::shared_ptr<BitOutputStream> m) -> SubPrePcsEncoder {return SubPrePcsEncoder::CreatePPM(p, s, m, nullptr, std::make_unique<MtfLogStorage>(255)); },
                            [](std::shared_ptr<BitInputStream> p, std::shared_ptr<BitInputStream> s, std::shared_ptr<BitInputStream> m) -> SubPrePcsDecoder {return SubPrePcsDecoder::CreatePPM(p, s, m, nullptr, std::make_unique<MtfLogStorage>(255)); });
    }
}