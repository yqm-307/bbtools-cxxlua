#include <bbt/cxxlua/detail/LuaVM.hpp>
#include <bbt/cxxlua/detail/LuaRef.hpp>
#include <bbt/cxxlua/detail/impl/LuaVMImpl.hpp>

namespace bbt::cxxlua::detail
{

LuaVM::LuaVM()
    :m_impl(new LuaVmImpl())
{
}

LuaVM::~LuaVM()
{
}

std::optional<LuaErr> LuaVM::LoadLuaLibrary()
{
    return m_impl->m_stack->LoadLuaLib();
}

std::optional<LuaErr> LuaVM::DoScript(const std::string& script)
{ 
    return m_impl->m_stack->DoScript(script);
}

std::optional<LuaErr> LuaVM::LoadFile(const std::string& file_path)
{
    return m_impl->m_stack->LoadFile(file_path);
}

std::optional<LuaErr> LuaVM::LoadFiles(const std::vector<std::string>& file_path_arr)
{
    for(auto&& path : file_path_arr) {
        auto err = LoadFile(path);
        if(!err)
            return err;
    }

    return std::nullopt;
}

std::optional<LuaErr> LuaVM::LoadFolder(const std::string& folder_path)
{
    return m_impl->m_stack->LoadFolder(folder_path);
}

std::optional<LuaErr> LuaVM::Pop(LuaValue& value)
{
    return m_impl->m_stack->Pop(value);
}

std::optional<LuaErr> LuaVM::GetGlobalValue(const std::string& global_value, LuaValue& value)
{
    m_impl->m_stack->GetGlobal(global_value);
    return Pop(value);
}

std::optional<LuaErr> LuaVM::RegistATableTemplate(std::shared_ptr<LuaTableHelper> table)
{
    if (!table) {
        return LuaErr("table is null!", ERRCODE::Comm_Failed);
    }

    return m_impl->m_stack->RegistLuaTable(table);
}

std::optional<LuaErr> LuaVM::PushAGlobalTableByName(
    const std::string& table_template_name,
    const std::string& global_table_name)
{
    auto err = m_impl->m_stack->Push2GlobalByName(table_template_name, global_table_name);

    if (err != std::nullopt) {
        return err;
    }

    return std::nullopt;
}


}