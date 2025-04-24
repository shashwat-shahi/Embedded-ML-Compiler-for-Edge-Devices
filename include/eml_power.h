#ifndef EML_POWER_H
#define EML_POWER_H

#include <cstdint>

namespace eml {
namespace power {

// Power profiling and management
class PowerProfiler {
public:
    struct PowerMetrics {
        uint32_t total_energy_uj;    // Total energy in microjoules
        uint32_t average_power_mw;   // Average power in milliwatts
        uint32_t peak_power_mw;      // Peak power in milliwatts
        uint32_t execution_time_us;  // Execution time in microseconds
        float efficiency_ops_per_mj; // Operations per millijoule
    };
    
    static void startProfiling();
    static void stopProfiling();
    static PowerMetrics getMetrics();
    static void resetCounters();
    
    // Power optimization strategies
    static void enableDynamicVoltageScaling(bool enable);
    static void setClockGating(uint32_t unused_peripherals_mask);
    static void configurePowerIslands(uint32_t island_mask);
    
private:
    static uint64_t start_time_;
    static uint32_t start_energy_counter_;
    static PowerMetrics current_metrics_;
};

// Power-aware execution policies
class PowerAwareExecutor {
public:
    enum class PowerMode {
        LOW_POWER,      // Minimize power consumption
        BALANCED,       // Balance power and performance
        PERFORMANCE     // Maximize performance
    };
    
    static void setPowerMode(PowerMode mode);
    static PowerMode getCurrentPowerMode();
    
    // Adaptive frequency scaling
    static void enableAdaptiveFrequencyScaling(bool enable);
    static void setFrequencyRange(uint32_t min_mhz, uint32_t max_mhz);
    
    // Workload-based optimization
    static void optimizeForWorkload(const char* workload_profile);
    
private:
    static PowerMode current_mode_;
    static bool adaptive_scaling_enabled_;
    static uint32_t min_frequency_;
    static uint32_t max_frequency_;
};

} // namespace power
} // namespace eml

#endif // EML_POWER_H