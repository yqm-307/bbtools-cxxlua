#pragma once
#include <bbt/core/type/type_traits.hpp>
#include <bbt/cxxlua/detail/Config.hpp>

namespace bbt::cxxlua::detail
{

template<typename T, typename Contrast>
constexpr bool RemoveCVRefAndIsSame()
{
    return std::is_same_v<bbt::type::remove_cvref_t<T>, Contrast>;
} 

/* 检测类型T是否为合法的 cxx 传送 到 lua 的类型 */
template<typename T>
constexpr bool CheckIsCanTransfromToLuaType()
{
    if constexpr(
        std::is_same_v<bbt::type::remove_cvref_t<T>, int> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, std::string> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, double> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, char*> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, const char*> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, bbt::cxxlua::detail::LuaRef> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, lua_CFunction> ||
        std::is_same_v<bbt::type::remove_cvref_t<T>, bbt::cxxlua::Nil>
    ) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
struct __ToLuaType
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_NONE; };

template<>
struct __ToLuaType<std::string>
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_CSTRING; };

template<>
struct __ToLuaType<char*>
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_CSTRING; };

template<>
struct __ToLuaType<const char*>
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_CSTRING; };

template<>
struct __ToLuaType<int>
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_NUMBER; };

template<>
struct __ToLuaType<double>
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_NUMBER; };

template<>
struct __ToLuaType<lua_CFunction>
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_FUNCTION; };

template<>
struct __ToLuaType<bbt::cxxlua::detail::LuaRef>
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_STACKREF; };

template<>
struct __ToLuaType<bbt::cxxlua::Nil>
{ static constexpr LUATYPE type = LUATYPE::LUATYPE_NIL; };

template<typename T>
inline constexpr LUATYPE luatype_v = __ToLuaType<bbt::type::remove_cvref_t<T>>::type;

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
