#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <bbt/cxxlua/detail/LuaStack.hpp>
#include <bbt/cxxlua/detail/LuaRef.hpp>


BOOST_AUTO_TEST_SUITE()

int cfunc_none(lua_State* vm)
{
    return 0;
}

/**
 * @brief 栈上 push 和 pop
 * 
 */
BOOST_AUTO_TEST_CASE(t_push_pop)
{
    auto stack = bbt::cxxlua::detail::LuaStack::Create(nullptr);
    BOOST_ASSERT_MSG(stack->Size() == 0, "bad stack size!");

    int v1 = 10;
    double v2 = 1.1f;
    std::string v3 = "hi";
    const char* v4 = "hello";
    int64_t v5 = 0x1FFFFFFFF;
    auto v6 = bbt::cxxlua::nil;


    BOOST_CHECK_EQUAL(stack->Push(v1), bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    BOOST_CHECK_EQUAL(stack->Push(v2), bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    BOOST_CHECK_EQUAL(stack->Push(v3), bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    BOOST_CHECK_EQUAL(stack->Push(v4), bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    BOOST_CHECK_EQUAL(stack->Push(cfunc_none), bbt::cxxlua::LUATYPE::LUATYPE_FUNCTION);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_FUNCTION);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    BOOST_CHECK_EQUAL(stack->Push(v5), bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_NUMBER);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    BOOST_CHECK_EQUAL(stack->Push(v6), bbt::cxxlua::LUATYPE::LUATYPE_NIL);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_NIL);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);
}

/**
 * @brief c++调用lua
 * 
 */
BOOST_AUTO_TEST_CASE(t_c_call_lua)
{
    auto stack = bbt::cxxlua::detail::LuaStack::Create();
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    stack->LoadLuaLib();

    stack->GetGlobal("tostring");
    stack->LuaCall(1, 1, 100);

    /* 函数返回一个值，此时栈上应只有一个值，且应为string */
    BOOST_CHECK_EQUAL(stack->Size(), 1);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);

    auto [err, ref] = stack->GetRef(-1);
    if (err)
        BOOST_WARN_MESSAGE(false, err.value().What());
    BOOST_ASSERT(!err.has_value());

    std::string str;
    auto get_value_err = ref->GetValue(str);
    if (get_value_err)
        BOOST_WARN_MESSAGE(false, err.value().What());
    BOOST_ASSERT(!get_value_err.has_value());

    BOOST_CHECK_EQUAL(str, "100");
    BOOST_CHECK_EQUAL(stack->Size(), 1);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);
}

/**
 * @brief 栈上表操作
 * 
 */
BOOST_AUTO_TEST_CASE(t_table_opt)
{

}

BOOST_AUTO_TEST_SUITE_END()