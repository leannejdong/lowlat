#include <atomic>
#include <array>
#include <cstring>

// Single Producer Single Consumer lock-free ring buffer
// Critical for real-time audio: producer (audio callback) never blocks
template<typename T, size_t Size>
class LockFreeRingBuffer {
    static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");
    
    alignas(64) std::atomic<size_t> write_pos{0};
    alignas(64) std::atomic<size_t> read_pos{0};
    alignas(64) std::array<T, Size> buffer;
    
    static constexpr size_t mask = Size - 1;
    
public:
    // Write samples (audio callback thread)
    size_t write(const T* data, size_t count) noexcept {
        const size_t w = write_pos.load(std::memory_order_relaxed);
        const size_t r = read_pos.load(std::memory_order_acquire);
        
        const size_t available = (r - w - 1) & mask;
        const size_t to_write = std::min(count, available);
        
        const size_t first_chunk = std::min(to_write, Size - (w & mask));
        std::memcpy(&buffer[w & mask], data, first_chunk * sizeof(T));
        
        if (to_write > first_chunk) {
            std::memcpy(&buffer[0], data + first_chunk, 
                       (to_write - first_chunk) * sizeof(T));
        }
        
        write_pos.store(w + to_write, std::memory_order_release);
        return to_write;
    }
    
    // Read samples (processing thread)
    size_t read(T* data, size_t count) noexcept {
        const size_t r = read_pos.load(std::memory_order_relaxed);
        const size_t w = write_pos.load(std::memory_order_acquire);
        
        const size_t available = (w - r) & mask;
        const size_t to_read = std::min(count, available);
        
        const size_t first_chunk = std::min(to_read, Size - (r & mask));
        std::memcpy(data, &buffer[r & mask], first_chunk * sizeof(T));
        
        if (to_read > first_chunk) {
            std::memcpy(data + first_chunk, &buffer[0], 
                       (to_read - first_chunk) * sizeof(T));
        }
        
        read_pos.store(r + to_read, std::memory_order_release);
        return to_read;
    }
    
    size_t available() const noexcept {
        const size_t w = write_pos.load(std::memory_order_acquire);
        const size_t r = read_pos.load(std::memory_order_acquire);
        return (w - r) & mask;
    }
};

// Usage in audio callback
LockFreeRingBuffer<float, 8192> audio_buffer;

void audio_callback(float* output, size_t frames) {
    audio_buffer.read(output, frames);
}
