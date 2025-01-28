#pragma once
#include <bbt/cxxlua/detail/LuaStack.hpp>

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
    constexpr LUATYPE tp = luatype_v<T>;
    static_assert(CheckIsCanTransfromToLuaType<T>());
    static_assert(( tp > LUATYPE::LUATYPE_NONE &&
                    tp < LUATYPE::Other),
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
LuaErrOpt LuaStack::__Insert(KeyType key, ValueType value, int table_index) const
{
    if (!__IsSafeValue(value)) {
        return LuaErr("", ERRCODE::Stack_ErrIndex);
    }

    Push(key);
    Push(value);
    lua_settable(Context(), table_index);

    return std::nullopt;
}

template<typename KeyType, typename ValueType>
LuaErrOpt LuaStack::SetTbField(KeyType key, ValueType value, int table_index) const
{
    LUATYPE top_type = GetType(-1);
    int value_type = luatype_v<ValueType>;

    /* TODO check key 合法性 */

    /* check 插入值的类型是否为合法的类型 */
    if (top_type != LUATYPE::LUATYPE_LUATABLE ||
        CXXLUAInvalidType(value_type) ||
        !CheckIsCanTransfromToLuaType<ValueType>()) 
    {
        return LuaErr("", ERRCODE::Type_UnExpected);
    }

    /* 确保类型正确，执行此操作必定成功 */
    __Insert(key, value, table_index);
    return std::nullopt;
}

template<typename TKey>
LuaRetPair<LUATYPE> LuaStack::GetTbField(int table_abs_index, const TKey& key) const
{
    return __CheckTable(table_abs_index, key);
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

template<typename TKey, typename ...Args>
std::optional<LuaErr> LuaStack::_GetByKey4Table(LuaValue& value, TKey key, Args ...args)
{
    if (LUATYPE_LUATABLE != GetType(-1))
        return LuaErr("[LuaStack::_GetByKey4Table] top value not lua table!", ERRCODE::Comm_Failed);

    // 压入key，获取table值
    Push(key);
    // 取出再尝试获取table值
    auto err = _GetByKey4Table(value, args...);
    Pop(1);

    return err;
}

template<typename TKeyValue>
std::optional<LuaErr> LuaStack::_GetByKey4Table(LuaValue& value, TKeyValue key)
{
    if (LUATYPE_LUATABLE != GetType(-1))
        return LuaErr("top value not lua table!", ERRCODE::Comm_Failed);
    
    Push(key);
    return Pop(value);
}

}