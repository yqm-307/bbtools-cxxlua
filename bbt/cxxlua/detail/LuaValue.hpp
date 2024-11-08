#pragma once
#include <bbt/cxxlua/detail/interface/ILuaValue.hpp>

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
    bool GetValue(T& value) const;
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
    
    bool _GetValue(double& value) const;
    bool _GetValue(int& value) const;
    bool _GetValue(std::string& value) const;
    bool _GetValue(bool& value) const;
    bool _GetValue(lua_CFunction& value) const;
    bool _GetValue(Nil& nil) const;
private:
    Value m_value;
    // LUATYPE m_type{LUATYPE::LUATYPE_NIL};
};

}

#include <bbt/cxxlua/detail/__TLuaValue.hpp>