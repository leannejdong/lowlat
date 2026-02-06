#include <complex>
#include <vector>
#include <span>
#include <memory>

// Overlap-Add convolution for reverb/IR processing
class ConvolutionProcessor {
    size_t block_size;
    size_t fft_size;
    size_t ir_length;
    
    // Pre-computed FFT of impulse response
    std::vector<std::complex<float>> ir_spectrum;
    
    // Overlap buffers
    std::vector<float> overlap_buffer;
    std::vector<float> input_buffer;
    std::vector<std::complex<float>> fft_buffer;
    
public:
    ConvolutionProcessor(std::span<const float> impulse_response, size_t block_size)
        : block_size(block_size)
        , ir_length(impulse_response.size())
        , fft_size(next_power_of_2(block_size + ir_length - 1))
        , overlap_buffer(fft_size, 0.0f)
        , input_buffer(fft_size, 0.0f)
        , fft_buffer(fft_size / 2 + 1)
        , ir_spectrum(fft_size / 2 + 1)
    {
        // Precompute IR spectrum (done once, not in real-time)
        std::vector<float> padded_ir(fft_size, 0.0f);
        std::copy(impulse_response.begin(), impulse_response.end(), 
                 padded_ir.begin());
        
        // FFT of impulse response (simplified - use FFTW/pffft in practice)
        compute_fft(padded_ir, ir_spectrum);
    }
    
    // Process audio block (real-time safe)
    void process(std::span<float> output, std::span<const float> input) noexcept {
        // Copy input to padded buffer
        std::copy(input.begin(), input.end(), input_buffer.begin());
        std::fill(input_buffer.begin() + block_size, input_buffer.end(), 0.0f);
        
        // FFT of input
        compute_fft(input_buffer, fft_buffer);
        
        // Multiply in frequency domain (convolution)
        for (size_t i = 0; i < fft_buffer.size(); ++i) {
            fft_buffer[i] *= ir_spectrum[i];
        }
        
        // IFFT back to time domain
        std::vector<float> result(fft_size);
        compute_ifft(fft_buffer, result);
        
        // Overlap-add
        for (size_t i = 0; i < block_size; ++i) {
            output[i] = result[i] + overlap_buffer[i];
        }
        
        // Save overlap for next block
        std::copy(result.begin() + block_size, result.end(), 
                 overlap_buffer.begin());
    }
    
private:
    static size_t next_power_of_2(size_t n) noexcept {
        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        return n + 1;
    }
    
    // Placeholder - use FFTW, Intel IPP, or pffft in production
    void compute_fft(const std::vector<float>& input, 
                    std::vector<std::complex<float>>& output) noexcept {
        // Use optimized FFT library here
    }
    
    void compute_ifft(const std::vector<std::complex<float>>& input,
                     std::vector<float>& output) noexcept {
        // Use optimized IFFT library here
    }
};
