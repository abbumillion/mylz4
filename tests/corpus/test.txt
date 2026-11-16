cmake_minimum_required(VERSION 3.20)
project(mylz4 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-O3 -march=native -flto -Wall -Wextra -Wpedantic)

add_library(mylz4 STATIC src/mylz4.c src/encode.c src/decode.c)
target_include_directories(mylz4 PUBLIC include src)

add_executable(mylz4_cli cli/mylz4.c)
target_link_libraries(mylz4_cli PRIVATE mylz4)

enable_testing()
add_executable(test_roundtrip tests/roundtrip.c)
target_link_libraries(test_roundtrip PRIVATE mylz4)
add_test(NAME roundtrip COMMAND test_roundtrip)
