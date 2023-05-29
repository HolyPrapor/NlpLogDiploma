//
// Created by zeliboba on 5/29/23.
//

#include <algorithm>
#include "../record_storage.h"

class GreedyRecordStorage : public RecordStorage {
protected:
    std::vector<std::vector<size_t>> log_records_;

    static int find_subarray(const std::vector<size_t>& record, const std::vector<size_t>& subarray) {
        auto it = std::search(record.begin(), record.end(), subarray.begin(), subarray.end());
        if(it != record.end()) {
            return it - record.begin();
        } else {
            return -1;
        }
    }

public:
    explicit GreedyRecordStorage(const size_t &window_size)
            : RecordStorage(window_size) {}

    void Clear() override {
        log_records_.clear();
    }

    std::vector<std::vector<size_t>> GetRecords() override {
        return log_records_;
    }

    std::tuple<std::optional<size_t>, std::optional<size_t>, std::optional<size_t>>
    GetLink(const std::vector<size_t> &line, const size_t &start) override {
        size_t length = 1;
        std::optional<size_t> record_index, record_start_index, record_length;

        for(size_t r = 0; r < log_records_.size(); r++) {
            std::vector<size_t> subarray(line.begin() + start, line.begin() + start + length);
            int index = find_subarray(log_records_[r], subarray);
            while(length < line.size() - start && index != -1) {
                record_index = r;
                record_start_index = index;
                record_length = length;
                length++;
                subarray = std::vector<size_t>(line.begin() + start, line.begin() + start + length);
                index = find_subarray(log_records_[r], subarray);
            }
            if(length == line.size() - start) {
                break;
            }
        }
        return {record_index, record_start_index, record_length};
    }

    void MoveToFront(const size_t& record_index) override {
        // Implement this method based on your business logic
    }
};