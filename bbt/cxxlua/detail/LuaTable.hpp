#pragma once
#include "Config.hpp"

namespace bbt::cxxlua::detail
{

/**
 * @brief 完全独立于lua vm，此类一旦实例化并初始化后
 * 应该可以注册到任意的lua vm中，不过目前只支持简单的
 * 字段。如果后续需要table嵌套等需求，可能需要以后再
 * 考虑新的方案。
 * 
 */
class LuaTable:
    std::enable_shared_from_this<LuaTable>
{
    friend class LuaStack;
public:
    /* lua表的字段集合 */
    typedef std::unordered_map<std::string, LUATYPE> FieldSet;
    /* lua表的函数集合，因为已经有LuaClass所以这里只支持c function */
    typedef std::unordered_map<std::string, lua_CFunction> FuncSet;
    typedef std::function<std::optional<LuaErr>(std::unique_ptr<LuaStack>)> TableInitFunction;

public:
    LuaTable() {}
    explicit LuaTable(const LuaTable& other) { CopyFrom(&other); }
    ~LuaTable() {}

    /* 用table覆盖自己 */
    void CopyFrom(const LuaTable* table)
    {
        m_cfunction_set = table->m_cfunction_set;
        m_field_set = table->m_field_set;
        m_table_init_func = m_table_init_func;
        m_table_name = m_table_name;
    }

    /* 注册table初始化函数，调用此函数时，保证栈顶元素是new的table */
    std::optional<LuaErr> RegistFieldInitFunc(const TableInitFunction& table_init_func)
    {
        m_table_init_func = table_init_func;
        return std::nullopt;
    }

protected:
    std::optional<LuaErr> InitFields(const FieldSet& fields)
    {
        m_field_set = fields;
        return std::nullopt;
    }

    std::optional<LuaErr> InitCFunctions(const FuncSet& funcs)
    {
        m_cfunction_set = funcs;
        return std::nullopt;
    }

    std::optional<LuaErr> InitTableName(const std::string& name)
    {
        m_table_name = name;
        return std::nullopt;
    }
private:
    FieldSet        m_field_set{};
    /* m_cfunction_set 是 m_field_set 的子集，用来寻找调用函数 */
    FuncSet         m_cfunction_set{};
    TableInitFunction m_table_init_func{nullptr};
    std::string     m_table_name{""};
};

} // namespace bbt::cxxlua::detail