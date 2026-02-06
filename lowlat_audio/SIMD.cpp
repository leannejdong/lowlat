#include <immintrin.h>  // SSE/AVX intrinsics
#include <cmath>
#include <array>

// Vectorized gain application (4x faster than scalar)
class AudioProcessor {
public:
    // Scalar version (baseline)
    static void apply_gain_scalar(float* buffer, size_t samples, float gain) noexcept {
        for (size_t i = 0; i < samples; ++i) {
            buffer[i] *= gain;
        }
    }
    
    // SSE version (4 samples at a time)
    static void apply_gain_sse(float* buffer, size_t samples, float gain) noexcept {
        const __m128 gain_vec = _mm_set1_ps(gain);
        const size_t simd_end = samples & ~3;  // Round down to multiple of 4
        
        for (size_t i = 0; i < simd_end; i += 4) {
            __m128 data = _mm_loadu_ps(&buffer[i]);
            data = _mm_mul_ps(data, gain_vec);
            _mm_storeu_ps(&buffer[i], data);
        }
        
        // Handle remaining samples
        for (size_t i = simd_end; i < samples; ++i) {
            buffer[i] *= gain;
        }
    }
    
    // AVX version (8 samples at a time)
    static void apply_gain_avx(float* buffer, size_t samples, float gain) noexcept {
        const __m256 gain_vec = _mm256_set1_ps(gain);
        const size_t simd_end = samples & ~7;  // Round down to multiple of 8
        
        for (size_t i = 0; i < simd_end; i += 8) {
            __m256 data = _mm256_loadu_ps(&buffer[i]);
            data = _mm256_mul_ps(data, gain_vec);
            _mm256_storeu_ps(&buffer[i], data);
        }
        
        // Handle remaining samples
        for (size_t i = simd_end; i < samples; ++i) {
            buffer[i] *= gain;
        }
    }
};

// SIMD stereo mix (interleaved L/R channels)
void mix_stereo_sse(float* dest, const float* src1, const float* src2, 
                    size_t frames) noexcept {
    const size_t samples = frames * 2;  // L/R interleaved
    const size_t simd_end = samples & ~3;
    
    for (size_t i = 0; i < simd_end; i += 4) {
        __m128 a = _mm_loadu_ps(&src1[i]);
        __m128 b = _mm_loadu_ps(&src2[i]);
        __m128 result = _mm_add_ps(a, b);
        _mm_storeu_ps(&dest[i], result);
    }
    
    for (size_t i = simd_end; i < samples; ++i) {
        dest[i] = src1[i] + src2[i];
    }
}
