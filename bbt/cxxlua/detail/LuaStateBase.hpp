#pragma once
#include "Config.hpp"

namespace bbt::cxxlua::detail
{

class LuaStateBase
{
public:
    LuaStateBase():m_lua(luaL_newstate()) {}
    ~LuaStateBase() { lua_close(m_lua); }
protected:
    lua_State* Context() { return m_lua; }

private:
    lua_State* m_lua;
};

} // namespace bbt::cxxlua::detail