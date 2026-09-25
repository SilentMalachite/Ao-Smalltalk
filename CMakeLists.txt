cmake_minimum_required(VERSION 3.28)
if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum macOS version (matches app/Package.swift)" FORCE)
endif()
project(ao LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OSX_ARCHITECTURES arm64)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

option(AO_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(AO_SANITIZE)
  set(AO_SANITIZE_FLAGS -fsanitize=address,undefined -fno-sanitize-recover=undefined
    -fno-omit-frame-pointer)
  add_compile_options(${AO_SANITIZE_FLAGS})
  add_link_options(${AO_SANITIZE_FLAGS})
endif()

enable_testing()
add_subdirectory(runtime)
add_subdirectory(compiler)
target_link_libraries(ao_runtime PRIVATE ao_compiler)
