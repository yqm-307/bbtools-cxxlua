#pragma once
#include <bbt/cxxlua/detail/impl/LuaVMImpl.hpp>
#include <bbt/cxxlua/detail/LuaStack.hpp>
#include <bbt/cxxlua/detail/TypeHelper.hpp>

namespace bbt::cxxlua::detail
{

template<typename TValue>
std::optional<LuaErr> LuaVmImpl::Push(TValue value)
{
    /* 该函数实例化时检测TValue是否合法 */
    static_assert(detail::CheckIsCanTransfromToLuaType<TValue>());

    return m_stack->Push(value);
}

template<typename TValue>
std::optional<LuaErr> LuaVmImpl::Pop(TValue& value)
{
    /* 该函数实例化时检测TValue是否合法 */
    static_assert(detail::CheckIsCanTransfromToLuaType<TValue>());

    return m_stack->Pop(value);
}

}