#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <bbt/cxxlua/CXXLua.hpp>

bbt::cxxlua::LuaVM lvm;

BOOST_AUTO_TEST_SUITE(TableOpt)

/**
 *  全局变量读写 置空
 */
BOOST_AUTO_TEST_CASE(t_global_value_ready_write)
{
    int value_a = 1;

    auto do_load_lualib_err = lvm.LoadLuaLibrary();
    if (do_load_lualib_err)
        BOOST_ERROR(do_load_lualib_err.value().What());

    auto do_set_a_err = lvm.SetGlobalValue("a", value_a);
    if (do_set_a_err)
        BOOST_ERROR(do_set_a_err.value().What());

    bbt::cxxlua::detail::LuaValue a;
    auto do_get_a_err = lvm.GetGlobalValue("a", a);
    if (do_get_a_err)
        BOOST_ERROR(do_set_a_err.value().What());

    BOOST_CHECK_EQUAL(a.basevalue.integer, value_a);

    a.type = bbt::cxxlua::LUATYPE::LUATYPE_NIL;
    lvm.SetGlobalValue("a", a);
}

// 写已有 luatable
BOOST_AUTO_TEST_CASE(t_writeonly_lua_table)
{

}

// 创建全局表 + 读写操作 + 异常访问
BOOST_AUTO_TEST_CASE(t_create_global)
{

}

BOOST_AUTO_TEST_SUITE_END()