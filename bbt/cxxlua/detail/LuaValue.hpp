#pragma once
#include "Config.hpp"

namespace bbt::cxxlua::detail
{

union BaseValue
{
    bool    boolean;
    int     integer;
    double  number;
};



struct LuaValue
{
    BaseValue       basevalue;
    lua_CFunction   cfunc{nullptr};
    std::string     str{""};
    LUATYPE         type{LUATYPE_NIL};
};

}