#include <atomic>
#include <vector>
#include <cstddef>
#include <new> // for hardware_destructive_interference_size

template<typename T>
class AudioRingBuffer {
public:
    explicit AudioRingBuffer(size_t capacity) 
        : buffer_(capacity), capacity_mask_(capacity - 1) {
        // Ensure capacity is a power of two for the bitwise mask to work
        if ((capacity & capacity_mask_) != 0 || capacity == 0) {
            throw std::invalid_argument("Capacity must be a power of 2");
        }
    }

    // Producer Side (e.g., UI thread or MIDI input)
    bool push(const T& item) {
        const auto currentHead = head_.load(std::memory_order_relaxed);
        const auto nextHead = (currentHead + 1) & capacity_mask_;

        if (nextHead == tail_.load(std::memory_order_acquire)) {
            return false; // Buffer full
        }

        buffer_[currentHead] = item;
        head_.store(nextHead, std::memory_order_release);
        return true;
    }

    // Consumer Side (e.g., Audio Render Thread)
    bool pop(T& item) {
        const auto currentTail = tail_.load(std::memory_order_relaxed);

        if (currentTail == head_.load(std::memory_order_acquire)) {
            return false; // Buffer empty
        }

        item = buffer_[currentTail];
        tail_.store((currentTail + 1) & capacity_mask_, std::memory_order_release);
        return true;
    }

private:
    std::vector<T> buffer_;
    const size_t capacity_mask_;

    // Align to prevent False Sharing (Cache Line Bouncing)
    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
};
