cmake_minimum_required(VERSION 3.2)

project(bbtools-cxxlua)

set(Wall_Flag "-Wall -Wno-sign-compare -Wno-format -Wno-reorder -Wno-unknown-pragmas")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++17 -g ${Wall_Flag}")

include_directories(
    ${PROJECT_SOURCE_DIR}/bbt
    ${PROJECT_SOURCE_DIR}
)

file(GLOB_RECURSE SRCS 
    ${PROJECT_SOURCE_DIR}/bbt/*.cc
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(bbt_cxxlua STATIC ${SRCS})
target_link_libraries(bbt_cxxlua PRIVATE
    bbt_core
    lua
)

# 编译例程
add_subdirectory(example)
enable_testing()
add_subdirectory(unit_test)
