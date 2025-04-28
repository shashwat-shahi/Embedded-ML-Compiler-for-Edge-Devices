cmake_minimum_required(VERSION 3.16)
project(EmbeddedMLCompiler VERSION 1.0.0 LANGUAGES C CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Check if we're cross-compiling for ARM
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    # Compiler flags for ARM Cortex-M (only when cross-compiling)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
endif()

# Optimization flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/compiler
    ${CMAKE_SOURCE_DIR}/src/runtime
    ${CMAKE_SOURCE_DIR}/src/cmsis_nn
    ${CMAKE_SOURCE_DIR}/src/zephyr
)

# Source files
file(GLOB_RECURSE COMPILER_SOURCES "src/compiler/*.cpp" "src/compiler/*.c")
file(GLOB_RECURSE RUNTIME_SOURCES "src/runtime/*.cpp" "src/runtime/*.c")
file(GLOB_RECURSE CMSIS_SOURCES "src/cmsis_nn/*.cpp" "src/cmsis_nn/*.c")
file(GLOB_RECURSE ZEPHYR_SOURCES "src/zephyr/*.cpp" "src/zephyr/*.c")
file(GLOB_RECURSE POWER_SOURCES "src/power/*.cpp" "src/power/*.c")

# Main library
add_library(embedded_ml_compiler STATIC
    ${COMPILER_SOURCES}
    ${RUNTIME_SOURCES}
    ${CMSIS_SOURCES}
    ${ZEPHYR_SOURCES}
    ${POWER_SOURCES}
)

target_include_directories(embedded_ml_compiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Compiler executable
add_executable(eml_compiler
    tools/compiler_main.cpp
)

target_link_libraries(eml_compiler embedded_ml_compiler)

# Example applications
add_subdirectory(examples)

# Enable testing
enable_testing()
add_subdirectory(tests)