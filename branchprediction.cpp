#include <cstddef>
// C++20: [[likely]] and [[unlikely]]
bool process_tick(double price, double threshold) noexcept {
    if (price > threshold) [[likely]] {  // Hot path
        // Fast path - happens 99% of the time
        return true;
    } else [[unlikely]] {  // Cold path
        // Rare error condition
        handle_error();
        return false;
    }
}

// C++23: [[assume]]
void process_array(int* data, size_t size) {
    [[assume(size > 0)]];  // Tell compiler size is always positive
    [[assume(data != nullptr)]];  // Tell compiler data is never null
    
    for (size_t i = 0; i < size; ++i) {
        data[i] *= 2;  // Compiler can optimize better
    }
}
