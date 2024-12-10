#pragma once
#include "LuaErr.hpp"
#include "LuaStack.hpp"
#include <vector>


namespace bbt::cxxlua::detail
{

class LuaVM
{
public:
    LuaVM(LuaVM&& vm);
    LuaVM(const LuaVM&& vm);
    LuaVM();
    ~LuaVM();

    LuaVM& operator=(LuaVM&& vm);
    LuaVM& operator=(const LuaVM&& vm);

public: /* high level api */

    /* 加载 lua 库函数 */
    CXXLUA_API std::optional<LuaErr> LoadLuaLibrary();
    CXXLUA_API std::optional<LuaErr> DoScript(const std::string& script);

    CXXLUA_API std::optional<LuaErr> LoadFile(const std::string& file_path);
    CXXLUA_API std::optional<LuaErr> LoadFiles(const std::vector<std::string>& file_path_arr);
    CXXLUA_API std::optional<LuaErr> LoadFolder(const std::string& folder_path);

    CXXLUA_API std::shared_ptr<LuaStack> GetStack() const;

    template<typename ... Args>
    CXXLUA_API std::optional<LuaErr> CallLuaFunction(
        const std::string&              funcname,
        int                             return_nums,
        const LuaParseReturnCallback&   parse_handler,
        Args                            ...args);
    
    CXXLUA_API std::optional<LuaErr> Pop(LuaValue& value);

    template<typename ...Args>
    CXXLUA_API std::optional<LuaErr> GetValue4Table(LuaValue& value, const std::string& global_table, Args ...args);
    
    CXXLUA_API std::optional<LuaErr> GetGlobalValue(const std::string& value_name, LuaValue& value);

    /**
     * @brief 设置全局变量
     * 
     * @tparam TValue 变量值类型
     * @param value_name 变量名
     * @param value 变量值
     * @return std::optional<LuaErr> 错误信息
     */
    template<typename TValue>
    CXXLUA_API std::optional<LuaErr> SetGlobalValue(const std::string& value_name, const TValue& value);
public: /* LuaClass 相关接口 */

    /* 在lua中注册 c++ class */
    template<typename CXXClass>
    CXXLUA_API std::optional<LuaErr> RegistClass();
public: /* LuaTableHelper 相关接口 */

    CXXLUA_API LABEL_LUATABLE std::optional<LuaErr> RegistATableTemplate(std::shared_ptr<LuaTableHelper> table);

    /* 将一个已经存在的表，push到lua vm的全局表中，并以global_table_name命名 */
    CXXLUA_API LABEL_LUATABLE std::optional<LuaErr> PushAGlobalTableByName(
        const std::string& table_template_name,
        const std::string& global_table_name);

protected: /* 表操作 */
    CXXLUA_API std::optional<LuaErr> ExistGlobalFunc(const std::string& funcname);

private:
    std::unique_ptr<LuaVmImpl> m_impl;
};

}

#include <bbt/cxxlua/detail/template/__TLuaVM.hpp>