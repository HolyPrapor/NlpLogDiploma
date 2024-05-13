#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <google/protobuf/text_format.h>
#include "encoding/generic/subprepcs_encoder.hpp"
#include "encoding/generic/subprepcs_decoder.hpp"
#include "encoding/utils/bit_input_stream.hpp"
#include "encoding/utils/bit_output_stream.hpp"
#include "compression_config.pb.h"
#include "src/default_compression_config_textproto.h"

namespace fs = std::filesystem;

void createDirectoriesForFile(const std::string& filePath) {
    fs::path path(filePath);
    fs::path dir = path.parent_path();
    if (!dir.empty() && !fs::exists(dir)) {
        fs::create_directories(dir);
    }
}

CompressionConfig parseEmbeddedConfig() {
    CompressionConfig compressionConfig;
    std::string protoStr(reinterpret_cast<const char*>(default_compression_config_textproto), default_compression_config_textproto_len);
    if (!google::protobuf::TextFormat::ParseFromString(protoStr, &compressionConfig)) {
        throw std::runtime_error("Error: Failed to parse embedded textproto");
    }
    return compressionConfig;
}

CompressionConfig parseCompressionConfig() {
    CompressionConfig compressionConfig;

    // Check if std::cin has data
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::ostringstream inputBuffer;
        inputBuffer << std::cin.rdbuf();
        std::string protoStr = inputBuffer.str();

        if (!google::protobuf::TextFormat::ParseFromString(protoStr, &compressionConfig)) {
            throw std::runtime_error("Error: Failed to parse textproto from stdin");
        }
    } else {
        compressionConfig = parseEmbeddedConfig();
    }

    return compressionConfig;
}

std::shared_ptr<std::ifstream> openInputFile(const std::string& filePath) {
    auto file = std::make_shared<std::ifstream>(filePath, std::ios::binary);
    if (!file->is_open()) {
        throw std::runtime_error("Error: Cannot open input file " + filePath);
    }
    return file;
}

std::shared_ptr<std::ofstream> openOutputFile(const std::string& filePath) {
    createDirectoriesForFile(filePath);
    auto file = std::make_shared<std::ofstream>(filePath, std::ios::binary);
    if (!file->is_open()) {
        throw std::runtime_error("Error: Cannot open output file " + filePath);
    }
    return file;
}

void compress(const std::string& inputFile, const std::string& outputPrefix, const CompressionConfig& compressionConfig) {
    auto inFile = openInputFile(inputFile);

    auto primaryFile = openOutputFile(outputPrefix + "_primary");
    auto secondaryFile = openOutputFile(outputPrefix + "_secondary");
    auto markupFile = openOutputFile(outputPrefix + "_markup");

    auto primaryStream = std::make_shared<BitOutputStream>(primaryFile);
    auto secondaryStream = std::make_shared<BitOutputStream>(secondaryFile);
    auto markupStream = std::make_shared<BitOutputStream>(markupFile);

    SubPrePcsEncoder encoder = SubPrePcsEncoder::CreatePPM(primaryStream, secondaryStream, markupStream);

    BitInputStream inputStream(inFile);
    encoder.Encode(inputStream);
    encoder.Finish();
}

void decompress(const std::string& inputPrefix, const std::string& outputFile, const CompressionConfig& compressionConfig) {
    auto primaryFile = openInputFile(inputPrefix + "_primary");
    auto secondaryFile = openInputFile(inputPrefix + "_secondary");
    auto markupFile = openInputFile(inputPrefix + "_markup");

    auto outFile = openOutputFile(outputFile);

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

    CompressionConfig compressionConfig;
    try {
        compressionConfig = parseCompressionConfig();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    try {
        if (command == "compress") {
            compress(inputFile, outputFile, compressionConfig);
        } else if (command == "decompress") {
            decompress(inputFile, outputFile, compressionConfig);
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