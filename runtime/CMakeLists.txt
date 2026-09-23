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
  src/Session.cpp
  src/abi.cpp
  src/Oop.cpp
  src/Heap.cpp
  src/VirtualRegion.cpp
  src/Gc.cpp
  src/Roots.cpp
  src/HandleScope.cpp
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
  src/ImageFormat.cpp
  src/ImageSave.cpp
  src/ImageLoad.cpp
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
target_include_directories(ao PRIVATE ${CMAKE_SOURCE_DIR}/compiler/include)
target_compile_options(ao PRIVATE -Wall -Wextra -Werror)
set_target_properties(ao PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(ao_runtime_tests
  tests/smoke_test.cpp
  tests/oop_tag_test.cpp
  tests/heap_alloc_test.cpp
  tests/gc_test.cpp
  tests/gc_stress_test.cpp
  tests/gc_safety_test.cpp
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
  tests/print_string_test.cpp
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
  tests/interpreter_test.cpp
  tests/vendor_extract_test.cpp
  tests/vendor_overwrite_test.cpp
  tests/vendor_filein_test.cpp
  tests/image_format_test.cpp
  tests/image_registry_test.cpp
  tests/image_save_load_test.cpp
  tests/session_abi_test.cpp
  tests/browser_abi_test.cpp
  tests/accept_abi_test.cpp
  tests/failure_test.cpp
)
target_link_libraries(ao_runtime_tests PRIVATE ao_runtime ao_compiler GTest::gtest_main)
target_compile_definitions(ao_runtime_tests PRIVATE "AO_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
include(GoogleTest)
gtest_discover_tests(ao_runtime_tests)

# runtime のスイート全体を、1 プロセスで GC ストレス下に回す（SPEC §4.1）。
# KernelBench.* は時間計測のテストで、通常の項目として回っている。ストレス下では計測の意味がなく、
# 1 本で十数秒かかるので除く。
add_test(NAME gcstress COMMAND ao_runtime_tests --gtest_filter=-KernelBench.*)
set_tests_properties(gcstress PROPERTIES ENVIRONMENT "AO_GC_STRESS=1")

# CLI の vendor file-in（image/vendor/LOAD_ORDER）と Smalltalk のテスト（ao --test image/tests）を、
# GC ストレス下で回す。ao --test は vendor を file-in しないので、先に filein を別プロセスで回す。
# ao filein は DEFERRED.md に列挙していないエラーを stderr に出し、あれば exit 1 で終わる（SPEC §3.12）。
# ストレスなしの 1 回を基準にして、ストレス下の出力がそれと一致し、どちらも exit 0 であることを確かめる。
# DEFERRED.md の集合との一致は、gcstress で回る vendor_filein_test が確かめる。
add_test(NAME gcstress_vendor
  COMMAND sh -c "AO_GC_STRESS=0 \"$0\" filein --load-order \"$1\" 2>\"$3.base\" && \"$0\" filein --load-order \"$1\" 2>\"$3.stress\" && diff \"$3.base\" \"$3.stress\" && \"$0\" --test \"$2\""
          $<TARGET_FILE:ao> ${CMAKE_SOURCE_DIR}/image/vendor/LOAD_ORDER
          ${CMAKE_SOURCE_DIR}/image/tests ${CMAKE_CURRENT_BINARY_DIR}/gcstress_vendor_filein)
set_tests_properties(gcstress_vendor PROPERTIES ENVIRONMENT "AO_GC_STRESS=1")

# CLI を実プロセスで起動し、終了コードと stderr の行を確かめる（SPEC §3.12 の file-in、§4.4 の ao --test）。
foreach(cli_case filein image_save test)
  add_test(NAME cli_${cli_case}
    COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/tests/cli_test.sh $<TARGET_FILE:ao>
            ${CMAKE_CURRENT_BINARY_DIR}/cli_${cli_case} ${cli_case})
endforeach()
