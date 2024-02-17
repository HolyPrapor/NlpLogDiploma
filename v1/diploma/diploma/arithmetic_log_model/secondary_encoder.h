#include <cstddef>
#include <string_view>
#include <vector>

class SecondaryEncoder {
public:
    virtual void OnMainEncode(const std::vector<size_t>& encoded) {};

    virtual void OnLineEncodeFinished(const std::vector<size_t>& line) {};

    virtual void Write(const size_t &token) = 0;

    virtual void Finish() = 0;

    virtual void
    Encode(const std::string_view &input_file_name, const std::string_view &output_file_name) = 0;
};