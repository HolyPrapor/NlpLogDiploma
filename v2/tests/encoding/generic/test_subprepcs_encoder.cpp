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

namespace fs = std::filesystem;

TEST_CASE("SubPrePCS sample coding", "[SubPrePCS]") {
    const std::string prefix = "temp_subprepcs";

    const auto tempDir = fs::temp_directory_path();
    const auto tempPrimaryFilePath = tempDir / (prefix + "primary.bin");
    const auto tempSecondaryFilePath = tempDir / (prefix + "secondary.bin");
    const auto tempMarkupFilePath = tempDir / (prefix + "markup.bin");

    auto tempPrimary = std::make_shared<std::ofstream>(tempPrimaryFilePath, std::ios::binary);
    auto tempBitPrimary = std::make_shared<BitOutputStream>(tempPrimary);
    auto tempSecondary = std::make_shared<std::ofstream>(tempSecondaryFilePath, std::ios::binary);
    auto tempBitSecondary = std::make_shared<BitOutputStream>(tempSecondary);
    auto tempMarkup = std::make_shared<std::ofstream>(tempMarkupFilePath, std::ios::binary);
    auto tempBitMarkup = std::make_shared<BitOutputStream>(tempMarkup);

    auto subPrePcsEncoder = SubPrePcsEncoder::CreateDefault(tempBitPrimary, tempBitSecondary, tempBitMarkup);

    SECTION("Test with a small log") {
        auto st = std::stringstream();
        st << "test";
        char buffer[4];
        st.read(buffer, 4);
        st.peek(), st.eof();
        st.clear();
        st << "pepe";
        st.flush();
        st.peek(), st.eof();
        st.read(buffer, 4);

        auto logData = std::make_shared<std::ifstream>("/home/zeliboba/projects/University/NlpLogDiploma/test_files/logs/small/java.log");
        auto logBitData = std::make_shared<BitInputStream>(logData);
        subPrePcsEncoder.Encode(*logBitData);
        subPrePcsEncoder.Finish();
    }

//    fs::remove(tempFilePath);
    std::cout << "tempFilePath: " << tempDir << std::endl;
}