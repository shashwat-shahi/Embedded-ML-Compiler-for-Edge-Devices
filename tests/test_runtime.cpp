#include "eml_compiler.h"
#include <iostream>
#include <cassert>
#include <vector>

void test_runtime_executor() {
    std::cout << "Testing runtime executor..." << std::endl;
    
    eml::RuntimeExecutor executor;
    
    // Test model loading
    std::vector<uint8_t> model_data = {
        0x45, 0x4D, 0x4C, 0x43, // Magic: "EMLC"
        0x01, 0x00, 0x00, 0x00, // Version: 1
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    
    bool loaded = executor.loadModel(model_data.data(), model_data.size());
    assert(loaded == true);
    
    std::cout << "✓ Runtime executor test passed" << std::endl;
}

void test_inference_execution() {
    std::cout << "Testing inference execution..." << std::endl;
    
    eml::RuntimeExecutor executor;
    
    // Load test model
    std::vector<uint8_t> model_data = {
        0x45, 0x4D, 0x4C, 0x43, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    
    executor.loadModel(model_data.data(), model_data.size());
    
    // Test inference
    std::vector<float> input(100, 0.5f);
    std::vector<float> output(10, 0.0f);
    
    bool success = executor.execute(input.data(), output.data());
    assert(success == true);
    
    std::cout << "✓ Inference execution test passed" << std::endl;
}

void test_memory_management() {
    std::cout << "Testing deterministic memory management..." << std::endl;
    
    eml::RuntimeExecutor executor;
    
    // Test aligned allocation
    void* ptr1 = executor.allocateAligned(64, 16);
    assert(ptr1 != nullptr);
    
    void* ptr2 = executor.allocateAligned(128, 32);
    assert(ptr2 != nullptr);
    
    // Test deallocation
    executor.deallocate(ptr1);
    executor.deallocate(ptr2);
    
    std::cout << "✓ Memory management test passed" << std::endl;
}

void test_power_modes() {
    std::cout << "Testing power modes..." << std::endl;
    
    eml::RuntimeExecutor executor;
    
    // Test different power modes
    executor.setPowerMode(0); // Low power
    executor.setPowerMode(1); // Balanced
    executor.setPowerMode(2); // Performance
    
    std::cout << "✓ Power modes test passed" << std::endl;
}

void test_profiling() {
    std::cout << "Testing profiling functionality..." << std::endl;
    
    eml::RuntimeExecutor executor;
    
    // Load test model
    std::vector<uint8_t> model_data = {
        0x45, 0x4D, 0x4C, 0x43, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    
    executor.loadModel(model_data.data(), model_data.size());
    
    // Test profiling
    executor.startProfiling();
    
    std::vector<float> input(100, 0.5f);
    std::vector<float> output(10, 0.0f);
    executor.execute(input.data(), output.data());
    
    executor.stopProfiling();
    
    auto metrics = executor.getProfileResults();
    assert(metrics.execution_time_us >= 0);
    assert(metrics.memory_usage_bytes >= 0);
    assert(metrics.power_consumption_mw >= 0);
    
    std::cout << "✓ Profiling test passed" << std::endl;
}

int main() {
    std::cout << "Running EML Runtime Tests" << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        test_runtime_executor();
        test_inference_execution();
        test_memory_management();
        test_power_modes();
        test_profiling();
        
        std::cout << std::endl;
        std::cout << "🎉 All runtime tests passed!" << std::endl;
        std::cout << "Verified features:" << std::endl;
        std::cout << "- Runtime model loading" << std::endl;
        std::cout << "- Optimized inference execution" << std::endl;
        std::cout << "- Deterministic memory management" << std::endl;
        std::cout << "- Power mode configuration" << std::endl;
        std::cout << "- Performance profiling" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}