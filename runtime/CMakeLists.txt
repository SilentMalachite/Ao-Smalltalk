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
  src/NativeMethod.cpp
  src/CompiledMethod.cpp
  src/Context.cpp
  src/Interpreter.cpp
  src/TestRunner.cpp
  src/Compile.cpp
  src/VendorExtract.cpp
  src/Lookup.cpp
  src/Send.cpp
  src/kernel/Install.cpp
  src/kernel/BlockContext.cpp
  src/kernel/Object.cpp
  src/kernel/Boolean.cpp
  src/kernel/Behavior.cpp
  src/kernel/Magnitude.cpp
  src/kernel/SmallInteger.cpp
  src/kernel/LargeInteger.cpp
  src/kernel/Float.cpp
  src/kernel/Character.cpp
  src/kernel/Collection.cpp
  src/kernel/Array.cpp
  src/kernel/String.cpp
  src/kernel/Dictionary.cpp
  src/kernel/Stream.cpp
  src/kernel/Process.cpp
  src/kernel/Geometry.cpp
  src/kernel/CompiledMethodNatives.cpp
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
  tests/native_method_test.cpp
  tests/lookup_test.cpp
  tests/native_send_test.cpp
  tests/object_boolean_test.cpp
  tests/behavior_test.cpp
  tests/smallinteger_arith_test.cpp
  tests/array_string_test.cpp
  tests/collection_do_test.cpp
  tests/transcript_model_test.cpp
  tests/process_test.cpp
  tests/geometry_test.cpp
  tests/kernel_scan_test.cpp
  tests/compiled_method_test.cpp
  tests/chunk_filein_test.cpp
  tests/context_test.cpp
  tests/compiler_roundtrip_test.cpp
  tests/block_test.cpp
  tests/vendor_extract_test.cpp
  tests/vendor_overwrite_test.cpp
)
target_link_libraries(ao_runtime_tests PRIVATE ao_runtime ao_compiler GTest::gtest_main)
target_compile_definitions(ao_runtime_tests PRIVATE "AO_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
include(GoogleTest)
gtest_discover_tests(ao_runtime_tests)
