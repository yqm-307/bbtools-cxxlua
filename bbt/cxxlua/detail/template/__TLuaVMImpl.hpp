#pragma once
#include <bbt/cxxlua/detail/LuaStack.hpp>
#include <bbt/cxxlua/detail/impl/LuaVMImpl.hpp>

namespace bbt::cxxlua::detail
{

template<typename TValue>
LuaErrOpt LuaVmImpl::Push(TValue value)
{
    /* 该函数实例化时检测TValue是否合法 */
    static_assert(detail::CheckIsCanTransfromToLuaType<TValue>(), "invalid type value!");
    return m_stack->Push(value);
}

template<typename TValue>
LuaErrOpt LuaVmImpl::Pop(TValue& value)
{
    /* 该函数实例化时检测TValue是否合法 */
    static_assert(detail::CheckIsCanTransfromToLuaType<TValue>(), "invalid type value!");
    return m_stack->Pop(value);
}

}