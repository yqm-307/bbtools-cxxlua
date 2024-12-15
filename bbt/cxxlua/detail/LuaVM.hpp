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
    CXXLUA_API LuaErrOpt LoadLuaLibrary();
    CXXLUA_API LuaErrOpt DoScript(const std::string& script);

    CXXLUA_API LuaErrOpt LoadFile(const std::string& file_path);
    CXXLUA_API LuaErrOpt LoadFiles(const std::vector<std::string>& file_path_arr);
    CXXLUA_API LuaErrOpt LoadFolder(const std::string& folder_path);

    CXXLUA_API std::shared_ptr<LuaStack> GetStack() const;

    template<typename ... Args>
    CXXLUA_API LuaErrOpt CallLuaFunction(
        const std::string&              funcname,
        int                             return_nums,
        const LuaParseReturnCallback&   parse_handler,
        Args                            ...args);
    
    CXXLUA_API LuaErrOpt Pop(LuaValue& value);

    template<typename ...Args>
    CXXLUA_API LuaErrOpt GetValue4Table(LuaValue& value, const std::string& global_table, Args ...args);
    
    CXXLUA_API LuaErrOpt GetGlobalValue(const std::string& value_name, LuaValue& value);

    /**
     * @brief 设置全局变量
     * 
     * @tparam TValue 变量值类型
     * @param value_name 变量名
     * @param value 变量值
     * @return LuaErrOpt 错误信息
     */
    template<typename TValue>
    CXXLUA_API LuaErrOpt SetGlobalValue(const std::string& value_name, const TValue& value);
public: /* LuaClass 相关接口 */

    /* 在lua中注册 c++ class */
    template<typename CXXClass>
    CXXLUA_API LuaErrOpt RegistClass();

    /* 表操作 */
    CXXLUA_API LuaErrOpt ExistGlobalFunc(const std::string& funcname);

    /* 执行table_ref[key] = value_ref。执行前后栈无变化 */
    CXXLUA_API LuaErrOpt Table_SetField(const LuaRef& table_ref, const std::string& key, const LuaRef& value_ref) const;
    /* 获取 table_ref[key]，  */
    CXXLUA_API LuaRetPair<LUATYPE> Table_GetField(const LuaRef& table_ref, const std::string& key) const;

private:
    std::unique_ptr<LuaVmImpl> m_impl;
};

}

#include <bbt/cxxlua/detail/template/__TLuaVM.hpp>