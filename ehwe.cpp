// C++23: std::expected - zero-overhead error handling
#include <expected>
#include <string>

enum class OrderError {
    InvalidPrice,
    InsufficientFunds,
    MarketClosed
};

// BAD: Exceptions in hot path
double calculate_pnl_slow(double price) {
    if (price < 0) 
        throw std::runtime_error("Invalid price");  // Slow!
    return price * 100;
}

// GOOD: std::expected - no exceptions
std::expected<double, OrderError> calculate_pnl_fast(double price) noexcept {
    if (price < 0) 
        return std::unexpected(OrderError::InvalidPrice);
    return price * 100;
}

// Usage
void process_order() {
    auto result = calculate_pnl_fast(99.5);
    if (result) {
        // Success path - inline, no overhead
        double pnl = *result;
    } else {
        // Error path - also inline
        switch (result.error()) {
            case OrderError::InvalidPrice: /* handle */ break;
            // ...
        }
    }
}
