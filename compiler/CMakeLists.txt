add_library(ao_compiler src/Compiler.cpp src/Scanner.cpp)
target_include_directories(ao_compiler
  PUBLIC
    ${CMAKE_SOURCE_DIR}/compiler/include
)
target_compile_options(ao_compiler PRIVATE -Wall -Wextra -Werror)

add_executable(ao_compiler_tests tests/smoke_test.cpp tests/scanner_test.cpp)
target_link_libraries(ao_compiler_tests PRIVATE ao_compiler GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(ao_compiler_tests)
