#include <immintrin.h>
#include <algorithm>

class WaveShaper {
public:
    // Soft clipping (tanh approximation) - SIMD version
    static void soft_clip_avx(float* buffer, size_t samples, float drive = 1.0f) noexcept {
        const __m256 drive_vec = _mm256_set1_ps(drive);
        const __m256 one = _mm256_set1_ps(1.0f);
        const __m256 neg_one = _mm256_set1_ps(-1.0f);
        
        const size_t simd_end = samples & ~7;
        
        for (size_t i = 0; i < simd_end; i += 8) {
            __m256 x = _mm256_loadu_ps(&buffer[i]);
            
            // x = x * drive
            x = _mm256_mul_ps(x, drive_vec);
            
            // Fast tanh approximation: tanh(x) ≈ x / (1 + |x|)
            __m256 abs_x = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), x);
            __m256 denom = _mm256_add_ps(one, abs_x);
            __m256 result = _mm256_div_ps(x, denom);
            
            // Clamp to [-1, 1]
            result = _mm256_max_ps(result, neg_one);
            result = _mm256_min_ps(result, one);
            
            _mm256_storeu_ps(&buffer[i], result);
        }
        
        // Handle remaining samples
        for (size_t i = simd_end; i < samples; ++i) {
            float x = buffer[i] * drive;
            buffer[i] = std::clamp(x / (1.0f + std::abs(x)), -1.0f, 1.0f);
        }
    }
    
    // Hard clipping
    static void hard_clip_avx(float* buffer, size_t samples, 
                             float threshold = 1.0f) noexcept {
        const __m256 thresh_pos = _mm256_set1_ps(threshold);
        const __m256 thresh_neg = _mm256_set1_ps(-threshold);
        const size_t simd_end = samples & ~7;
        
        for (size_t i = 0; i < simd_end; i += 8) {
            __m256 x = _mm256_loadu_ps(&buffer[i]);
            x = _mm256_max_ps(x, thresh_neg);
            x = _mm256_min_ps(x, thresh_pos);
            _mm256_storeu_ps(&buffer[i], x);
        }
        
        for (size_t i = simd_end; i < samples; ++i) {
            buffer[i] = std::clamp(buffer[i], -threshold, threshold);
        }
    }
};
