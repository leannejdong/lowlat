
// C++17: std::string_view - zero-copy string operations
#include <string_view>
#include <string>

// BAD: Creates copies
void process_legacy(const std::string& data) {
    std::string prefix = data.substr(0, 5);  // Copy!
    std::string suffix = data.substr(5);     // Another copy!
}

// GOOD: Zero-copy with string_view
void process_fast(std::string_view data) {
    std::string_view prefix = data.substr(0, 5);  // No copy!
    std::string_view suffix = data.substr(5);     // No copy!
}

// C++20: std::span - lightweight array views
#include <span>
#include <vector>

// BAD: Passing container or raw pointer + size
void process_data(const std::vector<int>& data) { /* ... */ }
void process_raw(const int* data, size_t size) { /* ... */ }

// GOOD: std::span provides bounds safety + zero overhead
void process_span(std::span<const int> data) {
    for (int value : data) {  // Safe iteration
        // Process value
    }
}

// Works with any contiguous container
std::vector<int> vec = {1, 2, 3, 4, 5};
std::array<int, 5> arr = {1, 2, 3, 4, 5};
process_span(vec);  // Works
process_span(arr);  // Works
process_span(std::span{vec}.subspan(1, 3));  // Slice without copy
