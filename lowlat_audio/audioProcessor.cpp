#include <array>
#include <memory>

// Complete audio processor using low-latency techniques
class RealtimeAudioProcessor {
    static constexpr size_t BUFFER_SIZE = 512;
    static constexpr size_t SAMPLE_RATE = 48000;
    
    // Lock-free communication
    LockFreeRingBuffer<float, 16384> input_buffer;
    LockFreeRingBuffer<float, 16384> output_buffer;
    
    // DSP components (allocated once, never in audio callback)
    BiquadFilter lowpass_filter;
    DelayLine delay{SAMPLE_RATE};  // 1 second max delay
    
    // Parameters (atomic for thread-safe updates)
    alignas(64) std::atomic<float> gain{1.0f};
    alignas(64) std::atomic<float> cutoff_freq{1000.0f};
    
    // Working buffer (stack allocated)
    alignas(32) std::array<float, BUFFER_SIZE> working_buffer;
    
public:
    RealtimeAudioProcessor() {
        lowpass_filter.set_lowpass(SAMPLE_RATE, 1000.0f);
    }
    
    // Audio callback - MUST be real-time safe!
    // NO: allocations, locks, system calls, unbounded loops
    void process_audio(float* output, const float* input, size_t frames) noexcept {
        // Read parameters (lock-free)
        const float current_gain = gain.load(std::memory_order_relaxed);
        const float current_cutoff = cutoff_freq.load(std::memory_order_relaxed);
        
        // Update filter if needed (could use flag to avoid every callback)
        if constexpr (true) {  // Simplified - add dirty flag in production
            lowpass_filter.set_lowpass(SAMPLE_RATE, current_cutoff);
        }
        
        // Process in place or use working buffer
        std::copy(input, input + frames, working_buffer.begin());
        
        // Apply gain (SIMD)
        AudioProcessor::apply_gain_avx(working_buffer.data(), frames, current_gain);
        
        // Apply filter
        lowpass_filter.process_buffer_sse(working_buffer.data(), frames);
        
        // Copy to output
        std::copy(working_buffer.begin(), working_buffer.begin() + frames, output);
    }
    
    // UI thread - safe parameter updates
    void set_gain(float new_gain) noexcept {
        gain.store(std::clamp(new_gain, 0.0f, 2.0f), std::memory_order_relaxed);
    }
    
    void set_cutoff(float new_cutoff) noexcept {
        cutoff_freq.store(std::clamp(new_cutoff, 20.0f, 20000.0f), 
                         std::memory_order_relaxed);
    }
};
