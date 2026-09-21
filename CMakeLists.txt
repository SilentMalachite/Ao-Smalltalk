cmake_minimum_required(VERSION 3.28)
project(ao LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OSX_ARCHITECTURES arm64)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

enable_testing()
add_subdirectory(runtime)
add_subdirectory(compiler)
target_link_libraries(ao_runtime PRIVATE ao_compiler)
