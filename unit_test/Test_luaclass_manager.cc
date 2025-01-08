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
        InitClass("CXXObject");
        return std::nullopt;
    }

    static int GetCount() { return m_count; }
private:
    static int m_count;
};

int Object::m_count = 0;


class ObjMgr
{
public:
    static int LuaGetAObjById(lua_State* l)
    {
        int id = lua_tointeger(l, -1);
        auto it = objmap.find(id);
        assert(it != objmap.end());
        it->second->PushMe(l, it->second->weak_from_this());
        return 1;
    }
    static int LuaCreateObject(lua_State* l)
    {
        int id = lua_tointeger(l, -1);
        objmap.insert(std::make_pair(id, std::make_shared<Object>()));
        return 0;
    }

    static int LuaReleaseObject(lua_State* l)
    {
        int type = lua_type(l, -1);
        assert(type == LUA_TNUMBER);
        int id = lua_tointeger(l, -1);
        auto it = objmap.find(id);
        assert(it != objmap.end());
        objmap[id] = nullptr;
        objmap.erase(it);
        return 0;
    }

public:
    static int id;
    static std::map<int, std::shared_ptr<Object>> objmap;
};

int ObjMgr::id = 0;
std::map<int, std::shared_ptr<Object>> ObjMgr::objmap = std::map<int, std::shared_ptr<Object>>();


BOOST_AUTO_TEST_SUITE()

std::string luascript = R"(

local tbIdList = {}
local tbCXXObjList = {}
for i=0, 100 do
    CreateObject(i)
    local obj = GetAObjById(i)
    assert(obj ~= nil)
    tbCXXObjList[i] = obj
    table.insert(tbIdList, i)
end

for _, id in pairs(tbIdList) do
    -- print("release before obj id=", id, tbCXXObjList[id])
    ReleaseObject(id)
    -- print("release after obj id=", id, tbCXXObjList[id])
end

print(type(tbCXXObjList[1]), tbCXXObjList[1])
-- tbCXXObjList[1]:f()

)";

BOOST_AUTO_TEST_CASE(t_manager_example_gc)
{
    bbt::cxxlua::LuaVM vm;

    BOOST_ASSERT(vm.LoadLuaLibrary() == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("GetAObjById", &ObjMgr::LuaGetAObjById) == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("CreateObject", &ObjMgr::LuaCreateObject) == std::nullopt);
    BOOST_ASSERT(vm.SetGlobalValue("ReleaseObject", &ObjMgr::LuaReleaseObject) == std::nullopt);

    auto err2 = vm.RegistClass<Object>();
    if (err2 != std::nullopt) {
        BOOST_FAIL(err2.value().What());
    }

    auto err1 = vm.DoScript(luascript);
    if (err1 != std::nullopt) {
        BOOST_FAIL(err1.value().What());
    }

    BOOST_ASSERT_MSG(Object::GetCount() == 0, "lua class has memory leak!");
}

BOOST_AUTO_TEST_SUITE_END()