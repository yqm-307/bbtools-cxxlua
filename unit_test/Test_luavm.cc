#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <bbt/cxxlua/CXXLua.hpp>


#define RET_OPTION_ERROR(fn, err)\
{ \
    auto err = fn; \
    if (err) \
        BOOST_ERROR(err.value().What()); \
}

bbt::cxxlua::LuaVM lvm;

BOOST_AUTO_TEST_SUITE(TableOpt)

/**
 *  全局int读写 置空
 */
BOOST_AUTO_TEST_CASE(t_global_value_read_write_int)
{
    int value_a = 1;
    bbt::cxxlua::detail::LuaValue lua_a;

    RET_OPTION_ERROR(lvm.LoadLuaLibrary(), do_load_lualib_err);
    RET_OPTION_ERROR(lvm.SetGlobalValue("a", value_a), do_set_a_err);
    RET_OPTION_ERROR(lvm.GetGlobalValue("a", lua_a), do_get_a_err);

    int number;
    BOOST_ASSERT(lua_a.GetValue(number));

    BOOST_CHECK_EQUAL(number, value_a);
    RET_OPTION_ERROR(lvm.SetGlobalValue("a", bbt::cxxlua::nil), do_set_a_nil_err);
    RET_OPTION_ERROR(lvm.GetGlobalValue("a", lua_a), do_get_a_err);
    BOOST_CHECK_EQUAL(lua_a.GetType(), bbt::cxxlua::LUATYPE_NIL);
}

/**
 *  全局str读写 置空
 */
BOOST_AUTO_TEST_CASE(t_global_value_read_write_string)
{
    std::string value_a = "hello world";
    bbt::cxxlua::detail::LuaValue lua_a;

    RET_OPTION_ERROR(lvm.LoadLuaLibrary(), do_load_lualib_err);
    RET_OPTION_ERROR(lvm.SetGlobalValue("a", value_a), do_set_a_err);
    RET_OPTION_ERROR(lvm.GetGlobalValue("a", lua_a), do_get_a_err);

    std::string str;
    BOOST_ASSERT(lua_a.GetValue(str));

    BOOST_CHECK_EQUAL(str, value_a);
    RET_OPTION_ERROR(lvm.SetGlobalValue("a", bbt::cxxlua::nil), do_set_a_nil_err);
    RET_OPTION_ERROR(lvm.GetGlobalValue("a", lua_a), do_get_a_err);
    BOOST_CHECK_EQUAL(lua_a.GetType(), bbt::cxxlua::LUATYPE_NIL);

}

/**
 *  全局number读写 置空
 */
BOOST_AUTO_TEST_CASE(t_global_value_read_write_double)
{
    double value_a = 1.23f;
    bbt::cxxlua::detail::LuaValue lua_a;

    RET_OPTION_ERROR(lvm.LoadLuaLibrary(), do_load_lualib_err);
    RET_OPTION_ERROR(lvm.SetGlobalValue("a", value_a), do_set_a_err);
    RET_OPTION_ERROR(lvm.GetGlobalValue("a", lua_a), do_get_a_err);

    double number;
    BOOST_ASSERT(lua_a.GetValue(number));

    BOOST_CHECK_EQUAL(number, value_a);
    RET_OPTION_ERROR(lvm.SetGlobalValue("a", bbt::cxxlua::nil), do_set_a_nil_err);
    RET_OPTION_ERROR(lvm.GetGlobalValue("a", lua_a), do_get_a_err);
    BOOST_CHECK_EQUAL(lua_a.GetType(), bbt::cxxlua::LUATYPE_NIL);
}

/**
 * 全局table读写 置空
 */
BOOST_AUTO_TEST_CASE(t_global_value_read_write_table)
{
    bbt::cxxlua::detail::LuaValue lua_a;

    RET_OPTION_ERROR(lvm.LoadLuaLibrary(), do_load_lualib_err);

}

BOOST_AUTO_TEST_SUITE_END()