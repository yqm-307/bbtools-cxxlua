
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test/lua)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")

add_executable(luatest luatest.cc)
target_link_libraries(luatest ybbt lua)


add_executable(luaclass luaclass.cc)
target_link_libraries(luaclass ybbt lua)

add_executable(luatable luatable.cc)
target_link_libraries(luatable ybbt lua)