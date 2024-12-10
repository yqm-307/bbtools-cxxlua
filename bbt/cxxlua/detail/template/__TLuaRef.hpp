#pragma once
#include <bbt/cxxlua/detail/LuaRef.hpp>
#include <bbt/cxxlua/detail/LuaStack.hpp>

namespace bbt::cxxlua::detail
{

template<typename T>
LuaErrOpt LuaRef::GetValue(T& value)
{
    auto stack = m_stack.lock();
    if (!CheckIndex(m_stack, m_index) || !stack)
        return LuaErr{"bad index or invaild stack", ERRCODE::LuaRef_InvaildRef};

    auto [get_value_obj_err, value_obj] = stack->GetValue(m_index);
    if (get_value_obj_err)
        return get_value_obj_err;
    
    auto get_value_err2 = value_obj->GetValue(value);
    if (get_value_err2)
        return get_value_err2;

    return std::nullopt;
}

}