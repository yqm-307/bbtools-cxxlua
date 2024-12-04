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

int cfunc_add(lua_State* vm)
{
    auto num = lua_gettop(vm);
    if (num < 2) {
        lua_getglobal(vm, "error");
        lua_pushliteral(vm, "must 2 params!");
        lua_pcall(vm, 1, 0, 0);
        return 0;
    }

    int total = 0;
    for (int i = 1; i <= num; ++i)
    {
        auto type = lua_type(vm, -1);
        if (type != bbt::cxxlua::LUATYPE_NUMBER)
            return 0;

        int value = lua_tointeger(vm, i);
        total += value;
    }

    lua_pushinteger(vm, total);
    return 1;
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
 * @brief 栈上表操作
 * 
 */
BOOST_AUTO_TEST_CASE(t_table_opt)
{
    auto stack = bbt::cxxlua::detail::LuaStack::Create();
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    stack->LoadLuaLib();

    // BOOST_ASSERT(!stack->DoScript(lua_script_example1));

    stack->NewLuaTable();
    BOOST_ASSERT(!stack->Insert2Table("key1", "i am value1"));
    BOOST_ASSERT(!stack->Insert2Table("key2", 1));
    BOOST_ASSERT(!stack->Insert2Table("key3", cfunc_add));

    auto [getref_err, ref] = stack->GetRef(-1);
    BOOST_ASSERT(!getref_err);
    BOOST_ASSERT(!stack->SetGlobalValue("tb", ref.value()));

    stack->DoScript(R"( for k, v in pairs(tb) do print("xxx", k, v) end )");

    BOOST_CHECK_EQUAL(stack->Size(), 1);


    {
        stack->GetGlobal("tb");
        std::string value1;
        auto [pop_key1_err, type] = stack->Pop4Table("key1");
        BOOST_ASSERT(!pop_key1_err);
        BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE_CSTRING);
        auto [err, value] = stack->GetTop().GetValue();
        BOOST_ASSERT(!err);
        BOOST_ASSERT(!value->GetValue(value1));
        BOOST_CHECK_EQUAL(value1, "i am value1");
        stack->Pop(1);
    }

    {
        int value2;
        auto [pop_key2_err, type] = stack->Pop4Table("key2");
        BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE_NUMBER);
        auto [err, value] = stack->GetTop().GetValue();
        BOOST_ASSERT(!err);
        BOOST_ASSERT(!value->GetValue(value2));
        BOOST_CHECK_EQUAL(value2, 1);
        stack->Pop(1);
    }

    {
        lua_CFunction value3;
        auto [pop_key3_err, type] = stack->Pop4Table("key3");
        BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE_FUNCTION);
        auto [err, value] = stack->GetTop().GetValue();
        BOOST_ASSERT(!err);
        BOOST_ASSERT(!value->GetValue(value3));
        BOOST_CHECK_EQUAL(value3, cfunc_add);
        stack->Pop(1);
    }

}

/**
 * @brief 调用lua tostring 并解析返回值
 * 
 */
BOOST_AUTO_TEST_CASE(t_call_lua_tostring)
{
    auto stack = bbt::cxxlua::detail::LuaStack::Create();
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    /* 加载lualib */
    stack->LoadLuaLib();

    stack->GetGlobal("tostring");
    auto call_err = stack->LuaCall(1, 1, 100);
    if (call_err)
        BOOST_ERROR(call_err.value().What());
    BOOST_ASSERT(!call_err.has_value());

    /* 函数返回一个值，此时栈上应只有一个值，且应为string */
    BOOST_CHECK_EQUAL(stack->Size(), 1);
    BOOST_CHECK_EQUAL(stack->GetTop().GetType(), bbt::cxxlua::LUATYPE::LUATYPE_CSTRING);

    auto [err, ref] = stack->GetRef(-1);
    if (err)
        BOOST_ERROR(err.value().What());
    BOOST_ASSERT(!err.has_value());

    std::string str;
    auto get_value_err = ref->GetValue(str);
    if (get_value_err)
        BOOST_ERROR(err.value().What());
    BOOST_ASSERT(!get_value_err.has_value());

    BOOST_CHECK_EQUAL(str, "100");
    BOOST_CHECK_EQUAL(stack->Size(), 1);
    stack->Pop(1);
    BOOST_CHECK_EQUAL(stack->Size(), 0);
}


auto lua_function_example_1 =
/**
 * @brief 调用lua function 并解析返回值
 */
R"(
function Function1(...)
    return {...}
end
)";
BOOST_AUTO_TEST_CASE(t_call_lua_func1)
{
    auto stack = bbt::cxxlua::detail::LuaStack::Create();
    BOOST_CHECK_EQUAL(stack->Size(), 0);

    stack->LoadLuaLib();

    auto do_script_err = stack->DoScript(lua_function_example_1);
    if (do_script_err)
        BOOST_ERROR(do_script_err->What());
    BOOST_ASSERT(!do_script_err.has_value());

    auto type = stack->Push("hello lua");
    BOOST_CHECK_EQUAL(type, bbt::cxxlua::LUATYPE_CSTRING);

    type = stack->Push(1);
    BOOST_CHECK_EQUAL(type, bbt::cxxlua::LUATYPE_NUMBER);

    type = stack->Push(1.1);
    BOOST_CHECK_EQUAL(type, bbt::cxxlua::LUATYPE_NUMBER);

    stack->NewLuaTable();

}

BOOST_AUTO_TEST_SUITE_END()