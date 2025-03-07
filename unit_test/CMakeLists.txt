set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/unit_test)

set(MY_COMM_LIB
    boost_unit_test_framework
    boost_test_exec_monitor
    bbt_core
    bbt_cxxlua
)

include_directories(
    ${PROJECT_SOURCE_DIR}
)

add_executable(Test_luaclass_gc Test_luaclass_gc.cc)
target_link_libraries(Test_luaclass_gc ${MY_COMM_LIB})
add_test(NAME Test_luaclass_gc COMMAND Test_luaclass_gc)

add_executable(Test_luavm Test_luavm.cc)
target_link_libraries(Test_luavm ${MY_COMM_LIB})
add_test(NAME Test_luavm COMMAND Test_luavm)

add_executable(Test_stack Test_stack.cc)
target_link_libraries(Test_stack ${MY_COMM_LIB})
add_test(NAME Test_stack COMMAND Test_stack)

add_executable(Test_luatype Test_luatype.cc)
target_link_libraries(Test_luatype ${MY_COMM_LIB})
add_test(NAME Test_luatype COMMAND Test_luatype)

add_executable(Test_luaclass_manager Test_luaclass_manager.cc)
target_link_libraries(Test_luaclass_manager ${MY_COMM_LIB})
add_test(NAME Test_luaclass_manager COMMAND Test_luaclass_manager)