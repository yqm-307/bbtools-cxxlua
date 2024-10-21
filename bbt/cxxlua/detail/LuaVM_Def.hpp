#pragma once
#include "LuaVM.hpp"
#include <string>
#include <bbt/base/file/FileHelper.hpp>

namespace bbt::cxxlua::detail
{

template<typename ... Args>
std::optional<LuaErr> LuaVM::CallLuaFunction(
    const std::string&              funcname,
    int                             return_nums,
    const LuaParseReturnCallback&   parse_handler,
    Args                            ...args)
{
    auto [err, type] = m_stack->CheckGlobalValue<bbt::cxxlua::detail::LUATYPE::LUATYPE_FUNCTION>(funcname);
    if (err != std::nullopt)
        return err;

    auto luacall_err = m_stack->LuaCall(sizeof ...(args), return_nums, args ...);
    if (luacall_err != std::nullopt)
        return luacall_err;

    if (parse_handler)
        return parse_handler(m_stack);

    return std::nullopt;
}

template<typename CXXClass>
std::optional<LuaErr> LuaVM::RegistClass()
{ 
    return CXXClass::CXXLuaInit(m_stack);
}

template<typename ...Args>
std::optional<LuaErr> LuaVM::GetByKey4Table(LuaValue& value, const std::string& global_table, Args ...args)
{
    if (LUATYPE_LUATABLE != m_stack->GetGlobal(global_table))
        return LuaErr("[LuaVM::GetByKey4Table] not a global table!", ERRCODE::Comm_Failed);
    return m_stack->GetByKey4Table(value, args...);
}
}