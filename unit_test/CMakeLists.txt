set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/unit_test)

set(MY_COMM_LIB
    boost_unit_test_framework
    boost_test_exec_monitor
    ybbt
)

include_directories(
    ${PROJECT_SOURCE_DIR}
)

add_executable(Test_luaclass_gc Test_luaclass_gc.cc)
target_link_libraries(Test_luaclass_gc ${MY_COMM_LIB} lua)
add_test(NAME Test_luaclass_gc COMMAND Test_luaclass_gc)