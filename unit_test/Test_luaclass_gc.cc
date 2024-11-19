#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <bbt/cxxlua/CXXLua.hpp>

class Object:
    bbt::cxxlua::LuaClass<Object>
{
public:
    Object()  { m_count++; }
    ~Object() { m_count--; }

    static std::optional<bbt::cxxlua::LuaErr> CXXLuaInit(std::shared_ptr<bbt::cxxlua::detail::LuaStack>& stack)
    {
        InitClass("GCObject");
        InitConstructor([](lua_State* l){
            return Object::cxx2lua_ctor(l);
        });
        Register(stack);
        return std::nullopt;
    }

    static int GetCount() { return m_count; }
private:
    static Object* cxx2lua_ctor(lua_State* l) { return new Object(); }
private:
    std::string m_mem1{"ASDHj)&UHJH!JLHSD*YU!HKJSA12379ujak"};
    static int m_count;
};

int Object::m_count = 0;

BOOST_AUTO_TEST_SUITE(CXX_Regist_LuaClass_GC)

const char* luascript_t_gc = " \
ObjectPool = {} \
\
function Create() \
    for i = 1, math.random(10000), 1 do \
        table.insert(ObjectPool, GCObject:new()) \
    end \
end \
\
function Release() \
    ObjectPool = {} \
end \
\
function Main() \
    for i = 1, 100, 1 do \
        Create() \
        Release() \
    end \
    collectgarbage(\"collect\") \
end \
";

BOOST_AUTO_TEST_CASE(t_gc)
{
    bbt::cxxlua::LuaVM vm;

    BOOST_ASSERT(vm.LoadLuaLibrary() == std::nullopt);
    auto err1 = vm.DoScript(luascript_t_gc);
    if (err1 != std::nullopt) {
        BOOST_FAIL(err1.value().What());
    }

    auto err2 = vm.RegistClass<Object>();
    if (err2 != std::nullopt) {
        BOOST_FAIL(err2.value().What());
    }

    auto err3 = vm.CallLuaFunction("Main", 0, nullptr);
    if (err3 != std::nullopt) {
        BOOST_FAIL(err3.value().What());
    }

    BOOST_ASSERT_MSG(Object::GetCount() == 0, "lua class has memory leak!");
}

BOOST_AUTO_TEST_SUITE_END()