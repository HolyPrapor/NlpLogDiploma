#pragma once

#include <vector>
#include <cstddef>
#include <string>
#include <memory>
#include <optional>
#include "../arithmetic_coding/bit_input_stream.h"

/**
 * Пока везде возвращаю std::tuple там где были Tuple в Питоне.
 * Но в идеале наверное перейти на структуры.
 */
class Coder {
private:
    bool is_binary_ = false;
public:
    explicit Coder(const bool& is_binary) : is_binary_{is_binary} {}

    virtual std::vector<size_t>
    EncodeLink(const size_t &record_index, const size_t &start_index, const size_t &length) = 0;

    virtual std::tuple<size_t, size_t, size_t, size_t>
    DecodeLink(const std::string &record, const size_t &index) = 0;

    /**
     *  У size было дефолтное значение None, но кажется, что оно бессмысленно.
     */
    virtual std::vector<size_t> EncodeInt(const size_t &value, const size_t& size) = 0;

    /**
     *  У size было дефолтное значение None, но кажется, что оно бессмысленно.
     */
    virtual std::tuple<size_t, size_t> DecodeInt(const size_t &value, const size_t& size) = 0;

    virtual size_t
    DecodeIntFromStream(std::shared_ptr<BitInputStream> stream, const size_t &size) = 0;

    virtual std::tuple<size_t, size_t, size_t>
    DecodeLinkFromStream(std::shared_ptr<BitInputStream> stream) = 0;

    virtual std::vector<size_t> EncodeToken(const unsigned char &symbol) = 0;

    virtual std::tuple<size_t, size_t>
    DecodeToken(const std::string &record, const size_t &index) = 0;

    virtual bool ShouldDecodeAsLink(const std::string &record, const size_t &index) = 0;

    /**
     * В коде на Питоне здесь были еще параметры record и start, но никто эти параметры
     * не использовал
     */
    virtual bool ShouldEncodeAsLink(const size_t &record_index, const size_t &start_index,
                                    const size_t &length) = 0;

    [[nodiscard]] bool IsBinary() const {
        return is_binary_;
    }
};