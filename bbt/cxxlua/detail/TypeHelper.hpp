#pragma once
#include "Config.hpp"
#include "bbt/base/type/type_traits.hpp"

namespace bbt::cxxlua::detail
{

/* 检测类型T是否为合法的 cxx 传送 到 lua 的类型 */
template<typename T>
constexpr bool CheckIsCanTransfromToLuaType()
{
    if constexpr(
        std::is_same_v<bbt::type::remove_cvref_t<T>, int> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, std::string> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, double> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, char*> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, bbt::cxxlua::detail::LuaRef> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, lua_CFunction>
    ) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
struct GetTypeEnum
{ static const LUATYPE type = LUATYPE::LUATYPE_NONE; };

template<>
struct GetTypeEnum<std::string>
{ static const LUATYPE type = LUATYPE::LUATYPE_CSTRING; };

template<>
struct GetTypeEnum<char*>
{ static const LUATYPE type = LUATYPE::LUATYPE_CSTRING; };

template<>
struct GetTypeEnum<int>
{ static const LUATYPE type = LUATYPE::LUATYPE_NUMBER; };

template<>
struct GetTypeEnum<double>
{ static const LUATYPE type = LUATYPE::LUATYPE_NUMBER; };

template<>
struct GetTypeEnum<lua_CFunction>
{ static const LUATYPE type = LUATYPE::LUATYPE_FUNCTION; };

template<>
struct GetTypeEnum<bbt::cxxlua::detail::LuaRef>
{ static const LUATYPE type = LUATYPE::LUATYPE_STACKREF; };

inline void DbgLuaStack(lua_State* l) {
    int type;
    fprintf(stderr, "-----------> top <-----------\n");
    fprintf(stderr, "索引\t类型\t值\n");
    for (int i = lua_gettop(l); i > 0; --i) {
        type = lua_type(l, i);
        fprintf(stderr, "(%d)\t%s\t%s\n", i, lua_typename(l, type), lua_tostring(l, i));
    }
    fprintf(stderr, "-----------> bottom <-----------\n");

}

} // namespace bbt::cxxlua::detail
