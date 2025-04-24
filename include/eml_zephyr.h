#ifndef EML_ZEPHYR_H
#define EML_ZEPHYR_H

#include <cstdint>
#include <cstddef>

namespace eml {
namespace zephyr {

// Zephyr RTOS integration
class ZephyrRTOS {
public:
    // Thread management
    static void createInferenceThread(uint8_t priority, size_t stack_size);
    static void setThreadDeadline(uint32_t deadline_us);
    
    // Memory management
    static void* allocateFromHeap(size_t size);
    static void* allocateFromStack(size_t size);
    static void freeMemory(void* ptr);
    
    // Power management
    static void enterLowPowerMode();
    static void exitLowPowerMode();
    static void configurePowerDomain(uint8_t domain, bool enable);
    
    // Timing and scheduling
    static uint64_t getCurrentTimeUs();
    static void delayUs(uint32_t delay_us);
    static void scheduleInference(uint32_t period_us);
    
    // Interrupt handling
    static void disableInterrupts();
    static void enableInterrupts();
    
    // Hardware abstraction
    static void configureClockFrequency(uint32_t freq_mhz);
    static uint32_t getCurrentClockFrequency();
};

// Real-time constraints manager
class RealTimeManager {
public:
    struct ConstraintConfig {
        uint32_t max_execution_time_us;
        uint32_t deadline_us;
        uint8_t priority;
        bool preemptible;
    };
    
    static void setConstraints(const ConstraintConfig& config);
    static bool checkDeadlineMet();
    static void reportConstraintViolation();
    static void resetExecutionTimer(); // Add public method to reset timer
    
private:
    static ConstraintConfig current_constraints_;
    static uint64_t execution_start_time_;
};

} // namespace zephyr
} // namespace eml

#endif // EML_ZEPHYR_H