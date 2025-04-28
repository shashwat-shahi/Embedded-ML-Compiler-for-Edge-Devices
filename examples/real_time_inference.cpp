#include "eml_compiler.h"
#include "eml_zephyr.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>

int main() {
    std::cout << "EML Real-Time Inference Example\n";
    std::cout << "================================\n\n";

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
    
    if (!executor.loadModel(model_data.data(), model_data.size())) {
        std::cerr << "Failed to load model" << std::endl;
        return 1;
    }
    
    // Configure real-time constraints
    eml::zephyr::RealTimeManager::ConstraintConfig rt_config;
    rt_config.max_execution_time_us = 500;  // 500 μs max execution time
    rt_config.deadline_us = 1000;           // 1 ms deadline
    rt_config.priority = 10;                // High priority
    rt_config.preemptible = false;          // Non-preemptible for determinism
    
    eml::zephyr::RealTimeManager::setConstraints(rt_config);
    
    // Set high scheduling priority
    executor.setSchedulingPriority(10);
    
    // Prepare input data
    const size_t input_size = 224 * 224 * 3;
    const size_t output_size = 1000;
    std::vector<float> input_data(input_size, 0.5f);
    std::vector<float> output_data(output_size, 0.0f);
    
    std::cout << "Real-time constraints configured:\n";
    std::cout << "  Max execution time: " << rt_config.max_execution_time_us << " μs\n";
    std::cout << "  Deadline: " << rt_config.deadline_us << " μs\n";
    std::cout << "  Priority: " << rt_config.priority << "\n";
    std::cout << "  Preemptible: " << (rt_config.preemptible ? "yes" : "no") << "\n\n";
    
    // Test 1: Single inference with timing validation
    std::cout << "=== Test 1: Single Inference Timing ===\n";
    
    for (int test = 1; test <= 5; ++test) {
        uint64_t start_time = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        
        bool success = executor.execute(input_data.data(), output_data.data());
        
        uint64_t end_time = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        uint64_t execution_time = end_time - start_time;
        
        bool deadline_met = eml::zephyr::RealTimeManager::checkDeadlineMet();
        
        std::cout << "Test " << test << ": ";
        std::cout << execution_time << " μs ";
        std::cout << (success ? "SUCCESS" : "FAILED") << " ";
        std::cout << (deadline_met ? "DEADLINE_MET" : "DEADLINE_MISSED") << "\n";
        
        if (!deadline_met) {
            std::cout << "  WARNING: Real-time constraint violated!\n";
        }
        
        // Small delay between tests
        eml::zephyr::ZephyrRTOS::delayUs(2000); // 2ms delay
    }
    
    // Test 2: Periodic inference simulation
    std::cout << "\n=== Test 2: Periodic Inference (10ms period) ===\n";
    
    const uint32_t period_us = 10000; // 10ms period
    const int num_periods = 10;
    
    std::vector<uint64_t> execution_times;
    std::vector<bool> deadline_violations;
    
    for (int period = 0; period < num_periods; ++period) {
        uint64_t period_start = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        
        // Execute inference
        uint64_t inference_start = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        bool success = executor.execute(input_data.data(), output_data.data());
        uint64_t inference_end = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        
        uint64_t execution_time = inference_end - inference_start;
        bool deadline_met = eml::zephyr::RealTimeManager::checkDeadlineMet();
        
        execution_times.push_back(execution_time);
        deadline_violations.push_back(!deadline_met);
        
        std::cout << "Period " << (period + 1) << ": " 
                  << execution_time << " μs "
                  << (success ? "OK" : "FAIL") << " "
                  << (deadline_met ? "DEADLINE_MET" : "DEADLINE_MISSED") << "\n";
        
        // Wait for next period
        uint64_t period_end = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        uint64_t period_duration = period_end - period_start;
        
        if (period_duration < period_us) {
            eml::zephyr::ZephyrRTOS::delayUs(period_us - period_duration);
        }
    }
    
    // Analyze periodic execution results
    std::cout << "\nPeriodic Execution Analysis:\n";
    
    uint64_t min_time = *std::min_element(execution_times.begin(), execution_times.end());
    uint64_t max_time = *std::max_element(execution_times.begin(), execution_times.end());
    uint64_t avg_time = 0;
    for (auto time : execution_times) {
        avg_time += time;
    }
    avg_time /= execution_times.size();
    
    int violations = std::count(deadline_violations.begin(), deadline_violations.end(), true);
    
    std::cout << "  Minimum execution time: " << min_time << " μs\n";
    std::cout << "  Maximum execution time: " << max_time << " μs\n";
    std::cout << "  Average execution time: " << avg_time << " μs\n";
    std::cout << "  Jitter (max-min): " << (max_time - min_time) << " μs\n";
    std::cout << "  Deadline violations: " << violations << "/" << num_periods << "\n";
    std::cout << "  Success rate: " << (100.0 * (num_periods - violations) / num_periods) << "%\n";
    
    // Test 3: Deterministic memory allocation
    std::cout << "\n=== Test 3: Deterministic Memory Management ===\n";
    
    // Test memory allocation patterns
    std::vector<void*> allocations;
    const size_t allocation_sizes[] = {64, 128, 256, 512, 1024};
    const size_t num_allocations = sizeof(allocation_sizes) / sizeof(allocation_sizes[0]);
    
    std::cout << "Testing deterministic memory allocation:\n";
    
    for (size_t i = 0; i < num_allocations; ++i) {
        void* ptr = executor.allocateAligned(allocation_sizes[i], 16); // 16-byte alignment
        
        if (ptr) {
            allocations.push_back(ptr);
            std::cout << "  Allocated " << allocation_sizes[i] << " bytes at " << ptr << "\n";
        } else {
            std::cout << "  Failed to allocate " << allocation_sizes[i] << " bytes\n";
        }
    }
    
    // Test inference with allocated memory
    std::cout << "\nTesting inference with allocated memory:\n";
    
    for (int test = 0; test < 3; ++test) {
        uint64_t start_time = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        bool success = executor.execute(input_data.data(), output_data.data());
        uint64_t end_time = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        
        std::cout << "  Test " << (test + 1) << ": " 
                  << (end_time - start_time) << " μs "
                  << (success ? "SUCCESS" : "FAILED") << "\n";
    }
    
    // Deallocate memory
    for (void* ptr : allocations) {
        executor.deallocate(ptr);
    }
    
    // Test 4: Interrupt-safe execution
    std::cout << "\n=== Test 4: Interrupt-Safe Execution ===\n";
    
    std::cout << "Testing inference with interrupt management:\n";
    
    for (int test = 0; test < 3; ++test) {
        // Disable interrupts for critical section
        eml::zephyr::ZephyrRTOS::disableInterrupts();
        
        uint64_t start_time = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        bool success = executor.execute(input_data.data(), output_data.data());
        uint64_t end_time = eml::zephyr::ZephyrRTOS::getCurrentTimeUs();
        
        // Re-enable interrupts
        eml::zephyr::ZephyrRTOS::enableInterrupts();
        
        std::cout << "  Critical section " << (test + 1) << ": " 
                  << (end_time - start_time) << " μs "
                  << (success ? "SUCCESS" : "FAILED") << "\n";
        
        // Small delay to allow interrupt handling
        eml::zephyr::ZephyrRTOS::delayUs(1000);
    }
    
    std::cout << "\nReal-time inference example completed!\n";
    std::cout << "Demonstrated:\n";
    std::cout << "- Deterministic execution timing\n";
    std::cout << "- Real-time constraint validation\n";
    std::cout << "- Periodic inference scheduling\n";
    std::cout << "- Deterministic memory management\n";
    std::cout << "- Interrupt-safe execution\n";
    std::cout << "- Jitter analysis and deadline monitoring\n";
    
    return 0;
}