#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <bbt/cxxlua/CXXLua.hpp>

class Object:
    public bbt::cxxlua::LuaClass<Object>,
    public std::enable_shared_from_this<Object>
{
public:
    Object()  { m_count++; }
    ~Object() { m_count--; }

    static std::optional<bbt::cxxlua::LuaErr> CXXLuaInit()
    {
        InitClass("GCObject");
        InitFuncs({
            {"GetMem1", GenCallable(&Object::cxx2lua_GetMem1, "GetMem1", bbt::cxxlua::emCallType_MemberFunc)},
            {"SetMem1", GenCallable(&Object::cxx2lua_SetMem1, "SetMem1", bbt::cxxlua::emCallType_MemberFunc)},
        });
        InitField({
            {"mem1", GenCallable(&Object::cxx2lua_GetMem1, "mem1", bbt::cxxlua::emCallType_ReadonlyFunc)},
            {"mem2", GenCallable(&Object::GetMem2, "mem2", bbt::cxxlua::emCallType_ReadonlyFunc)},
        });
        return std::nullopt;
    }

    static int GetCount() { return m_count; }
    int cxx2lua_GetMem1(lua_State* l)
    {
        lua_pushstring(l, m_mem1.c_str());
        return 1;
    }
    int cxx2lua_SetMem1(lua_State* l)
    {
        const char* mem1 = lua_tostring(l, -1);
        m_mem1 = mem1;
        return 0;
    }

    int GetMem2(lua_State* l)
    { 
        lua_pushinteger(l, m_mem2);        
        return 1;
    }
private:
    static Object* cxx2lua_ctor(lua_State* l) { return new Object(); }
private:
    std::string m_mem1{"ASDHj)&UHJH!JLHSD*YU!HKJSA12379ujak"};
    int m_mem2{0};
    static int m_count;
};

class ObjectMgr
{
public:
    ObjectMgr() = default;
    ~ObjectMgr() = default;

    void AddObject(int id, const Object& obj)
    {
        m_objects[id] = obj;
    }

    void RemoveObject(int id)
    {
        m_objects.erase(id);
    }

    Object& GetGObject(int id)
    {
        return m_objects[id];
    }

    int GetObjectCount() const
    {
        return m_objects.size();
    }
private:
    std::map<int, Object> m_objects;
};

std::shared_ptr<Object> g_object = nullptr;

int GetGObject(lua_State* l)
{
    if (!g_object)
        return 0;

    return g_object->PushMe(l, g_object->weak_from_this());
}

int NewObject(lua_State* l)
{
    g_object = std::make_shared<Object>();
    return 0;
}

int DelObject(lua_State* l)
{
    g_object = nullptr;
    return 0;
}

int Object::m_count = 0;

BOOST_AUTO_TEST_SUITE(CXX_Regist_LuaClass_GC)

std::string luascript_t_gc = R"(
function Main()
    NewGObj()
    local obj = GetGObj()
    DelGObj()
    obj = GetGObj()
    print(obj == nil)
end
)";

BOOST_AUTO_TEST_CASE(t_regist_class)
{
    bbt::cxxlua::LuaVM vm;

    BOOST_ASSERT(vm.LoadLuaLibrary() == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("GetGObj", GetGObject) == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("NewGObj", NewObject) == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("DelGObj", DelObject) == std::nullopt);

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

std::string luascript__memberfunc = R"(
function Main()
    NewGObj()
    local obj = GetAObj()
    print(type(obj))
    obj:SetMem1("hello world")
    print(obj:GetMem1())
    obj:SetMem1("i change set mem1 succ!")
    DelObj()
end
)";

BOOST_AUTO_TEST_CASE(t_regist_class_with_memberfunc)
{
    bbt::cxxlua::LuaVM vm;
    g_object = std::make_shared<Object>();
    BOOST_ASSERT(vm.LoadLuaLibrary() == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("GetGObj", GetGObject) == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("NewGObj", NewObject) == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("DelGObj", DelObject) == std::nullopt);

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

    g_object = nullptr;
    BOOST_ASSERT_MSG(Object::GetCount() == 0, "lua class has memory leak!");
}

BOOST_AUTO_TEST_CASE(t_regist_class_with_readonly_value)
{

}

BOOST_AUTO_TEST_SUITE_END()