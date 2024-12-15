#include <bbt/cxxlua/detail/LuaVM.hpp>
#include <bbt/cxxlua/detail/LuaStack.hpp>
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

LuaVM::LuaVM(LuaVM&& vm)
{
    m_impl = std::move(vm.m_impl);
}

LuaVM& LuaVM::operator=(LuaVM&& vm)
{
    m_impl = std::move(vm.m_impl);
    return *this;
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

std::shared_ptr<LuaStack> LuaVM::GetStack() const
{
    return m_impl->m_stack;
}

LuaErrOpt LuaVM::Table_SetField(const LuaRef& table_ref, const std::string& key, const LuaRef& value_ref) const
{
    return m_impl->SetTbField(table_ref, key, value_ref);
}

LuaRetPair<LUATYPE> LuaVM::Table_GetField(const LuaRef& table_ref, const std::string& key) const
{
    return m_impl->GetTbField(table_ref, key);
}

}