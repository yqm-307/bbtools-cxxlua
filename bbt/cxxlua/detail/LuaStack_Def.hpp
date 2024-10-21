#pragma once
#include "LuaStack.hpp"

namespace bbt::cxxlua::detail
{

template<LUATYPE LuaType>
std::pair<std::optional<LuaErr>, LUATYPE> LuaStack::CheckGlobalValue(const std::string& value_name)
{
    static_assert(( LuaType > LUATYPE::LUATYPE_NONE &&
                    LuaType < LUATYPE::Other && 
                    LuaType != LUATYPE::LUATYPE_NIL),
    "TValue LuaType is not a right type.");

    LUATYPE type = __GetGlobalValue(value_name);
    if(type == LUATYPE::LUATYPE_NONE) {
        return {LuaErr("", ERRCODE::VM_ErrParams), LUATYPE::LUATYPE_NONE};
    }else if (type != LuaType) {
        return {LuaErr("", ERRCODE::Type_UnExpected), LUATYPE::LUATYPE_NONE};
    }

    return {std::nullopt, (LUATYPE)type};  
}

template<typename T>
std::optional<LuaErr> LuaStack::SetGlobalValue(const std::string& value_name, T value)
{
    /* 类型检测 */
    constexpr LUATYPE tp = GetTypeEnum<T>::type;
    static_assert(CheckIsCanTransfromToLuaType<T>());
    static_assert(( tp > LUATYPE::LUATYPE_NONE &&
                    tp < LUATYPE::Other && 
                    tp != LUATYPE::LUATYPE_NIL
                    ),
    "TValue LuaType is not a right type.");

    if(Push(value) != tp) {
        return LuaErr("", ERRCODE::Type_UnExpected);
    }

    return __SetGlobalValue(value_name);
}

template<typename T, typename ... Args>
void LuaStack::PushMany(T arg, Args ...args)
{
    Push(arg);
    PushMany(args...);
}


template<typename KeyType, typename ValueType>
std::optional<LuaErr> LuaStack::__Insert(KeyType key, ValueType value) 
{
    if (!__IsSafeValue(value)) {
        return LuaErr("", ERRCODE::Stack_ErrIndex);
    }

    Push(key);
    Push(value);
    lua_settable(Context(), -3);

    return std::nullopt;
}

template<typename KeyType, typename ValueType>
std::optional<LuaErr> LuaStack::Insert2Table(KeyType key, ValueType value)
{
    int top_type = lua_type(Context(), -1);
    int value_type = GetTypeEnum<bbt::type::remove_cvref_t<ValueType>>::type;

    /* check 插入值的类型是否为合法的类型 */
    if (top_type != LUATYPE::LUATYPE_LUATABLE ||
        CXXLUAInvalidType(value_type) ||
        !CheckIsCanTransfromToLuaType<ValueType>()) 
    {
        return LuaErr("", ERRCODE::Type_UnExpected);
    }

    /* 确保类型正确，执行此操作必定成功 */
    __Insert(key, value);
    return std::nullopt;
}

template<typename ... Args>
std::optional<LuaErr> LuaStack::LuaCall(
    int                 nparam,
    int                 nresult,
    Args                ...args)
{
    return __CallLuaFunction(nparam, nresult, args ...);
}

template<typename ... Args>
std::optional<LuaErr> LuaStack::__CallLuaFunction(int params, int returns, Args... args)
{
    /* 递归插入所有值 */
    PushMany(args...);

    int ret = lua_pcall(Context(), params, returns, 0);
    switch (ret)
    {
    case LUA_OK:
        break;    
    case LUA_ERRRUN:
        return LuaErr(lua_tostring(Context(), -1), ERRCODE::VM_ErrLuaRuntime);
    default:
        return LuaErr(std::to_string(ret), ERRCODE::Default);
    }

    return std::nullopt;
}

}