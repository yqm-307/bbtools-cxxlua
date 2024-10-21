cmake_minimum_required(VERSION 3.2)

project(bbtools-cxxlua)


include_directories(
    ${PROJECT_SOURCE_DIR}/bbt
    ${PROJECT_SOURCE_DIR}
)


# 编译例程
add_subdirectory(example)
enable_testing()
add_subdirectory(unit_test)
