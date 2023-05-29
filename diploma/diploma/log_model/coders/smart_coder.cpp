//
// Created by zeliboba on 5/29/23.
//

#include "../coder.h"

class SmartCoder : Coder {
private:
    size_t max_value_;
public:
    explicit SmartCoder(size_t max_value = 127) : Coder(false) {
        if (max_value <= 0 || max_value > 255) {
            throw std::invalid_argument("max_value is not supported");
        }
        max_value_ = max_value;
    }

    std::vector<size_t> EncodeLink(const size_t &record_index, const size_t &start_index, const size_t &length) override {
        std::vector<size_t> record;
        record.insert(record.end(), EncodeInt(start_index, 0).begin(), EncodeInt(start_index, 0).end());
        record.insert(record.end(), EncodeInt(record_index, 0).begin(), EncodeInt(record_index, 0).end());
        record.insert(record.end(), EncodeInt(length, 0).begin(), EncodeInt(length, 0).end());
        return record;
    }

    std::vector<size_t> EncodeInt(const size_t &value, const size_t &size) override {
        std::vector<size_t> encoded;
        size_t val = value;
        while (val >= max_value_) {
            encoded.push_back(255);
            val -= max_value_;
        }
        encoded.push_back(255 - max_value_ + val);
        return encoded;
    }

    std::tuple<size_t, size_t> DecodeInt(const std::vector<size_t> &record, const size_t &i, const size_t &size) override {
        size_t value = 0;
        size_t index = i;
        while (record[index] == 255) {
            value += max_value_;
            index++;
        }
        value += record[index] - (255 - max_value_);
        index++;
        return std::make_tuple(value, index);
    }

    size_t DecodeIntFromStream(std::shared_ptr<BitInputStream> stream, const size_t &size) override {
        std::vector<size_t> decoded;
        decoded.push_back(stream->Read());
        while (decoded.back() == 255) {
            decoded.push_back(stream->Read());
        }
        return std::get<0>(DecodeInt(decoded, 0, size));
    }

    std::tuple<size_t, size_t, size_t> DecodeLinkFromStream(std::shared_ptr<BitInputStream> stream) override {
        return std::make_tuple(DecodeIntFromStream(stream, 0), DecodeIntFromStream(stream, 0), DecodeIntFromStream(stream, 0));
    }
};