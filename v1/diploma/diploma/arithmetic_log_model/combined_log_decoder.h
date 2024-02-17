#include <memory>
#include "../log_model/coder.h"
#include "../log_model/record_storage.h"
#include "../arithmetic_coding/bit_output_stream.h"
#include "secondary_decoder.h"

class CombinedLogDecoder {
private:
    std::unique_ptr<Coder> log_encoder_;
    std::unique_ptr<RecordStorage> storage_;
    std::shared_ptr<BitInputStream> lz_input_stream_;
    std::shared_ptr<BitInputStream> auxiliary_input_stream_;
    std::unique_ptr<SecondaryDecoder> secondary_decoder_;

    size_t GetNextInt(const bool& is_auxiliary, const size_t& size = 1);
public:
    CombinedLogDecoder(std::unique_ptr<Coder> log_encoder,
                       std::unique_ptr<RecordStorage> storage,
                       std::unique_ptr<BitInputStream> lz_input_stream,
                       std::unique_ptr<BitInputStream> auxiliary_input_stream,
                       std::unique_ptr<SecondaryDecoder> secondary_decoder);

    // TODO: check size types in this methods
    std::vector<std::vector<size_t>> DecodeText();
    std::vector<size_t> Decode();
};