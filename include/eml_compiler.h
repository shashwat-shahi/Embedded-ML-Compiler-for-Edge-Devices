#ifndef EML_COMPILER_H
#define EML_COMPILER_H

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

namespace eml {

// Forward declarations
class Model;
class Tensor;
class Operator;

// Compiler configuration
struct CompilerConfig {
    bool enable_power_optimization = true;
    bool enable_memory_optimization = true;
    bool enable_real_time_constraints = true;
    uint32_t target_frequency_mhz = 84;  // Cortex-M4 typical frequency
    uint32_t available_memory_kb = 256;   // Typical Cortex-M4 SRAM
    std::string target_platform = "cortex-m4";
};

// Performance metrics
struct PerformanceMetrics {
    uint32_t execution_time_us;
    uint32_t memory_usage_bytes;
    uint32_t power_consumption_mw;
    float throughput_ops_per_sec;
};

// Main compiler class
class Compiler {
public:
    explicit Compiler(const CompilerConfig& config);
    ~Compiler();

    // Compile model to optimized binary
    bool compile(const std::string& model_path, const std::string& output_path);
    
    // Generate C++ code for Zephyr RTOS
    bool generateZephyrCode(const std::string& model_path, const std::string& output_dir);
    
    // Optimize for power consumption
    void enablePowerOptimization(bool enable);
    
    // Set real-time constraints
    void setRealTimeConstraints(uint32_t max_latency_us, uint32_t deadline_us);
    
    // Get performance metrics
    PerformanceMetrics getPerformanceMetrics() const;

private:
    std::unique_ptr<class CompilerImpl> impl_;
};

// Runtime executor
class RuntimeExecutor {
public:
    RuntimeExecutor();
    ~RuntimeExecutor();

    // Load compiled model
    bool loadModel(const uint8_t* model_data, size_t model_size);
    
    // Execute inference
    bool execute(const float* input_data, float* output_data);
    
    // Power management
    void setPowerMode(uint8_t mode);  // 0=low power, 1=balanced, 2=performance
    
    // Memory management
    void* allocateAligned(size_t size, size_t alignment);
    void deallocate(void* ptr);
    
    // Real-time scheduling
    void setSchedulingPriority(uint8_t priority);
    
    // Profiling
    void startProfiling();
    void stopProfiling();
    PerformanceMetrics getProfileResults() const;

private:
    std::unique_ptr<class RuntimeImpl> impl_;
};

} // namespace eml

#endif // EML_COMPILER_H