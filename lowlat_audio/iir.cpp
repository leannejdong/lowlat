#include <array>
#include <cmath>

// Biquad filter - fundamental building block for EQ, filters
class BiquadFilter {
    // Coefficients
    float b0, b1, b2;  // Feedforward
    float a1, a2;      // Feedback
    
    // State (previous samples)
    alignas(16) float x1{0}, x2{0};  // Input history
    alignas(16) float y1{0}, y2{0};  // Output history
    
public:
    // Low-pass filter design
    void set_lowpass(float sample_rate, float cutoff_hz, float q = 0.707f) noexcept {
        const float omega = 2.0f * M_PI * cutoff_hz / sample_rate;
        const float sin_omega = std::sin(omega);
        const float cos_omega = std::cos(omega);
        const float alpha = sin_omega / (2.0f * q);
        
        const float a0 = 1.0f + alpha;
        b0 = ((1.0f - cos_omega) / 2.0f) / a0;
        b1 = (1.0f - cos_omega) / a0;
        b2 = b0;
        a1 = (-2.0f * cos_omega) / a0;
        a2 = (1.0f - alpha) / a0;
    }
    
    // High-pass filter design
    void set_highpass(float sample_rate, float cutoff_hz, float q = 0.707f) noexcept {
        const float omega = 2.0f * M_PI * cutoff_hz / sample_rate;
        const float sin_omega = std::sin(omega);
        const float cos_omega = std::cos(omega);
        const float alpha = sin_omega / (2.0f * q);
        
        const float a0 = 1.0f + alpha;
        b0 = ((1.0f + cos_omega) / 2.0f) / a0;
        b1 = -(1.0f + cos_omega) / a0;
        b2 = b0;
        a1 = (-2.0f * cos_omega) / a0;
        a2 = (1.0f - alpha) / a0;
    }
    
    // Process single sample (scalar)
    [[gnu::hot]] float process(float input) noexcept {
        const float output = b0 * input + b1 * x1 + b2 * x2
                           - a1 * y1 - a2 * y2;
        
        // Update state
        x2 = x1;
        x1 = input;
        y2 = y1;
        y1 = output;
        
        return output;
    }
    
    // Process buffer
    void process_buffer(float* buffer, size_t samples) noexcept {
        for (size_t i = 0; i < samples; ++i) {
            buffer[i] = process(buffer[i]);
        }
    }
    
    // SIMD version (4 samples at a time with transposed Direct Form II)
    void process_buffer_sse(float* buffer, size_t samples) noexcept {
        __m128 state1 = _mm_setzero_ps();
        __m128 state2 = _mm_setzero_ps();
        
        const __m128 vb0 = _mm_set1_ps(b0);
        const __m128 vb1 = _mm_set1_ps(b1);
        const __m128 vb2 = _mm_set1_ps(b2);
        const __m128 va1 = _mm_set1_ps(-a1);
        const __m128 va2 = _mm_set1_ps(-a2);
        
        const size_t simd_end = samples & ~3;
        
        for (size_t i = 0; i < simd_end; i += 4) {
            __m128 input = _mm_loadu_ps(&buffer[i]);
            
            __m128 output = _mm_mul_ps(input, vb0);
            output = _mm_add_ps(output, state1);
            
            state1 = _mm_mul_ps(input, vb1);
            state1 = _mm_add_ps(state1, state2);
            state1 = _mm_add_ps(state1, _mm_mul_ps(output, va1));
            
            state2 = _mm_mul_ps(input, vb2);
            state2 = _mm_add_ps(state2, _mm_mul_ps(output, va2));
            
            _mm_storeu_ps(&buffer[i], output);
        }
        
        // Process remaining samples
        for (size_t i = simd_end; i < samples; ++i) {
            buffer[i] = process(buffer[i]);
        }
    }
    
    void reset() noexcept {
        x1 = x2 = y1 = y2 = 0.0f;
    }
};
