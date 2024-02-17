//
// Created by zeliboba on 5/29/23.
//

#include <stdexcept>
#include <fstream>
#include <memory>
#include "secondary_decoder.h"
#include "../arithmetic_coding/arithmetic_decoder.h"
#include "../probability_model/ppm_model.h"
#include "../bwt/bwt_coder.h"

class ArithmeticPPMDecoder: public SecondaryDecoder {
private:
    int contextSize;
    bool useBwt;
    std::vector<unsigned char> lines;
    size_t pointer;

public:
    ArithmeticPPMDecoder(
            const std::string_view &encoded,
            const std::string_view &decoded,
            int context_size=3,
            bool use_bwt=true
    ) : contextSize(context_size), useBwt(use_bwt), pointer(0) {
        Decode_(encoded, decoded, true);
    }

    unsigned char Read() override {
        pointer += 1;
        return lines[pointer - 1];
    }

    void Finish() override {}

    void Decode(const std::string_view &input, const std::string_view &output) override {
        Decode_(input, output);
    }

private:
    void Decode_(const std::string_view &input, const std::string_view &output, bool saveInMemory = false) {
        decode_ppm(input, output, contextSize, useBwt);
        if (saveInMemory) {
            std::ifstream f(output.data(), std::ios::binary);
            if (!f) {
                throw std::runtime_error("Could not open file " + std::string(output.data()));
            }
            lines.assign((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
            f.close();
        }
    }

    static void decode_ppm(const std::string_view &encoded_filename, const std::string_view &output_filename, int context_size = 3, bool use_bwt = true) {
        std::string_view encoded = encoded_filename;
        std::string_view bwt_decoded = "bwt_decoded";
        std::string_view decoded = output_filename;

        if (!use_bwt) {
            bwt_decoded = decoded;
        }

        std::ifstream f(encoded.data(), std::ios::binary);
        if (!f) {
            throw std::runtime_error("Could not open file " + std::string(encoded));
        }
        ArithmeticDecoder decoder(32, std::make_unique<BitInputStream>(f));
        std::vector<Token> tokens;
        PPMModelDecoder model(decoder, context_size, tokens);

        while (true) {
            auto frequencies = model.GetFrequencies();
            Token current_token(decoder.Read(frequencies));
            if (current_token.value == model.GetEndOfFileToken().value) {
                break;
            }
            model.Feed({current_token});
            if (tokens.back().value == model.GetEndOfFileToken().value) {
                tokens.pop_back();
                break;
            }
        }

        std::string text = model.Postprocess(tokens);
        std::ofstream o(bwt_decoded.data(), std::ios::binary);
        if (!o) {
            throw std::runtime_error("Could not open file " + std::string(bwt_decoded));
        }
        o.write(text.data(), text.size());
        o.close();

        if (use_bwt) {
            BwtCoder bwt;
            bwt.Decode(
                    std::string(bwt_decoded),
                    std::string(encoded.data()) + ".indices",
                    std::string(encoded.data()) + ".eof",
                    std::string(decoded)
            );
        }
    }
};

