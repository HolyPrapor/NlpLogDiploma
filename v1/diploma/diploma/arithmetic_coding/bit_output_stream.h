#include <fstream>

class BitOutputStream {
    std::ofstream output_stream_;
    size_t num_bits_filled_;
    unsigned char current_byte_;
    bool is_closed_;

public:
    explicit BitOutputStream(std::ofstream& output_stream);
    // TODO: think about method parameter type
    void Write(const size_t& bit);
    void Close();
};