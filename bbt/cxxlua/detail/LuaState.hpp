#pragma once
#include "LuaStateBase.hpp"

namespace bbt::cxxlua::detail
{

class LuaVM;

class LuaState:
    public LuaStateBase
{
    friend class LuaVM;
public:

protected:

};

} // namespace bbt::cxxlua::detail