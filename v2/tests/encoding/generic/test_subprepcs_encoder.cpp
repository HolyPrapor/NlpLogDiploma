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

TEST_CASE("SubPrePCS sample coding", "[SubPrePCS]") {
    const std::string prefix = "temp_subprepcs";

    const auto tempDir = fs::temp_directory_path();
    const auto decodedFilePath = tempDir / (prefix + ".log");
    const auto tempPrimaryFilePath = tempDir / (prefix + "primary.bin");
    const auto tempSecondaryFilePath = tempDir / (prefix + "secondary.bin");
    const auto tempMarkupFilePath = tempDir / (prefix + "markup.bin");

    SECTION("Test encode with a small log") {
        auto tempPrimary = std::make_shared<std::ofstream>(tempPrimaryFilePath, std::ios::binary);
        auto tempBitPrimary = std::make_shared<BitOutputStream>(tempPrimary);
        auto tempSecondary = std::make_shared<std::ofstream>(tempSecondaryFilePath, std::ios::binary);
        auto tempBitSecondary = std::make_shared<BitOutputStream>(tempSecondary);
        auto tempMarkup = std::make_shared<std::ofstream>(tempMarkupFilePath, std::ios::binary);
        auto tempBitMarkup = std::make_shared<BitOutputStream>(tempMarkup);

        auto subPrePcsEncoder = SubPrePcsEncoder::CreateDefault(tempBitPrimary, tempBitSecondary, tempBitMarkup);
        auto logData = std::make_shared<std::ifstream>("/home/zeliboba/projects/University/NlpLogDiploma/test_files/logs/small/java.log");
        auto logBitData = std::make_shared<BitInputStream>(logData);
        subPrePcsEncoder.Encode(*logBitData);
        subPrePcsEncoder.Finish();
    }

    SECTION("Test decode with a small log") {
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

//    fs::remove(tempFilePath);
    std::cout << "tempFilePath: " << tempDir << std::endl;
}