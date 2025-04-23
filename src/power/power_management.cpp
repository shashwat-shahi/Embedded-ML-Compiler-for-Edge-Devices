#include "eml_power.h"
#include "eml_zephyr.h"
#include <iostream>
#include <algorithm>
#include <cstring>

namespace eml {
namespace power {

// Static variables for power profiler
uint64_t PowerProfiler::start_time_ = 0;
uint32_t PowerProfiler::start_energy_counter_ = 0;
PowerProfiler::PowerMetrics PowerProfiler::current_metrics_ = {0, 0, 0, 0, 0.0f};

// Static variables for power-aware executor
PowerAwareExecutor::PowerMode PowerAwareExecutor::current_mode_ = PowerMode::BALANCED;
bool PowerAwareExecutor::adaptive_scaling_enabled_ = false;
uint32_t PowerAwareExecutor::min_frequency_ = 16; // 16 MHz minimum
uint32_t PowerAwareExecutor::max_frequency_ = 84; // 84 MHz maximum

// Power profiler implementation
void PowerProfiler::startProfiling() {
    start_time_ = zephyr::ZephyrRTOS::getCurrentTimeUs();
    
    // In real implementation, read energy counter from hardware
    // start_energy_counter_ = read_energy_counter();
    start_energy_counter_ = 0;
    
    // Reset current metrics
    current_metrics_ = {0, 0, 0, 0, 0.0f};
    
    std::cout << "Power profiling started" << std::endl;
}

void PowerProfiler::stopProfiling() {
    uint64_t end_time = zephyr::ZephyrRTOS::getCurrentTimeUs();
    
    // In real implementation, read energy counter from hardware
    // uint32_t end_energy_counter = read_energy_counter();
    uint32_t end_energy_counter = start_energy_counter_ + 100; // Simulated energy consumption
    
    current_metrics_.execution_time_us = static_cast<uint32_t>(end_time - start_time_);
    current_metrics_.total_energy_uj = end_energy_counter - start_energy_counter_;
    
    if (current_metrics_.execution_time_us > 0) {
        current_metrics_.average_power_mw = 
            (current_metrics_.total_energy_uj * 1000) / current_metrics_.execution_time_us;
        current_metrics_.efficiency_ops_per_mj = 
            1000000.0f / current_metrics_.total_energy_uj; // Approximate ops per millijoule
    }
    
    std::cout << "Power profiling completed:" << std::endl;
    std::cout << "  Total energy: " << current_metrics_.total_energy_uj << " μJ" << std::endl;
    std::cout << "  Average power: " << current_metrics_.average_power_mw << " mW" << std::endl;
    std::cout << "  Execution time: " << current_metrics_.execution_time_us << " μs" << std::endl;
    std::cout << "  Efficiency: " << current_metrics_.efficiency_ops_per_mj << " ops/mJ" << std::endl;
}

PowerProfiler::PowerMetrics PowerProfiler::getMetrics() {
    return current_metrics_;
}

void PowerProfiler::resetCounters() {
    current_metrics_ = {0, 0, 0, 0, 0.0f};
    start_time_ = 0;
    start_energy_counter_ = 0;
    
    std::cout << "Power profiling counters reset" << std::endl;
}

void PowerProfiler::enableDynamicVoltageScaling(bool enable) {
    std::cout << "Dynamic voltage scaling " << (enable ? "enabled" : "disabled") << std::endl;
    
    // In real implementation:
    // Configure voltage regulator and clock management unit
    // Enable/disable DVFS (Dynamic Voltage and Frequency Scaling)
}

void PowerProfiler::setClockGating(uint32_t unused_peripherals_mask) {
    std::cout << "Clock gating configured for peripherals: 0x" 
              << std::hex << unused_peripherals_mask << std::dec << std::endl;
    
    // In real implementation:
    // Disable clocks to unused peripherals to save power
    // Configure RCC (Reset and Clock Control) registers
}

void PowerProfiler::configurePowerIslands(uint32_t island_mask) {
    std::cout << "Power islands configured: 0x" 
              << std::hex << island_mask << std::dec << std::endl;
    
    // In real implementation:
    // Configure power domains and isolation cells
    // Enable/disable power islands for unused subsystems
}

// Power-aware executor implementation
void PowerAwareExecutor::setPowerMode(PowerMode mode) {
    current_mode_ = mode;
    
    std::string mode_str;
    uint32_t target_freq;
    
    switch (mode) {
        case PowerMode::LOW_POWER:
            mode_str = "Low Power";
            target_freq = min_frequency_;
            break;
        case PowerMode::BALANCED:
            mode_str = "Balanced";
            target_freq = (min_frequency_ + max_frequency_) / 2;
            break;
        case PowerMode::PERFORMANCE:
            mode_str = "Performance";
            target_freq = max_frequency_;
            break;
        default:
            mode_str = "Unknown";
            target_freq = max_frequency_;
            break;
    }
    
    std::cout << "Power mode set to: " << mode_str << std::endl;
    std::cout << "Target frequency: " << target_freq << " MHz" << std::endl;
    
    // Configure hardware based on power mode
    zephyr::ZephyrRTOS::configureClockFrequency(target_freq);
    
    // Apply power optimizations
    switch (mode) {
        case PowerMode::LOW_POWER:
            PowerProfiler::enableDynamicVoltageScaling(true);
            PowerProfiler::setClockGating(0xFFFF0000); // Gate many peripherals
            break;
        case PowerMode::BALANCED:
            PowerProfiler::enableDynamicVoltageScaling(true);
            PowerProfiler::setClockGating(0xFF000000); // Gate some peripherals
            break;
        case PowerMode::PERFORMANCE:
            PowerProfiler::enableDynamicVoltageScaling(false);
            PowerProfiler::setClockGating(0x00000000); // Keep all peripherals active
            break;
    }
}

PowerAwareExecutor::PowerMode PowerAwareExecutor::getCurrentPowerMode() {
    return current_mode_;
}

void PowerAwareExecutor::enableAdaptiveFrequencyScaling(bool enable) {
    adaptive_scaling_enabled_ = enable;
    
    std::cout << "Adaptive frequency scaling " << (enable ? "enabled" : "disabled") << std::endl;
    
    if (enable) {
        std::cout << "  Frequency range: " << min_frequency_ << " - " << max_frequency_ << " MHz" << std::endl;
    }
    
    // In real implementation:
    // Configure hardware timers and interrupt handlers for adaptive scaling
    // Monitor workload and adjust frequency dynamically
}

void PowerAwareExecutor::setFrequencyRange(uint32_t min_mhz, uint32_t max_mhz) {
    min_frequency_ = std::max(1u, min_mhz);  // Minimum 1 MHz
    max_frequency_ = std::min(168u, max_mhz); // Maximum 168 MHz for Cortex-M4
    
    std::cout << "Frequency range set to: " << min_frequency_ << " - " << max_frequency_ << " MHz" << std::endl;
    
    // Ensure current frequency is within new range
    uint32_t current_freq = zephyr::ZephyrRTOS::getCurrentClockFrequency();
    if (current_freq < min_frequency_ || current_freq > max_frequency_) {
        uint32_t new_freq = std::max(min_frequency_, std::min(max_frequency_, current_freq));
        zephyr::ZephyrRTOS::configureClockFrequency(new_freq);
    }
}

void PowerAwareExecutor::optimizeForWorkload(const char* workload_profile) {
    std::cout << "Optimizing for workload profile: " << workload_profile << std::endl;
    
    // Configure power settings based on workload characteristics
    if (strcmp(workload_profile, "inference_continuous") == 0) {
        setPowerMode(PowerMode::BALANCED);
        enableAdaptiveFrequencyScaling(true);
        setFrequencyRange(32, 84);
    } else if (strcmp(workload_profile, "inference_burst") == 0) {
        setPowerMode(PowerMode::PERFORMANCE);
        enableAdaptiveFrequencyScaling(false);
        setFrequencyRange(84, 84);
    } else if (strcmp(workload_profile, "sensor_monitoring") == 0) {
        setPowerMode(PowerMode::LOW_POWER);
        enableAdaptiveFrequencyScaling(true);
        setFrequencyRange(16, 48);
    } else {
        std::cout << "Unknown workload profile, using balanced settings" << std::endl;
        setPowerMode(PowerMode::BALANCED);
    }
}

} // namespace power
} // namespace eml