//
// Created by lexlippi on 28.05.23.
//

#include "combined_log_decoder.h"

CombinedLogDecoder::CombinedLogDecoder(
        std::unique_ptr<Coder> log_encoder,
        std::unique_ptr<RecordStorage> storage,
        std::unique_ptr<BitInputStream> lz_input_stream,
        std::unique_ptr<BitInputStream> auxiliary_input_stream,
        std::unique_ptr<SecondaryDecoder> secondary_decoder) :
        log_encoder_{std::move(log_encoder)},
        storage_{std::move(storage)},
        lz_input_stream_{std::move(lz_input_stream)},
        auxiliary_input_stream_{std::move(auxiliary_input_stream)},
        secondary_decoder_{std::move(secondary_decoder)} {}

size_t CombinedLogDecoder::GetNextInt(const bool &is_auxiliary, const size_t &size) {
    return log_encoder_->DecodeIntFromStream(
            is_auxiliary ? auxiliary_input_stream_ : lz_input_stream_, size);
}

std::vector<std::vector<size_t>> CombinedLogDecoder::DecodeText() {
    std::vector<std::vector<size_t>> result;
    for (auto i = 0; i < GetNextInt(true, 2); ++i) {
        auto tokens = Decode();
        result.push_back(tokens);
    }
    return result;
}

std::vector<size_t> CombinedLogDecoder::Decode() {
    std::vector<size_t> line;
    auto iterations = GetNextInt(true, 2);
    while (iterations > 0) {
        switch (auxiliary_input_stream_->Read()) {
            case 0: {
                auto link = log_encoder_->DecodeLinkFromStream(lz_input_stream_);
                auto record_index = std::get<0>(link);
                auto start_index = std::get<1>(link);
                auto length = std::get<2>(link);
                std::vector<size_t> parsed_link = {storage_->GetRecords()[record_index].begin() +
                                    static_cast<int>(start_index),
                                    storage_->GetRecords()[record_index].begin() +
                                    static_cast<int>(start_index + length)};
                line.insert(line.end(), parsed_link.begin(), parsed_link.end());
                secondary_decoder_->OnMainDecode(parsed_link);
                break;
            }
            case 1:
                line.push_back(secondary_decoder_->Read());
                break;
            default:
                throw std::runtime_error("Unexpected bit in stream");

        }
        iterations--;
    }
    storage_->StoreRecord(line);
    secondary_decoder_->OnLineDecodeFinished(line);
    return line;
}