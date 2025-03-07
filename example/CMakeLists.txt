
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/example)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")

add_executable(luatest luatest.cc)
target_link_libraries(luatest bbt_core bbt_cxxlua)

add_executable(luaclass luaclass.cc)
target_link_libraries(luaclass bbt_core bbt_cxxlua)