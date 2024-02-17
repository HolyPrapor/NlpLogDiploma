#include <cstddef>
#include <string_view>
#include <vector>

class SecondaryDecoder {
public:
    virtual void OnMainDecode(const std::vector<size_t>& decoded) {};

    virtual void OnLineDecodeFinished(const std::vector<size_t>& line) {};

    virtual unsigned char Read() = 0;

    virtual void Finish() = 0;

    virtual void
    Decode(const std::string_view &input_file_name, const std::string_view &output_file_name) = 0;
};