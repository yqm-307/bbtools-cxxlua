#pragma once
#include <string>
#include <bbt/base/file/FileHelper.hpp>
#include <bbt/cxxlua/detail/impl/LuaVMImpl.hpp>

namespace bbt::cxxlua::detail
{

template<typename ... Args>
std::optional<LuaErr> LuaVM::CallLuaFunction(
    const std::string&              funcname,
    int                             return_nums,
    const LuaParseReturnCallback&   parse_handler,
    Args                            ...args)
{
    auto [err, type] = m_impl->m_stack->CheckGlobalValue<bbt::cxxlua::LUATYPE::LUATYPE_FUNCTION>(funcname);
    if (err != std::nullopt)
        return err;

    auto luacall_err = m_impl->m_stack->LuaCall(sizeof ...(args), return_nums, args ...);
    if (luacall_err != std::nullopt)
        return luacall_err;

    if (parse_handler)
        return parse_handler(m_impl->m_stack);

    return std::nullopt;
}

template<typename CXXClass>
std::optional<LuaErr> LuaVM::RegistClass()
{ 
    return CXXClass::CXXLuaInit(m_impl->m_stack);
}

template<typename ...Args>
std::optional<LuaErr> LuaVM::GetValue4Table(LuaValue& value, const std::string& global_table, Args ...args)
{
    if (LUATYPE_LUATABLE != m_impl->m_stack->GetGlobal(global_table))
        return LuaErr("[LuaVM::GetValue4Table] not a global table!", ERRCODE::Comm_Failed);
    return m_impl->m_stack->GetValue4Table(value, args...);
}

template<typename TValue>
std::optional<LuaErr> LuaVM::SetGlobalValue(const std::string& value_name, const TValue& value)
{
    return m_impl->m_stack->SetGlobalValue(value_name, value);
}

}