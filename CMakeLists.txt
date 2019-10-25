cmake_minimum_required(VERSION 3.0)

find_library(
    ffi
    NAMES ffi libffi
    PATHS lib
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wno-unused-parameter -g")
add_library(neon_ffi SHARED ffi.cpp)
target_include_directories(neon_ffi PRIVATE ../../common)
target_link_libraries(neon_ffi ${ffi})
