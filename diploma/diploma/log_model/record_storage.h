#include <cstddef>
#include <vector>
#include <string>
#include <optional>

class RecordStorage {
private:
    size_t window_size_;

public:
    explicit RecordStorage(const size_t &window_size) : window_size_{window_size} {};

    virtual void StoreRecord(const std::vector<size_t> &record) = 0;

    virtual std::tuple<std::optional<size_t>, std::optional<size_t>, std::optional<size_t>>
    GetLink(const std::vector<size_t> &tokens, const size_t &index) = 0;

    virtual void MoveToFront(const size_t& record_index) {};

    virtual void Clear() = 0;

    virtual std::vector<std::vector<size_t>> GetRecords() = 0;
};