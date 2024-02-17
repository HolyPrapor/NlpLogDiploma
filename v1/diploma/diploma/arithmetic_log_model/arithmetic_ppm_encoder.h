//
// Created by zeliboba on 5/29/23.
//

#include <iostream>
#include "../arithmetic_coding/arithmetic_encoder.h"
#include "../probability_model/ppm_model.h"
#include "../bwt/bwt_coder.h"

class ArithmeticPPMEncoder : public SecondaryEncoder {
public:
    ArithmeticPPMEncoder(ArithmeticEncoder& encoder, int context_size = 3)
            : encoder(encoder), model(encoder, context_size) {}

    void OnMainEncode(const std::vector<size_t>& encoded) override {
        for (const auto& token : encoded) {
            model.Feed({ Token(static_cast<int>(token)) });
            model.UpdateTrie();
        }
    }

    void Write(const size_t &token) override {
        model.Feed({ Token(static_cast<int>(token)) });
        auto frequencies = model.GetFrequencies();
        encoder.Write(frequencies, static_cast<int>(token));
    }

    void Finish() override {
        encoder.Finish();
    }

    void Encode(const std::string_view &input_file_name, const std::string_view &output_file_name) override {
        encode_ppm(input_file_name, output_file_name, model.context_size);
    }

private:
    ArithmeticEncoder& encoder;
    PPMEncoderModel model;

    static void encode_ppm(const std::string_view &input_filename, const std::string_view &output_filename, int context_size = 3, bool use_bwt = true) {
        std::string_view to_encode = input_filename;
        std::string_view bwt_encoded = "bwt_encoded";
        std::string_view encoded = output_filename;

        if (use_bwt) {
            BwtCoder bwt;
            bwt.Encode(
                    std::string(to_encode),
                    std::string(bwt_encoded),
                    std::string(output_filename) + ".indices",
                    std::string(output_filename) + ".eof"
            );
        } else {
            bwt_encoded = to_encode;
        }

        std::ifstream f(bwt_encoded.data(), std::ios::binary);
        std::string input_text((std::istreambuf_iterator<char>(f)),
                                     std::istreambuf_iterator<char>());
        f.close();

        std::ofstream o(encoded.data(), std::ios::binary);

        ArithmeticEncoder encoder(32, std::make_unique<BitOutputStream>(o));
        PPMEncoderModel model(encoder, context_size);
        auto tokens = model.Preprocess(input_text);
        tokens.push_back(model.GetEndOfFileToken());

        int token_count = 0;
        for (auto token : tokens) {
            model.Feed({ token });
            auto frequencies = model.GetFrequencies();
            encoder.Write(frequencies, token.value);
            token_count++;
            if (token_count % 10000 == 0) {
                std::cout << "Encoded " << token_count << " tokens...\n";
            }
        }
        encoder.Finish();
    }
};
