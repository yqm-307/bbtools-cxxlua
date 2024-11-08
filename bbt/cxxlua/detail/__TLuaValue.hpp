#pragma once
#include <bbt/cxxlua/detail/LuaValue.hpp>

namespace bbt::cxxlua::detail
{



template<typename T>
bool LuaValue::GetValue(T& value) const
{
    return _GetValue(value);
}

template<typename T>
bool LuaValue::SetValue(const T& value)
{
    _SetValue(value);
    return true;
}

}