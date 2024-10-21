#pragma once
#include "LuaClass.hpp"

namespace bbt::cxxlua::detail
{

template<typename CXXClassType>
LuaClass<CXXClassType>::LuaClass() {}

template<typename CXXClassType>
LuaClass<CXXClassType>::~LuaClass() {}

template<typename CXXClassType>
bool LuaClass<CXXClassType>::InitFuncs(std::initializer_list<FuncsMapEntry> list)
{
    for (auto &&pair : list) {
        auto [it, succ] = m_funcs.insert(pair);
        if (!succ) {
            m_funcs.clear();
            return false;
        }
    }
    
    return true;
}

template<typename CXXClassType>
bool LuaClass<CXXClassType>::InitClass(const std::string& classname)
{
    assert(!classname.empty());
    m_class_name = classname.c_str();
    return true;
}

template<typename CXXClassType>
bool LuaClass<CXXClassType>::InitConstructor(const ConstructFunction& constructor)
{
    assert(constructor);
    m_construct_func = constructor;
    return true;
}



template<typename CXXClassType>
bool LuaClass<CXXClassType>::Register(std::unique_ptr<LuaStack>& stack)
{
    /**
     * 下面的操作类似于lua中的（效果类似）：
     * 
     * Class = {}   -- 全局表名：classname
     * local mt = {
     *      "__metatable" = Class,
     *      "__index"     = Class,
     *      "__tostring"  = cxx2lua_to_string,
     *      "__gc"        = cxx2lua_destructor,
     * }
     * 
     * Class.new = cxx2lua_constructor
     * 
     * local Funcs = {
     *      "new" = ...,    -- 必定存在
     *      ...,            -- 派生类补充
     * }
     * 
     * for szName, fnMemFunc in pairs(Funcs) do
     *      Class[szName] = fnMemFunc
     * end
     */

    stack->NewLuaTable();
    auto opts_index_table = stack->GetTop();

    stack->NewMetatable(m_class_name.c_str());
    auto mt_table_ref = stack->GetTop();

    if (stack->SetGlobalValueByIndex(m_class_name, opts_index_table)) {
        return false;
    }

    stack->Copy2Top(mt_table_ref);
    assert(stack->Insert2Table("__metatable",   opts_index_table) == std::nullopt);
    assert(stack->Insert2Table("__index",       opts_index_table) == std::nullopt);
    assert(stack->Insert2Table("__tostring",    cxx2lua_to_string) == std::nullopt);
    assert(stack->Insert2Table("__gc",          cxx2lua_destructor) == std::nullopt);

    stack->Copy2Top(opts_index_table);
    assert(stack->Insert2Table("new",           cxx2lua_constructor) == std::nullopt);
    for (auto&& func : m_funcs) {
        auto l = stack->Context();
        lua_pushstring(l, func.first.c_str());
        lua_pushlightuserdata(l, (void*)(&func));
        lua_pushcclosure(l, cxx2lua_call, 1);
        lua_settable(l, opts_index_table.GetIndex());
    }

    stack->Pop(-2);

    return true;
}

template<typename CXXClassType>
int LuaClass<CXXClassType>::cxx2lua_call(lua_State* l)
{
    CXXClassType** userdata = static_cast<CXXClassType**>(luaL_checkudata(l, 1, m_class_name.c_str()));
    luaL_argcheck(l, userdata != nullptr, 1, "invalid user data!");
    if (userdata == nullptr) {
        return 0;
    }

    CXXClassType* obj = static_cast<CXXClassType*>(*userdata);
    void* upvalue = lua_touserdata(l, lua_upvalueindex(1));
    assert(upvalue != nullptr);

    typename FuncsMap::value_type* pair = static_cast<typename FuncsMap::value_type*>(upvalue);
    int ret = ((obj)->*(pair->second))(l);
    return ret;
}


template<typename CXXClassType>
int LuaClass<CXXClassType>::cxx2lua_to_string(lua_State* l)
{
    char buff[128];
    CXXClassType** userdata = static_cast<CXXClassType**>(luaL_checkudata(l, 1, m_class_name.c_str()));
    CXXClassType* obj = *userdata;
    sprintf(buff, "%p", (void*)obj);
    lua_pushfstring(l, "%s (%s)", m_class_name.c_str(), buff);

    return 1;
}

template<typename CXXClassType>
int LuaClass<CXXClassType>::cxx2lua_constructor(lua_State* l)
{
    CXXClassType* obj = nullptr;

    if(m_construct_func) {
        obj = m_construct_func(l);
    } else {
        obj = new CXXClassType();
    }

    /* 创建失败 */
    if (!obj) {
        lua_pushnil(l);
        return false;
    }

    /**
     * 
     *  function new()
     *      mt = classtable
     *      local new_tb = userdata
     *      setmetatable(new_tb, classtable)
     * 
     *      (可选) new_tb[key] = value //TODO 构造阶段可以设置一些只读的值
     *      return new_tb
     *  end
     */


    CXXClassType** userdata = static_cast<CXXClassType**>(lua_newuserdata(l, sizeof(CXXClassType*)));    
    *userdata = obj;

    int type = luaL_getmetatable(l, m_class_name.c_str());
    assert(type != LUATYPE::LUATYPE_NIL);
    lua_setmetatable(l, -2);

    return 1;
}

template<typename CXXClassType>
int LuaClass<CXXClassType>::cxx2lua_destructor(lua_State* l)
{
    if (luaL_getmetafield(l, 1, "do not trash")) {
        lua_pushvalue(l, 1);
        lua_gettable(l, -2);
        if (!lua_isnil(l, -1))
            return 0;
    }

    auto** userdata = static_cast<CXXClassType**>(lua_touserdata(l, 1));
    auto* obj = *userdata;
    if (obj)
        delete obj;

    return 0;
}

} // namespace bbt::cxxlua::detail