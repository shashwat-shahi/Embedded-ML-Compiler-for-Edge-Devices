#include "eml_compiler.h"
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    std::cout << "EML Basic Inference Example\n";
    std::cout << "===========================\n\n";

    // Create runtime executor
    eml::RuntimeExecutor executor;
    
    // Simulate loading a compiled model
    std::vector<uint8_t> model_data = {
        0x45, 0x4D, 0x4C, 0x43, // Magic: "EMLC"
        0x01, 0x00, 0x00, 0x00, // Version: 1
        // Add some dummy model data
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    
    // Load the model
    if (!executor.loadModel(model_data.data(), model_data.size())) {
        std::cerr << "Failed to load model" << std::endl;
        return 1;
    }
    
    std::cout << "Model loaded successfully!\n\n";
    
    // Set balanced power mode
    executor.setPowerMode(1); // Balanced mode
    
    // Prepare input data (simulated image data)
    const size_t input_size = 224 * 224 * 3; // RGB image 224x224
    const size_t output_size = 1000; // 1000 classes
    
    std::vector<float> input_data(input_size, 0.5f); // Dummy input
    std::vector<float> output_data(output_size, 0.0f);
    
    // Run inference multiple times to demonstrate performance
    const int num_iterations = 10;
    
    std::cout << "Running " << num_iterations << " inference iterations...\n";
    
    executor.startProfiling();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        if (!executor.execute(input_data.data(), output_data.data())) {
            std::cerr << "Inference failed at iteration " << i << std::endl;
            return 1;
        }
        
        std::cout << "Iteration " << (i + 1) << " completed" << std::endl;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    executor.stopProfiling();
    
    // Calculate total execution time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    float avg_time_us = duration.count() / static_cast<float>(num_iterations);
    
    std::cout << "\nPerformance Results:\n";
    std::cout << "====================\n";
    std::cout << "Total iterations: " << num_iterations << "\n";
    std::cout << "Total time: " << duration.count() << " μs\n";
    std::cout << "Average time per inference: " << avg_time_us << " μs\n";
    std::cout << "Throughput: " << (1000000.0f / avg_time_us) << " inferences/sec\n";
    
    // Get profiling results
    auto metrics = executor.getProfileResults();
    std::cout << "\nProfiling Results:\n";
    std::cout << "==================\n";
    std::cout << "Execution time: " << metrics.execution_time_us << " μs\n";
    std::cout << "Memory usage: " << metrics.memory_usage_bytes << " bytes\n";
    std::cout << "Power consumption: " << metrics.power_consumption_mw << " mW\n";
    std::cout << "Throughput: " << metrics.throughput_ops_per_sec << " ops/sec\n";
    
    // Display sample output values
    std::cout << "\nSample Output Values:\n";
    std::cout << "=====================\n";
    for (size_t i = 0; i < std::min(static_cast<size_t>(10), output_size); ++i) {
        std::cout << "Output[" << i << "] = " << output_data[i] << "\n";
    }
    
    std::cout << "\nExample completed successfully!\n";
    std::cout << "Demonstrated:\n";
    std::cout << "- Model loading and validation\n";
    std::cout << "- Optimized inference execution\n";
    std::cout << "- Performance profiling\n";
    std::cout << "- Memory-efficient operation\n";
    
    return 0;
}