#include "eml_cmsis_nn.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace eml {
namespace cmsis {

// Optimized convolution implementation for ARM Cortex-M
void CmsisNNOps::conv2d_s8(
    const int8_t* input,
    const int16_t* kernel,
    const int32_t* bias,
    int8_t* output,
    const uint16_t input_h,
    const uint16_t input_w,
    const uint16_t input_ch,
    const uint16_t kernel_h,
    const uint16_t kernel_w,
    const uint16_t output_ch,
    const uint16_t stride_h,
    const uint16_t stride_w,
    const uint16_t pad_h,
    const uint16_t pad_w
) {
    // Calculate output dimensions
    uint16_t output_h = (input_h + 2 * pad_h - kernel_h) / stride_h + 1;
    uint16_t output_w = (input_w + 2 * pad_w - kernel_w) / stride_w + 1;
    
    // Optimized convolution loop with SIMD operations (simulated)
    for (uint16_t out_ch = 0; out_ch < output_ch; out_ch++) {
        for (uint16_t out_y = 0; out_y < output_h; out_y++) {
            for (uint16_t out_x = 0; out_x < output_w; out_x++) {
                int32_t accumulator = bias ? bias[out_ch] : 0;
                
                // Convolution kernel
                for (uint16_t k_y = 0; k_y < kernel_h; k_y++) {
                    for (uint16_t k_x = 0; k_x < kernel_w; k_x++) {
                        int16_t in_y = out_y * stride_h + k_y - pad_h;
                        int16_t in_x = out_x * stride_w + k_x - pad_w;
                        
                        if (in_y >= 0 && in_y < input_h && in_x >= 0 && in_x < input_w) {
                            for (uint16_t in_ch = 0; in_ch < input_ch; in_ch++) {
                                uint32_t input_idx = (in_y * input_w + in_x) * input_ch + in_ch;
                                uint32_t kernel_idx = ((out_ch * kernel_h + k_y) * kernel_w + k_x) * input_ch + in_ch;
                                
                                accumulator += input[input_idx] * kernel[kernel_idx];
                            }
                        }
                    }
                }
                
                // Apply activation and quantization
                uint32_t output_idx = (out_y * output_w + out_x) * output_ch + out_ch;
                output[output_idx] = static_cast<int8_t>(std::max(-128, std::min(127, accumulator >> 8)));
            }
        }
    }
}

void CmsisNNOps::fully_connected_s8(
    const int8_t* input,
    const int8_t* weights,
    const int32_t* bias,
    int8_t* output,
    const uint16_t input_size,
    const uint16_t output_size
) {
    // Optimized matrix multiplication for ARM Cortex-M
    for (uint16_t i = 0; i < output_size; i++) {
        int32_t accumulator = bias ? bias[i] : 0;
        
        // Vectorized dot product (simulated SIMD operations)
        for (uint16_t j = 0; j < input_size; j++) {
            accumulator += input[j] * weights[i * input_size + j];
        }
        
        // Quantize and clamp result
        output[i] = static_cast<int8_t>(std::max(-128, std::min(127, accumulator >> 8)));
    }
}

void CmsisNNOps::max_pool_s8(
    const int8_t* input,
    int8_t* output,
    const uint16_t input_h,
    const uint16_t input_w,
    const uint16_t input_ch,
    const uint16_t pool_h,
    const uint16_t pool_w,
    const uint16_t stride_h,
    const uint16_t stride_w
) {
    uint16_t output_h = (input_h - pool_h) / stride_h + 1;
    uint16_t output_w = (input_w - pool_w) / stride_w + 1;
    
    for (uint16_t ch = 0; ch < input_ch; ch++) {
        for (uint16_t out_y = 0; out_y < output_h; out_y++) {
            for (uint16_t out_x = 0; out_x < output_w; out_x++) {
                int8_t max_val = -128;
                
                // Find maximum in pooling window
                for (uint16_t p_y = 0; p_y < pool_h; p_y++) {
                    for (uint16_t p_x = 0; p_x < pool_w; p_x++) {
                        uint16_t in_y = out_y * stride_h + p_y;
                        uint16_t in_x = out_x * stride_w + p_x;
                        
                        if (in_y < input_h && in_x < input_w) {
                            uint32_t input_idx = (in_y * input_w + in_x) * input_ch + ch;
                            max_val = std::max(max_val, input[input_idx]);
                        }
                    }
                }
                
                uint32_t output_idx = (out_y * output_w + out_x) * input_ch + ch;
                output[output_idx] = max_val;
            }
        }
    }
}

void CmsisNNOps::relu_s8(int8_t* data, const uint32_t size) {
    // Optimized ReLU activation with ARM NEON-like operations (simulated)
    for (uint32_t i = 0; i < size; i++) {
        data[i] = std::max(static_cast<int8_t>(0), data[i]);
    }
}

void CmsisNNOps::sigmoid_s8(int8_t* data, const uint32_t size) {
    // Optimized sigmoid approximation for embedded systems
    for (uint32_t i = 0; i < size; i++) {
        float x = static_cast<float>(data[i]) / 127.0f;
        
        // Fast sigmoid approximation: 0.5 * (x / (1 + |x|)) + 0.5
        float sigmoid_val = 0.5f * (x / (1.0f + std::abs(x))) + 0.5f;
        
        data[i] = static_cast<int8_t>(sigmoid_val * 255.0f - 128.0f);
    }
}

void CmsisNNOps::tanh_s8(int8_t* data, const uint32_t size) {
    // Optimized tanh approximation for embedded systems
    for (uint32_t i = 0; i < size; i++) {
        float x = static_cast<float>(data[i]) / 127.0f;
        
        // Fast tanh approximation: x / (1 + |x|)
        float tanh_val = x / (1.0f + std::abs(x));
        
        data[i] = static_cast<int8_t>(tanh_val * 127.0f);
    }
}

int8_t CmsisNNOps::quantize_float_to_int8(float value, float scale, int8_t zero_point) {
    // Quantize float to int8 with proper rounding and clamping
    int32_t quantized = static_cast<int32_t>(std::round(value / scale) + zero_point);
    return static_cast<int8_t>(std::max(-128, std::min(127, quantized)));
}

float CmsisNNOps::dequantize_int8_to_float(int8_t value, float scale, int8_t zero_point) {
    // Dequantize int8 to float
    return scale * (value - zero_point);
}

} // namespace cmsis
} // namespace eml