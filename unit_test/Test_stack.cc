#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <bbt/cxxlua/detail/LuaStack.hpp>
#include <bbt/cxxlua/detail/LuaRef.hpp>


BOOST_AUTO_TEST_SUITE()

/**
 * @brief 栈上 push 和 pop
 * 
 */
BOOST_AUTO_TEST_CASE(t_push_pop)
{
    auto stack = bbt::cxxlua::detail::LuaStack::Create(nullptr);

    int v1 = 10;
    double v2 = 1.1f;
    std::string v3 = "hi";
    const char* v4 = "hello";


    BOOST_ASSERT(stack->Push(v1) == bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);
    BOOST_ASSERT(stack->GetTop().GetType() == bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);

    BOOST_ASSERT(stack->Push(v2) == bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);
    BOOST_ASSERT(stack->GetTop().GetType() == bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);

    BOOST_ASSERT(stack->Push(v3) == bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);
    BOOST_ASSERT(stack->GetTop().GetType() == bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);

    BOOST_ASSERT(stack->Push(v4) == bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);
    BOOST_ASSERT(stack->GetTop().GetType() == bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);
}


BOOST_AUTO_TEST_SUITE_END()