cmake_minimum_required(VERSION 3.16)

# Example applications
add_executable(basic_inference_example
    basic_inference.cpp
)

target_link_libraries(basic_inference_example embedded_ml_compiler)

add_executable(power_aware_example
    power_aware_inference.cpp
)

target_link_libraries(power_aware_example embedded_ml_compiler)

add_executable(real_time_example
    real_time_inference.cpp
)

target_link_libraries(real_time_example embedded_ml_compiler)