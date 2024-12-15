#pragma once
#include <bbt/cxxlua/detail/LuaStack.hpp>
#include <bbt/cxxlua/detail/LuaRef.hpp>
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

template<typename TKey>
LuaErrOpt LuaVmImpl::SetTbField(const LuaRef& table_ref, const TKey& key, const LuaRef& value_ref) const
{
    if (!table_ref || !value_ref)
        return LuaErr{!table_ref ? "[LuaVmImpl::SetTbField] bad table ref!" : "[LuaVmImpl::SetTbField] bad value ref!", ERRCODE::LuaRef_InvaildRef};
    
    if (GetType(table_ref) != LUATYPE::LUATYPE_LUATABLE)
        return LuaErr{"[LuaVmImpl::SetTbField] can`t set field to non-table value!", ERRCODE::Type_UnExpected};

    return m_stack->SetTbField(key, value_ref, table_ref.GetAbsIndex());
}

template<typename TKey>
LuaRetPair<LUATYPE> LuaVmImpl::GetTbField(const LuaRef& table_ref, const TKey& key) const
{
    if (!table_ref)
        return {LuaErr{"[LuaVmImpl::GetTbField] bad table ref!", ERRCODE::LuaRef_InvaildRef}, LUATYPE_NONE};
    
    if (GetType(table_ref) != LUATYPE::LUATYPE_LUATABLE)
        return {LuaErr{"[LuaVmImpl::GetTbField] bad table ref!", ERRCODE::Type_UnExpected}, LUATYPE_NONE};
    
    return m_stack->GetTbField(table_ref.GetAbsIndex(), key);
}

}