#include <bbt/cxxlua/CXXLua.hpp>
#include <optional>
#include <iostream>
#include <thread>

class Player : public bbt::cxxlua::LuaClass<Player>
{
public:
    Player() {
    }
    ~Player() {
    }

    static std::optional<bbt::cxxlua::LuaErr> CXXLuaInit() {
        InitFuncs({
            {"GetId", GenCallable(&Player::cxx2lua_GetId, "id", emCallType_ReadonlyFunc)},
            {"SetId", GenCallable(&Player::cxx2lua_SetId, "SetId", emCallType_MemberFunc)},
            {"GetName", GenCallable(&Player::cxx2lua_GetName, "GetName", emCallType_MemberFunc)},
            {"SetName", GenCallable(&Player::cxx2lua_SetName, "SetName", emCallType_MemberFunc)},
        });

        InitClass("Player");
        InitConstructor([] (lua_State* l) {
            return cxx2lua_construct(l);
        });
        return std::nullopt;
    }

public:
    int cxx2lua_GetId(lua_State* l) {
        auto a = m_id;
        lua_pushinteger(l, m_id);
        return 1;
    }

    int cxx2lua_SetId(lua_State* l) {
        m_id = lua_tointeger(l, -1);
        return 0;
    }

    int cxx2lua_GetName(lua_State* l) {
        lua_pushstring(l, m_name.c_str());
        return 1;
    }

    int cxx2lua_SetName(lua_State* l) {
        const char* name = lua_tostring(l, -1);
        assert(name != nullptr);
        m_name = name;
        return 0;
    }

    static Player* cxx2lua_construct(lua_State* l) {
        auto id = lua_tointeger(l, -1);
        auto name = lua_tostring(l, -2);
        auto player = new Player;
        player->InitArgs(id, name);
        return player;
    }

    void InitArgs(int id, const char* name) {
        m_id = id;
        m_name = name;
    }

private:
    int m_id;
    std::string m_name;
};


int main()
{
    bbt::cxxlua::LuaVM vm;

    assert( vm.RegistClass<Player>()    == std::nullopt);
    assert( vm.LoadLuaLibrary()         == std::nullopt);
    assert( vm.LoadFile("example/script/luaclass/luaclass.lua") == std::nullopt);

    auto err = vm.CallLuaFunction("Main", 0, nullptr);
    if (err != std::nullopt) {
        printf("%s\n", err.value().What().c_str());
    }

    auto err2 = vm.DoScript("print(\" do script once \") Main()");
    if (err2 != std::nullopt) {
        printf("%s\n", err.value().What().c_str());
    }

}