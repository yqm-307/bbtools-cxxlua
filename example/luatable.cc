#include <bbt/cxxlua/CXXLua.hpp>
#include <queue>

class MsgQueue:
    public bbt::cxxlua::LuaTableHelper
{
public:
    static std::shared_ptr<MsgQueue> GetInst() {
        std::shared_ptr<MsgQueue> msg = nullptr;
        if (msg == nullptr) {
            msg = std::shared_ptr<MsgQueue>(new MsgQueue);
        }

        return msg;
    }

    ~MsgQueue() {}

private:
    MsgQueue() 
    {
        InitCFunctions({
            {"SendMsg", SendMsg},
            {"GetMsg", GetMsg}
        });

        InitTableName("MsgQueue");
    }

    static int SendMsg(lua_State* l) {
        int type = lua_type(l, -1);
        if (type != LUA_TSTRING) {
            lua_pushboolean(l, 0);
            return 1;
        }

        const char* msg = lua_tostring(l, -1);
        if (msg == nullptr) {
            lua_pushboolean(l, 0);
            return 1;
        }

        g_msg.push(msg);
        printf("from lua: %s\n", msg);
        lua_pushboolean(l, 1);
        return 1;
    }

    static int GetMsg(lua_State* l) {
        if (g_msg.empty()) {
            lua_pushnil(l);
            return 1;
        }

        auto msg =  g_msg.front();
        lua_pushstring(l, msg.c_str());
        return 1;
    }

    static std::queue<std::string> g_msg;

};

std::queue<std::string> MsgQueue::g_msg;

int main()
{
    bbt::cxxlua::LuaVM vm;

    assert( vm.LoadLuaLibrary()         == std::nullopt);
    assert( vm.LoadFile("example/script/luatable/luatable.lua") == std::nullopt);



    assert( vm.RegistATableTemplate(MsgQueue::GetInst()) == std::nullopt );

    assert(vm.PushAGlobalTableByName("MsgQueue", "MsgQueue") == std::nullopt );

    auto err = vm.CallLuaFunction("Main", 0, nullptr);

    auto err2 = vm.CallLuaFunction("Notify", 0, nullptr);

}