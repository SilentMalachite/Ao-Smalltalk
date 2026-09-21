include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(ao_runtime
  src/Runtime.cpp
  src/abi.cpp
  src/Oop.cpp
  src/Heap.cpp
  src/Gc.cpp
  src/Roots.cpp
  src/WellKnown.cpp
  src/Bootstrap.cpp
  src/Globals.cpp
  src/Symbol.cpp
  src/MethodDictionary.cpp
)
target_include_directories(ao_runtime
  PUBLIC
    ${CMAKE_SOURCE_DIR}/runtime/include
    ${CMAKE_SOURCE_DIR}/bridge
)
target_compile_options(ao_runtime PRIVATE -Wall -Wextra -Werror)

add_executable(ao src/ao_main.cpp)
target_link_libraries(ao PRIVATE ao_runtime)
target_compile_options(ao PRIVATE -Wall -Wextra -Werror)
set_target_properties(ao PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(ao_runtime_tests
  tests/smoke_test.cpp
  tests/oop_tag_test.cpp
  tests/heap_alloc_test.cpp
  tests/gc_test.cpp
  tests/bootstrap_test.cpp
  tests/symbol_test.cpp
  tests/method_dictionary_test.cpp
)
target_link_libraries(ao_runtime_tests PRIVATE ao_runtime GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(ao_runtime_tests)
