#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include "encoding/generic/subprepcs_encoder.hpp"
#include "encoding/generic/subprepcs_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/utils/bit_output_stream.hpp"

namespace fs = std::filesystem;

void createDirectoriesForFile(const std::string& filePath) {
    fs::path path(filePath);
    fs::path dir = path.parent_path();
    if (!dir.empty() && !fs::exists(dir)) {
        fs::create_directories(dir);
    }
}

void compress(const std::string& inputFile, const std::string& outputPrefix) {
    auto inFile = std::make_shared<std::ifstream>(inputFile, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Error: Cannot open input file " + inputFile);
    }

    createDirectoriesForFile(outputPrefix + "_primary");
    createDirectoriesForFile(outputPrefix + "_secondary");
    createDirectoriesForFile(outputPrefix + "_markup");

    auto primaryFile = std::make_shared<std::ofstream>(outputPrefix + "_primary", std::ios::binary);
    auto secondaryFile = std::make_shared<std::ofstream>(outputPrefix + "_secondary", std::ios::binary);
    auto markupFile = std::make_shared<std::ofstream>(outputPrefix + "_markup", std::ios::binary);

    if (!primaryFile || !secondaryFile || !markupFile) {
        throw std::runtime_error("Error: Cannot open output files with prefix " + outputPrefix);
    }

    auto primaryStream = std::make_shared<BitOutputStream>(primaryFile);
    auto secondaryStream = std::make_shared<BitOutputStream>(secondaryFile);
    auto markupStream = std::make_shared<BitOutputStream>(markupFile);

    SubPrePcsEncoder encoder = SubPrePcsEncoder::CreatePPM(primaryStream, secondaryStream, markupStream);

    BitInputStream inputStream(inFile);
    encoder.Encode(inputStream);
    encoder.Finish();
}

void decompress(const std::string& inputPrefix, const std::string& outputFile) {
    auto primaryFile = std::make_shared<std::ifstream>(inputPrefix + "_primary", std::ios::binary);
    auto secondaryFile = std::make_shared<std::ifstream>(inputPrefix + "_secondary", std::ios::binary);
    auto markupFile = std::make_shared<std::ifstream>(inputPrefix + "_markup", std::ios::binary);

    if (!primaryFile || !secondaryFile || !markupFile) {
        throw std::runtime_error("Error: Cannot open input files with prefix " + inputPrefix);
    }

    createDirectoriesForFile(outputFile);

    auto outFile = std::make_shared<std::ofstream>(outputFile, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Error: Cannot open output file " + outputFile);
    }

    auto primaryStream = std::make_shared<BitInputStream>(primaryFile);
    auto secondaryStream = std::make_shared<BitInputStream>(secondaryFile);
    auto markupStream = std::make_shared<BitInputStream>(markupFile);

    SubPrePcsDecoder decoder = SubPrePcsDecoder::CreatePPM(primaryStream, secondaryStream, markupStream);

    BitOutputStream outputStream(outFile);
    decoder.Decode(outputStream);
    decoder.Finish();
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage:\n"
                  << "  compress <inputfile> <outputprefix>\n"
                  << "  decompress <inputprefix> <outputfile>\n";
        return 1;
    }

    std::string command = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    try {
        if (command == "compress") {
            compress(inputFile, outputFile);
        } else if (command == "decompress") {
            decompress(inputFile, outputFile);
        } else {
            std::cerr << "Error: Unknown command " << command << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
