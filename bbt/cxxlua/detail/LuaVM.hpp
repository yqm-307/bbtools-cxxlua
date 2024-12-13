#pragma once
#include <vector>
#include <bbt/base/templateutil/Noncopyable.hpp>
#include <bbt/cxxlua/detail/LuaErr.hpp>


namespace bbt::cxxlua::detail
{

class LuaVM:
    bbt::templateutil::noncopyable
{
public:
    LuaVM();
    ~LuaVM();

    LuaVM(LuaVM&& vm);
    LuaVM& operator=(LuaVM&& vm);

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

protected: /* 表操作 */
    CXXLUA_API std::optional<LuaErr> ExistGlobalFunc(const std::string& funcname);

private:
    std::unique_ptr<LuaVmImpl> m_impl;
};

}

#include <bbt/cxxlua/detail/template/__TLuaVM.hpp>