cmake_minimum_required(VERSION 3.16)

# Test framework - using simple assert-based tests
add_executable(test_compiler
    test_compiler.cpp
)

target_link_libraries(test_compiler embedded_ml_compiler)

add_executable(test_runtime
    test_runtime.cpp
)

target_link_libraries(test_runtime embedded_ml_compiler)

add_executable(test_power
    test_power.cpp
)

target_link_libraries(test_power embedded_ml_compiler)

# Add tests to CTest
add_test(NAME CompilerTest COMMAND test_compiler)
add_test(NAME RuntimeTest COMMAND test_runtime)
add_test(NAME PowerTest COMMAND test_power)