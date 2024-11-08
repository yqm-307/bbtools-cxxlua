#pragma once
#include <bbt/cxxlua/detail/Config.hpp>
#include <bbt/cxxlua/detail/TypeHelper.hpp>

namespace bbt::cxxlua::detail
{

union BaseValue
{
    bool    boolean;
    double  number;
};

struct Value
{
    BaseValue       basevalue;
    lua_CFunction   cfunc{nullptr};
    std::string     str{""};
    LUATYPE         type{LUATYPE::LUATYPE_NIL};
};

class ILuaValue
{
public:
    virtual LUATYPE GetType() const = 0;
    template<typename T>
    bool GetValue(T& value) const { AssertWithInfo(false, "please rewrite!"); return false; };
    template<typename T>
    bool SetValue(const T& value) { AssertWithInfo(false, "please rewrite!"); return false; };
    virtual bool IsNil() const = 0;
    virtual int  StackIndex() const = 0;

    /**
     * @brief 如果此值有__tostring元方法，调用元方法并返回值
     */
    virtual bool ToLString(std::string& lua_str) const = 0;
};

}