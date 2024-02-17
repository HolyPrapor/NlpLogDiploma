//
// Created by zeliboba on 5/29/23.
//

#include <algorithm>
#include "greedy_record_storage.h"

class MoveToFrontStorage : public GreedyRecordStorage {
public:
    explicit MoveToFrontStorage(const size_t &window_size)
            : GreedyRecordStorage(window_size) {}

    void StoreRecord(const std::vector<size_t> &record) override {
        if (record.empty()) {
            return;
        }
        log_records_.insert(log_records_.begin(), record);
        if (log_records_.size() > window_size_) {
            log_records_.pop_back();
        }
    }

    void MoveToFront(const size_t &record_index) override {
        if (record_index >= log_records_.size()){
            return;
        }
        std::vector<size_t> record = log_records_[record_index];
        log_records_.erase(log_records_.begin() + record_index);
        log_records_.insert(log_records_.begin(), record);
    }
};
