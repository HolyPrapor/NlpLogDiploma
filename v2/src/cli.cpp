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
#include "encoding/log/secondary/ppm_secondary_log_encoder.hpp"
#include "encoding/log/link/residue_link_encoder.hpp"
#include "encoding/log/link/storage/greedy_log_storage.hpp"
#include "encoding/log/link/storage/mtf_log_storage.hpp"
#include "encoding/log/link/residue_link_decoder.hpp"
#include "encoding/log/secondary/ppm_secondary_log_decoder.hpp"
#include "compression_config.pb.h"
#include "src/default_compression_config_textproto.h"
#include "encoding/log/secondary/residue_secondary_log_encoder.hpp"
#include "encoding/log/secondary/naive_secondary_log_encoder.hpp"
#include "encoding/generic/identity_encoder.hpp"
#include "encoding/log/secondary/residue_secondary_log_decoder.hpp"
#include "encoding/log/secondary/naive_secondary_log_decoder.hpp"
#include "encoding/generic/identity_decoder.hpp"

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
    google::protobuf::TextFormat::Parser parser;
    parser.AllowUnknownField(false); // Disallow unknown fields
    if (!parser.ParseFromString(protoStr, &compressionConfig)) {
        throw std::runtime_error("Error: Failed to parse embedded textproto");
    }
    return compressionConfig;
}

CompressionConfig parseCompressionConfig() {
    CompressionConfig compressionConfig;

    // Check if std::cin has data
    if (std::cin.peek() != std::char_traits<char>::eof()) {
        std::ostringstream inputBuffer;
        inputBuffer << std::cin.rdbuf();
        std::string protoStr = inputBuffer.str();

        google::protobuf::TextFormat::Parser parser;
        parser.AllowUnknownField(false); // Disallow unknown fields
        if (!parser.ParseFromString(protoStr, &compressionConfig)) {
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

std::unique_ptr<LogLinkEncoder> createLogLinkEncoder(const PrimaryLogCoderConfig& config) {
    if (config.has_residue_log_link_coder()) {
        return std::make_unique<ResidueLinkEncoder>();
    }

    return std::make_unique<ResidueLinkEncoder>();
}

std::unique_ptr<LogStorage> createLogStorage(const PrimaryLogCoderConfig& config) {
    int storageSize = 255;
    if (config.storage_size() > 0) {
        storageSize = config.storage_size();
    }

    if (config.has_greedy_sliding_window_storage()) {
        return std::make_unique<GreedyLogStorage>(storageSize);
    }
    else if (config.has_greedy_move_to_front_storage()) {
        return std::make_unique<MtfLogStorage>(storageSize);
    }

    return std::make_unique<GreedyLogStorage>(storageSize);
}

std::unique_ptr<SecondaryLogEncoder> createSecondaryLogEncoder(const SecondaryLogCoderConfig& config, std::shared_ptr<BitOutputStream> secondaryStream) {
    std::unique_ptr<SecondaryLogEncoder> encoder = nullptr;

    if (config.has_ppm_secondary_log_coder()) {
        encoder = std::make_unique<PPMSecondaryLogEncoder>(secondaryStream, config.ppm_secondary_log_coder().window_size());
    }
    else if (config.has_residue_secondary_log_coder()) {
        encoder = std::make_unique<ResidueSecondaryLogEncoder>(secondaryStream);
    }
    else if (config.has_naive_secondary_log_coder()) {
        encoder = std::make_unique<NaiveSecondaryLogEncoder>(secondaryStream);
    }
    else {
        encoder = std::make_unique<PPMSecondaryLogEncoder>(secondaryStream);
    }

    return encoder;
}

std::unique_ptr<PrimaryLogEncoder> createPrimaryLogEncoder(const PrimaryLogCoderConfig& config, std::unique_ptr<SecondaryLogEncoder> secondaryLogEncoder, std::unique_ptr<LogLinkEncoder> linkEncoder, std::unique_ptr<LogStorage> storage, std::shared_ptr<BitOutputStream> primaryStream, std::shared_ptr<BitOutputStream> markupStream) {
    int minLinkLength = 6;
    if (config.min_link_length() > 0) {
        minLinkLength = config.min_link_length();
    }

    return std::make_unique<PrimaryLogEncoder>(std::move(linkEncoder), std::move(storage), std::move(secondaryLogEncoder), primaryStream, markupStream, minLinkLength);
}

std::unique_ptr<GenericEncoder> createGenericEncoder(const GenericCoderConfig& config, std::shared_ptr<BitOutputStream> stream) {
    if (config.has_identity_coder()) {
        return std::make_unique<IdentityEncoder>(stream);
    }
    if (config.has_modelling_coder()) {
        return ModellingEncoder::CreateDefault(stream, config.modelling_coder().context_size());
    }
    return ModellingEncoder::CreateDefault(stream);
}

void compress(const std::string& inputFile, const std::string& outputPrefix, const CompressionConfig& compressionConfig) {
    auto inFile = openInputFile(inputFile);

    auto primaryFile = openOutputFile(outputPrefix + "_primary");
    auto secondaryFile = openOutputFile(outputPrefix + "_secondary");
    auto markupFile = openOutputFile(outputPrefix + "_markup");

    auto primaryStream = std::make_shared<BitOutputStream>(primaryFile);
    auto secondaryStream = std::make_shared<BitOutputStream>(secondaryFile);
    auto markupStream = std::make_shared<BitOutputStream>(markupFile);

    PrimaryLogEncoderFactory primaryFactory = [&](std::unique_ptr<LogLinkEncoder> linkEncoder, std::unique_ptr<LogStorage> storage, std::unique_ptr<SecondaryLogEncoder> secondaryEncoder, std::shared_ptr<BitOutputStream> primaryStream, std::shared_ptr<BitOutputStream> secondaryStream) {
        return createPrimaryLogEncoder(compressionConfig.primary_log_coder(), std::move(secondaryEncoder), std::move(linkEncoder), std::move(storage), primaryStream, secondaryStream);
    };

    SecondaryLogEncoderFactory secondaryFactory = [&](std::shared_ptr<BitOutputStream> stream) {
        return createSecondaryLogEncoder(compressionConfig.secondary_log_coder(), stream);
    };

    GenericEncoderFactory primaryGenericFactory = [&](std::shared_ptr<BitOutputStream> stream) {
        return createGenericEncoder(compressionConfig.generic_primary_coder(), stream);
    };

    GenericEncoderFactory secondaryGenericFactory = [&](std::shared_ptr<BitOutputStream> stream) {
        return createGenericEncoder(compressionConfig.generic_secondary_coder(), stream);
    };

    GenericEncoderFactory markupGenericFactory = [&](std::shared_ptr<BitOutputStream> stream) {
        return createGenericEncoder(compressionConfig.generic_markup_coder(), stream);
    };

    auto linkEncoder = createLogLinkEncoder(compressionConfig.primary_log_coder());
    auto storage = createLogStorage(compressionConfig.primary_log_coder());

    SubPrePcsEncoder encoder = SubPrePcsEncoder::Create(primaryStream, secondaryStream, markupStream, primaryFactory, secondaryFactory, primaryGenericFactory, secondaryGenericFactory, markupGenericFactory, std::move(linkEncoder), std::move(storage));

    BitInputStream inputStream(inFile);
    encoder.Encode(inputStream);
    encoder.Finish();
}

std::unique_ptr<LogLinkDecoder> createLogLinkDecoder(const PrimaryLogCoderConfig& config) {
    if (config.has_residue_log_link_coder()) {
        return std::make_unique<ResidueLinkDecoder>();
    }

    return std::make_unique<ResidueLinkDecoder>();
}

std::unique_ptr<SecondaryLogDecoder> createSecondaryLogDecoder(const SecondaryLogCoderConfig& config, std::shared_ptr<BitInputStream> stream) {
    std::unique_ptr<SecondaryLogDecoder> decoder = nullptr;

    if (config.has_ppm_secondary_log_coder()) {
        decoder = std::make_unique<PPMSecondaryLogDecoder>(stream, config.ppm_secondary_log_coder().window_size());
    }
    else if (config.has_residue_secondary_log_coder()) {
        decoder = std::make_unique<ResidueSecondaryLogDecoder>(stream);
    }
    else if (config.has_naive_secondary_log_coder()) {
        decoder = std::make_unique<NaiveSecondaryLogDecoder>(stream);
    }
    else {
        decoder = std::make_unique<PPMSecondaryLogDecoder>(stream);
    }

    return decoder;
}

std::unique_ptr<GenericDecoder> createGenericDecoder(const GenericCoderConfig& config, std::shared_ptr<BitInputStream> stream) {
    if (config.has_identity_coder()) {
        return std::make_unique<IdentityDecoder>(stream);
    }
    if (config.has_modelling_coder()) {
        return ModellingDecoder::CreateDefault(stream, config.modelling_coder().context_size());
    }
    return ModellingDecoder::CreateDefault(stream);
}

void decompress(const std::string& inputPrefix, const std::string& outputFile, const CompressionConfig& compressionConfig) {
    auto primaryFile = openInputFile(inputPrefix + "_primary");
    auto secondaryFile = openInputFile(inputPrefix + "_secondary");
    auto markupFile = openInputFile(inputPrefix + "_markup");

    auto outFile = openOutputFile(outputFile);

    auto primaryStream = std::make_shared<BitInputStream>(primaryFile);
    auto secondaryStream = std::make_shared<BitInputStream>(secondaryFile);
    auto markupStream = std::make_shared<BitInputStream>(markupFile);

    PrimaryLogDecoderFactory primaryFactory = [&](std::unique_ptr<LogLinkDecoder> linkDecoder, std::unique_ptr<LogStorage> storage, std::unique_ptr<SecondaryLogDecoder> secondaryDecoder, std::shared_ptr<BitInputStream> primary, std::shared_ptr<BitInputStream> markup) {
        return std::make_unique<PrimaryLogDecoder>(std::move(linkDecoder), std::move(storage), std::move(secondaryDecoder), primary, markup);
    };

    SecondaryLogDecoderFactory secondaryFactory = [&](std::shared_ptr<BitInputStream> in) {
        return createSecondaryLogDecoder(compressionConfig.secondary_log_coder(), in);
    };

    GenericDecoderFactory primaryGenericFactory = [&](std::shared_ptr<BitInputStream> in) {
        return createGenericDecoder(compressionConfig.generic_primary_coder(), in);
    };

    GenericDecoderFactory secondaryGenericFactory = [&](std::shared_ptr<BitInputStream> in) {
        return createGenericDecoder(compressionConfig.generic_secondary_coder(), in);
    };

    GenericDecoderFactory markupGenericFactory = [&](std::shared_ptr<BitInputStream> in) {
        return createGenericDecoder(compressionConfig.generic_markup_coder(), in);
    };

    auto linkDecoder = createLogLinkDecoder(compressionConfig.primary_log_coder());
    auto storage = createLogStorage(compressionConfig.primary_log_coder());

    SubPrePcsDecoder decoder = SubPrePcsDecoder::Create(primaryStream, secondaryStream, markupStream, primaryFactory, secondaryFactory, primaryGenericFactory, secondaryGenericFactory, markupGenericFactory, std::move(linkDecoder), std::move(storage));

    BitOutputStream outputStream(outFile);
    decoder.Decode(outputStream);
    decoder.Finish(outputStream);
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