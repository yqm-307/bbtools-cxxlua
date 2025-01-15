cmake_minimum_required(VERSION 3.2)

project(bbtools-cxxlua)


include_directories(
    ${PROJECT_SOURCE_DIR}/bbt
    ${PROJECT_SOURCE_DIR}
)

file(GLOB_RECURSE SRCS 
    ${PROJECT_SOURCE_DIR}/bbt/*.cc
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(bbt_cxxlua STATIC ${SRCS})
target_link_libraries(bbt_cxxlua
    ybbt
    lua
)

# 编译例程
add_subdirectory(example)
enable_testing()
add_subdirectory(unit_test)
