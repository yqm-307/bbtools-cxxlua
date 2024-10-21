#pragma once
#include "Config.hpp"

namespace bbt::cxxlua::detail
{

class LuaRef
{
    friend class LuaStack;
    template<typename T> friend class LuaClass;
public:
    explicit LuaRef(int index, LUATYPE type):m_index(index), m_type(type) {}
    LuaRef(const LuaRef& other):m_index(other.m_index), m_type(other.m_type) {}
    ~LuaRef() {}

protected:
    void SetIndex(int index, LUATYPE type) { m_index = index; m_type = type; }
    int  GetIndex() const { return m_index; }
    LUATYPE GetType() const { return m_type; }
private:
    int m_index{0};
    LUATYPE m_type{LUATYPE::LUATYPE_NONE};
};

static const LuaRef g_lua_top_ref = LuaRef(-1, LUATYPE::LUATYPE_NONE);

} // namespace bbt::cxxlua::detail