//
// Created by zeliboba on 5/29/23.
//

#include "greedy_record_storage.h"

class SlidingWindowRecordStorage : GreedyRecordStorage {
public:
    explicit SlidingWindowRecordStorage(const size_t &window_size)
            : GreedyRecordStorage(window_size) {}

    void StoreRecord(const std::vector<size_t> &record) override {
        if (record.empty()) {
            return;
        }
        log_records_.insert(log_records_.begin(), record);
        if(log_records_.size() > window_size_) {
            log_records_.pop_back();
        }
    }
};