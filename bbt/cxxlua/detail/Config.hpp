#pragma once

#include <lua.hpp>
#include <cassert>
#include <memory>
#include <functional>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <string>

#include <bbt/core/Attribute.hpp>
#include <bbt/core/util/Assert.hpp>

// 对外 api flag
#define CXXLUA_API BBTATTR_FUNC_RetVal
// lua table 相关（标签，用于标识函数）
#define LABEL_LUATABLE 
// lua 低级api（标签，用于标识函数）
#define LOW_LEVEL

#define CXXLUAInvalidType(type) \
    ( type <= bbt::cxxlua::LUATYPE::LUATYPE_NONE  || \
      type >= bbt::cxxlua::LUATYPE::Other)


namespace bbt::cxxlua
{

struct Nil {};
BBTATTR_COMM_Unused static Nil nil;

enum LUATYPE
{
    LUATYPE_NONE        = LUA_TNONE,
    LUATYPE_NIL         = LUA_TNIL,
    LUATYPE_BOOL        = LUA_TBOOLEAN,
    LUATYPE_LIGHTUSERDATA = LUA_TLIGHTUSERDATA,
    LUATYPE_NUMBER      = LUA_TNUMBER,
    LUATYPE_CSTRING     = LUA_TSTRING,
    LUATYPE_LUATABLE    = LUA_TTABLE,
    LUATYPE_FUNCTION    = LUA_TFUNCTION,
    LUATYPE_USERDATA    = LUA_TUSERDATA,
    LUATYPE_THREAD      = LUA_TTHREAD,
    /* cxxlua 内置类型 */
    LUATYPE_STACKREF      = LUATYPE_THREAD + 1, // 栈元素引用
    Other,
};

enum CallType {
    emCallType_MemberFunc,
    emCallType_ReadonlyFunc,
};

namespace detail
{

class LuaErr;
class LuaState;
class LuaVM;
class LuaVmImpl;
class LuaStack;
class LuaValue;
class LuaRef;

typedef std::optional<LuaErr>   LuaErrOpt; 
typedef std::optional<LuaValue> LuaValueOpt;
typedef std::optional<LuaRef>   LuaRefOpt;

template<typename TParam>
using LuaRetPair = std::pair<LuaErrOpt, TParam>;

typedef std::function<void(std::shared_ptr<LuaState>&)> LuaFunction;
/* cxx 调用 lua ，lua 返回值解析函数 */
typedef std::function<std::optional<LuaErr>(std::shared_ptr<LuaStack>&)> LuaParseReturnCallback;


} // namespace bbt::cxxlua::detail
} // namespace bbt::cxxlua