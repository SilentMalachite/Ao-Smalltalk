add_library(ao_compiler src/Compiler.cpp src/Scanner.cpp src/Parser.cpp src/Codegen.cpp
  src/ChunkParser.cpp)
target_include_directories(ao_compiler
  PUBLIC
    ${CMAKE_SOURCE_DIR}/compiler/include
)
target_compile_options(ao_compiler PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Werror>")

add_executable(ao_compiler_tests tests/smoke_test.cpp tests/scanner_test.cpp tests/parser_test.cpp
  tests/bytecode_isa_test.cpp tests/codegen_snapshot_test.cpp tests/workspace_temp_test.cpp
  tests/chunk_test.cpp tests/pc_map_test.cpp)
target_link_libraries(ao_compiler_tests PRIVATE ao_compiler GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(ao_compiler_tests)
