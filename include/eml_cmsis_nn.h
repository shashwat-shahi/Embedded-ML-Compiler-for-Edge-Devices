#ifndef EML_CMSIS_NN_H
#define EML_CMSIS_NN_H

#include <cstdint>

namespace eml {
namespace cmsis {

// CMSIS-NN optimized operators
class CmsisNNOps {
public:
    // Convolution operations
    static void conv2d_s8(
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
    );

    // Fully connected operations
    static void fully_connected_s8(
        const int8_t* input,
        const int8_t* weights,
        const int32_t* bias,
        int8_t* output,
        const uint16_t input_size,
        const uint16_t output_size
    );

    // Pooling operations
    static void max_pool_s8(
        const int8_t* input,
        int8_t* output,
        const uint16_t input_h,
        const uint16_t input_w,
        const uint16_t input_ch,
        const uint16_t pool_h,
        const uint16_t pool_w,
        const uint16_t stride_h,
        const uint16_t stride_w
    );

    // Activation functions
    static void relu_s8(int8_t* data, const uint32_t size);
    static void sigmoid_s8(int8_t* data, const uint32_t size);
    static void tanh_s8(int8_t* data, const uint32_t size);

    // Quantization utilities
    static int8_t quantize_float_to_int8(float value, float scale, int8_t zero_point);
    static float dequantize_int8_to_float(int8_t value, float scale, int8_t zero_point);
};

} // namespace cmsis
} // namespace eml

#endif // EML_CMSIS_NN_H