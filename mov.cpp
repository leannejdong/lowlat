#include <vector>
#include <memory>

struct MarketData {
    std::vector<double> prices;
    std::unique_ptr<int[]> large_buffer;
    
    MarketData(size_t size) 
        : prices(size), large_buffer(std::make_unique<int[]>(size)) {}
    
    // Move constructor - transfers ownership, no copies
    MarketData(MarketData&& other) noexcept 
        : prices(std::move(other.prices))
        , large_buffer(std::move(other.large_buffer)) {}
};

// BAD: Copies expensive data
std::vector<MarketData> create_data_slow() {
    std::vector<MarketData> result;
    result.push_back(MarketData(10000));  // Copy if no move
    return result;  // Copy return
}

// GOOD: Uses move semantics
std::vector<MarketData> create_data_fast() {
    std::vector<MarketData> result;
    result.push_back(MarketData(10000));  // Moved into vector
    return result;  // Return value optimization or move
}

// Explicit move in hot path
void process_order(MarketData data) {
    static std::vector<MarketData> queue;
    queue.push_back(std::move(data));  // Explicit move, no copy
}
