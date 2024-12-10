#pragma once
#include <bbt/cxxlua/detail/interface/ILuaValue.hpp>
#include <bbt/cxxlua/detail/LuaErr.hpp>

namespace bbt::cxxlua::detail
{

class LuaValue:
    public ILuaValue
{
public:
    LuaValue();
    explicit LuaValue(Value& value);
    explicit LuaValue(Value&& value);
    virtual ~LuaValue();

    virtual LUATYPE GetType() const override;
    template<typename T>
    LuaErrOpt GetValue(T& value) const;
    template<typename T>
    bool SetValue(const T& value);
    virtual bool IsNil() const override;
    virtual int  StackIndex() const override;
    virtual bool ToLString(std::string& lua_str) const override;
protected:
    void _SetValue(const double& value);
    void _SetValue(const int& value);
    void _SetValue(const std::string& value);
    void _SetValue(const bool& value);
    void _SetValue(const lua_CFunction& value);
    void _SetValue(const Nil& value);
    
    LuaErrOpt _GetValue(double& value) const;
    LuaErrOpt _GetValue(int& value) const;
    LuaErrOpt _GetValue(std::string& value) const;
    LuaErrOpt _GetValue(bool& value) const;
    LuaErrOpt _GetValue(lua_CFunction& value) const;
    LuaErrOpt _GetValue(Nil& nil) const;
private:
    Value m_value;
    // LUATYPE m_type{LUATYPE::LUATYPE_NIL};
};

}

#include <bbt/cxxlua/detail/template/__TLuaValue.hpp>