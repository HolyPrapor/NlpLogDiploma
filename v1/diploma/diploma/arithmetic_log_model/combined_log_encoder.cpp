#include "combined_log_encoder.h"

std::vector<size_t> GetBitFromInt(const size_t &n) {
    std::vector<size_t> bits(8);
    for (auto i = 0; i < 8; ++i) {
        bits[i] = (n & (1 << (7 - i))) == (1 << (7 - i)) ? 1 : 0;
    }
    return bits;
}

std::vector<size_t> GetBits(const std::vector<size_t> &elements) {
    std::vector<size_t> bits;
    for (const auto &element: elements) {
        auto new_bits = GetBitFromInt(element);
        bits.insert(bits.end(), new_bits.begin(), new_bits.end());
    }
    return bits;
}

CombinedLogEncoder::CombinedLogEncoder(
        std::unique_ptr<Coder> log_encoder,
        std::unique_ptr<RecordStorage> storage,
        std::unique_ptr<BitOutputStream> lz_output_stream,
        std::unique_ptr<BitOutputStream> auxiliary_output_stream,
        std::unique_ptr<SecondaryEncoder> secondary_encoder) :
        log_encoder_{std::move(log_encoder)},
        storage_{std::move(storage)},
        lz_output_stream_{std::move(lz_output_stream)},
        auxiliary_output_stream_{std::move(auxiliary_output_stream)},
        secondary_encoder_{std::move(secondary_encoder)} {}

void
CombinedLogEncoder::WriteToStream(const std::vector<size_t> &elements, const bool &is_auxiliary) {
    for (const auto &bit: GetBits(elements)) {
        if (is_auxiliary) {
            auxiliary_output_stream_->Write(bit);
        } else {
            lz_output_stream_->Write(bit);
        }
    }
}

void CombinedLogEncoder::EncodeLines(const std::vector<std::vector<size_t>> &lines) {
    WriteToStream(log_encoder_->EncodeInt(lines.size(), 4));
    for (const auto &tokens: lines) {
        const auto operations = Encode(tokens);
        WriteToStream(log_encoder_->EncodeInt(operations.size(), 2));
        for (const auto &bit: operations) {
            auxiliary_output_stream_->Write(bit);
        }
    }
    secondary_encoder_->Finish();
}

std::vector<size_t> CombinedLogEncoder::Encode(const std::vector<size_t> &tokens) {
    std::vector<size_t> operations;
    size_t start = 0;
    while (start < tokens.size()) {
        auto record = storage_->GetLink(tokens, start);
        auto record_index = std::get<0>(record);
        auto record_start_index = std::get<1>(record);
        auto record_length = std::get<2>(record);
        if (record_length.has_value() && record_length.value() > 6) {
            const auto length = record_length.value();
            operations.push_back(0);
            auto encoded = log_encoder_->EncodeLink(record_index.value(),
                                                    record_start_index.value(),
                                                    length);
            storage_->MoveToFront(record_index.value());
            if (log_encoder_->IsBinary()) {
                for (const auto &bit: encoded) {
                    lz_output_stream_->Write(bit);
                }
            } else {
                WriteToStream(encoded, false);
            }
            // TODO: check how it works
            secondary_encoder_->OnMainEncode({tokens.begin() + static_cast<int>(start),
                                              tokens.begin() + static_cast<int>(start + length)});
            start += length;
        } else {
            operations.push_back(1);
            const auto length = std::min(static_cast<size_t>(4), tokens.size() - start);
            for (auto i = 0; i < length; ++i) {
                secondary_encoder_->Write(tokens[start + i]);
            }
            start += length;
        }
    }
    storage_->StoreRecord(tokens);
    secondary_encoder_->OnLineEncodeFinished(tokens);
    return operations;
}