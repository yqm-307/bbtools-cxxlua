#pragma once
#include "LuaErr.hpp"
#include "LuaStack.hpp"
#include <vector>


namespace bbt::cxxlua::detail
{

class LuaVM
{
public:
    LuaVM(): m_stack(std::make_unique<LuaStack>(luaL_newstate())) {}
    ~LuaVM() {}

public: /* high level api */

    /* 加载 lua 库函数 */
    CXXLUA_API std::optional<LuaErr> LoadLuaLibrary() { return m_stack->LoadLuaLib(); }
    CXXLUA_API std::optional<LuaErr> DoScript(const std::string& script) { return m_stack->DoScript(script); }
    // CXXLUA_API std::optional<LuaErr> LoadScript(const std::string& script);

    CXXLUA_API std::optional<LuaErr> LoadFile(const std::string& file_path) { return m_stack->LoadFile(file_path); }
    CXXLUA_API std::optional<LuaErr> LoadFiles(const std::vector<std::string>& file_path_arr)
    {
        for(auto&& path : file_path_arr) {
            auto err = LoadFile(path);
            if(!err)
                return err;
        }

        return std::nullopt;
    }

    CXXLUA_API std::optional<LuaErr> LoadFolder(const std::string& folder_path) { return m_stack->LoadFolder(folder_path); }

    template<typename ... Args>
    CXXLUA_API std::optional<LuaErr> CallLuaFunction(
        const std::string&              funcname,
        int                             return_nums,
        const LuaParseReturnCallback&   parse_handler,
        Args                            ...args);
    
    CXXLUA_API std::optional<LuaErr> Pop(LuaValue& value)
    {
        return m_stack->Pop(value);
    }

    template<typename ...Args>
    CXXLUA_API std::optional<LuaErr> GetByKey4Table(LuaValue& value, const std::string& global_table, Args ...args);
    
    CXXLUA_API std::optional<LuaErr> GetGlobalValue(LuaValue& value, const std::string& global_value)
    {
        m_stack->GetGlobal(global_value);
        return Pop(value);
    }

public: /* LuaClass 相关接口 */

    /* 在lua中注册 c++ class */
    template<typename CXXClass>
    CXXLUA_API std::optional<LuaErr> RegistClass();
public: /* LuaTable 相关接口 */

    CXXLUA_API LABEL_LUATABLE std::optional<LuaErr> RegistATableTemplate(std::shared_ptr<LuaTable> table)
    {
        if (!table) {
            return LuaErr("table is null!", ERRCODE::Comm_Failed);
        }

        return m_stack->RegistLuaTable(table);
    }

    /* 将一个已经存在的表，push到lua vm的全局表中，并以global_table_name命名 */
    CXXLUA_API LABEL_LUATABLE std::optional<LuaErr> PushAGlobalTableByName(
        const std::string& table_template_name,
        const std::string& global_table_name)
    {
        auto err = m_stack->Push2GlobalByName(table_template_name, global_table_name);

        if (err != std::nullopt) {
            return err;
        }

        return std::nullopt;
    }

public: /* low level api */
    
    
    template<typename KeyType>
    CXXLUA_API LOW_LEVEL std::optional<LuaErr> Insert2Table();
protected: /* 表操作 */
    CXXLUA_API std::optional<LuaErr> ExistGlobalFunc(const std::string& funcname);

protected: /* 函数操作 */
private:
    std::unique_ptr<LuaStack> m_stack;
};

}

#include "./LuaVM_Def.hpp"