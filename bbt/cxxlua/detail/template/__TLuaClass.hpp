#pragma once
#include <bbt/cxxlua/detail/LuaClass.hpp>

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
bool LuaClass<CXXClassType>::InitField(std::initializer_list<FuncsMapEntry> list)
{
    for (auto&& pair : list) {
        auto [it, succ] = m_readonly_field_funcs.insert(pair);
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
bool LuaClass<CXXClassType>::Register(std::shared_ptr<LuaStack>& stack)
{
    stack->NewMetatable(m_class_name.c_str());
    auto mt_table_ref = stack->GetTop();

    assert(stack->SetTbField("__index",       metatable_index) == std::nullopt);
    assert(stack->SetTbField("__tostring",    cxx2lua_to_string) == std::nullopt);
    assert(stack->SetTbField("__gc",          cxx2lua_destructor) == std::nullopt);

    stack->Pop(-2);

    return true;
}

template<typename CXXClassType>
typename LuaClass<CXXClassType>::Callable LuaClass<CXXClassType>::GenCallable(MemberFunc f, const std::string& name, CallType type)
{
    Callable obj;
    obj.func = f;
    obj.funcname = name;
    obj.type = type;

    return obj;
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

    Callable* callobj = static_cast<Callable*>(upvalue);
    int ret = ((obj)->*(callobj->func))(l);
    return ret;
}

template<typename CXXClassType>
int LuaClass<CXXClassType>::metatable_index(lua_State* l)
{
    const char* key = lua_tostring(l, -2);
    assert(key != nullptr);
    Callable* callable = GetCallableByName(key);
    if (!callable) {
        lua_pushnil(l);
        return 1;
    }

    switch (callable->type)
    {
    // 如果是访问函数，返回函数
    case emCallType_MemberFunc:
        lua_pushlightuserdata(l, (void*)(callable));
        lua_pushcclosure(l, cxx2lua_call, 1);
        break;
    // 如果是访问变量，执行函数
    case emCallType_ReadonlyFunc:
        cxx2lua_call(l);
        break;
    default:
        lua_pushnil(l);
        break;
    }

    return 1;

}

template<typename CXXClassType>
typename LuaClass<CXXClassType>::Callable* LuaClass<CXXClassType>::GetCallableByName(const std::string& key)
{
    auto member_func_itor = m_funcs.find(key);
    if (member_func_itor != m_funcs.end())
        return &(member_func_itor->second);
    
    auto field_func_itor = m_readonly_field_funcs.find(key);
    if (field_func_itor != m_funcs.end())
        return &(field_func_itor->second);

    return nullptr;
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