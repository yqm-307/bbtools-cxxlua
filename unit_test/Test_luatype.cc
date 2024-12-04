#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <bbt/cxxlua/CXXLua.hpp>

BOOST_AUTO_TEST_SUITE()

BOOST_AUTO_TEST_CASE(t_get_type)
{
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<int>, bbt::cxxlua::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<const int>, bbt::cxxlua::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<const int&>, bbt::cxxlua::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<double>, bbt::cxxlua::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<const double>, bbt::cxxlua::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<const double&>, bbt::cxxlua::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<char*>, bbt::cxxlua::LUATYPE_CSTRING);
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<const char*>, bbt::cxxlua::LUATYPE_CSTRING);
    BOOST_CHECK_EQUAL(bbt::cxxlua::detail::luatype_v<const char*&>, bbt::cxxlua::LUATYPE_CSTRING);

}

BOOST_AUTO_TEST_SUITE_END()