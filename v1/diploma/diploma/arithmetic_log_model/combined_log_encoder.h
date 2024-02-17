#pragma once

#include <memory>
#include "../arithmetic_coding/arithmetic_encoder.cpp"
#include "../probability_model/model.h"
#include "../log_model/record_storage.h"
#include "../log_model/coder.h"
#include "secondary_encoder.h"


class CombinedLogEncoder {
private:
    std::unique_ptr<Coder> log_encoder_;
    std::unique_ptr<RecordStorage> storage_;
    std::unique_ptr<BitOutputStream> lz_output_stream_;
    std::unique_ptr<BitOutputStream> auxiliary_output_stream_;
    std::unique_ptr<SecondaryEncoder> secondary_encoder_;

    void WriteToStream(const std::vector<size_t> &elements, const bool& is_auxiliary = true);
public:
    CombinedLogEncoder(std::unique_ptr<Coder> log_encoder,
                       std::unique_ptr<RecordStorage> storage,
                       std::unique_ptr<BitOutputStream> lz_output_stream,
                       std::unique_ptr<BitOutputStream> auxiliary_output_stream,
                       std::unique_ptr<SecondaryEncoder> secondary_encoder);

    void EncodeLines(const std::vector<std::vector<size_t>>& lines);
    std::vector<size_t> Encode(const std::vector<size_t>& tokens);
};