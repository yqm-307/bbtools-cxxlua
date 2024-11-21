#include <bbt/cxxlua/detail/LuaValue.hpp>
#include <bbt/cxxlua/detail/LuaErr.hpp>

namespace bbt::cxxlua::detail
{

LuaValue::LuaValue()
{
}

LuaValue::LuaValue(Value& value):
    m_value(value)
{
}

LuaValue::LuaValue(Value&& value):
    m_value(std::forward<Value&&>(value))
{
}

LuaValue::~LuaValue()
{
}

LUATYPE LuaValue::GetType() const
{
    return m_value.type;
}

bool LuaValue::IsNil() const
{
    return m_value.type == LUATYPE::LUATYPE_NIL;
}

int  LuaValue::StackIndex() const
{
    return -1;
}

bool LuaValue::ToLString(std::string& lua_str) const
{
    return false;
}

void LuaValue::_SetValue(const double& value)
{
    m_value.basevalue.number = value;
    m_value.type = LUATYPE::LUATYPE_NUMBER;
}

void LuaValue::_SetValue(const int& value)
{
    m_value.basevalue.number = value;
    m_value.type = LUATYPE::LUATYPE_NUMBER;
}

void LuaValue::_SetValue(const std::string& value)
{
    m_value.str = value;
    m_value.type = LUATYPE::LUATYPE_CSTRING;
}

void LuaValue::_SetValue(const bool& value)
{
    m_value.basevalue.boolean = value;
    m_value.type = LUATYPE::LUATYPE_BOOL;
}

void LuaValue::_SetValue(const lua_CFunction& value)
{
    m_value.cfunc = value;
    m_value.type = LUATYPE::LUATYPE_FUNCTION;
}

void LuaValue::_SetValue(const Nil& value)
{
    m_value.type = LUATYPE::LUATYPE_NIL;
}

LuaErrOpt LuaValue::_GetValue(double& value) const
{
    if (GetType() != LUATYPE::LUATYPE_NUMBER)
        return LuaErr{"not number", ERRCODE::Type_UnExpected};

    value = m_value.basevalue.number;
    return std::nullopt;
}

LuaErrOpt LuaValue::_GetValue(int& value) const
{
    if (GetType() != LUATYPE::LUATYPE_NUMBER)
        return LuaErr{"not number", ERRCODE::Type_UnExpected};
    
    value =  static_cast<int>(m_value.basevalue.number);
    return std::nullopt;
}

LuaErrOpt LuaValue::_GetValue(std::string& value) const
{
    if (GetType() != LUATYPE::LUATYPE_CSTRING)
        return LuaErr{"not string", ERRCODE::Type_UnExpected};

    value = m_value.str;
    return std::nullopt;
}

LuaErrOpt LuaValue::_GetValue(bool& value) const
{
    if (GetType() != LUATYPE::LUATYPE_BOOL)
        return LuaErr{"not bool", ERRCODE::Type_UnExpected};

    value = m_value.basevalue.boolean;
    return std::nullopt;
}

LuaErrOpt LuaValue::_GetValue(lua_CFunction& value) const
{
    if (GetType() != LUATYPE::LUATYPE_FUNCTION)
        return LuaErr{"not function", ERRCODE::Type_UnExpected};

    value = m_value.cfunc;
    return std::nullopt;
}

LuaErrOpt LuaValue::_GetValue(Nil& nil) const
{
    return std::nullopt;
}

}