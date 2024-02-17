#include <iostream>
#include <vector>
#include <chrono>
#include "arithmetic_coding/arithmetic_decoder.cpp"
#include "arithmetic_log_model/combined_log_encoder.cpp"
#include "log_model/coders/smart_coder.h"
#include "log_model/storages/move_to_front_record_storage.h"
#include "arithmetic_log_model/arithmetic_ppm_encoder.h"

std::unique_ptr<BitOutputStream> createOutputStream(std::string filepath) {
    std::ofstream stream(filepath);
    return std::make_unique<BitOutputStream>(stream);
}

std::vector<std::vector<size_t>> readFileIntoVector(const std::string& filename) {
    std::vector<std::vector<size_t>> result;

    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::vector<size_t> lineVector;
        std::istringstream lineStream(line);

        char value;
        while (lineStream >> value) {
            lineVector.push_back(static_cast<size_t>(value));
        }

        result.push_back(lineVector);
    }

    return result;
}

// todo: cleaning?
void encode(std::string &filepath) {
    ArithmeticEncoder secondaryEncoder(32, createOutputStream(filepath + "_secondary"));
    CombinedLogEncoder encoder(std::make_unique<SmartCoder>(),
            std::make_unique<MoveToFrontStorage>(50),
                    createOutputStream(filepath + "_main"),
                    createOutputStream(filepath + "_auxiliary"),
                    std::make_unique<ArithmeticPPMEncoder>(secondaryEncoder, 5)
    );

    auto inputLines = readFileIntoVector(filepath);

    encoder.EncodeLines(inputLines);
}

int main() {
    std::vector<std::string> filenames{
        "android"
//        "bgl"
//        "hdfs"
//        "java"
//        "windows"
    };
    std::vector<std::string> dir_names{"small"};
    for (auto dirname : dir_names) {
        for (auto filename : filenames) {
            std::string filepath = "/home/lexlippi/PycharmProjects/NlpLogDiploma/diploma/diploma/test_files/logs/" + dirname + "/" + filename + ".log";
            std::cout << filepath << std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            encode(filepath);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Encoding time for " << filepath << ": " << elapsed.count() << " seconds." << std::endl;
        }
    }
    return 0;
}
