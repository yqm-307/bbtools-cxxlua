#pragma once
#include <bbt/cxxlua/detail/LuaClass.hpp>

namespace bbt::cxxlua::detail
{

template<typename CXXClassType>
LuaClass<CXXClassType>::LuaClass() {}

template<typename CXXClassType>
LuaClass<CXXClassType>::~LuaClass() {}

template<typename CXXClassType>
LuaErrOpt LuaClass<CXXClassType>::InitFuncs(std::initializer_list<FuncsMapEntry> list)
{
    return ClassMgr::GetInstance()->RegistClassFunc(m_class_name, list);
}

template<typename CXXClassType>
LuaErrOpt LuaClass<CXXClassType>::InitField(std::initializer_list<FuncsMapEntry> list)
{
    return ClassMgr::GetInstance()->RegistClassFunc(m_class_name, list);
}

template<typename CXXClassType>
LuaErrOpt LuaClass<CXXClassType>::InitClass(const std::string& classname)
{
    assert(!classname.empty());
    m_class_name = classname.c_str();
    return std::nullopt;
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
typename LuaClass<CXXClassType>::Callable LuaClass<CXXClassType>::GenCallable(LuaClassFunc f, const std::string& name, CallType type)
{
    Callable obj;
    obj.func = (LuaMeta::MemberFunc)f;
    obj.funcname = name;
    obj.type = type;

    return obj;
}

template<typename CXXClassType>
bool LuaClass<CXXClassType>::PushMe(lua_State* l)
{
    if (!this) {
        lua_pushnil(l);
        return false;
    }

    CXXClassType** userdata = static_cast<CXXClassType**>(lua_newuserdata(l, sizeof(CXXClassType**)));
    *userdata = dynamic_cast<CXXClassType*>(this);

    int type = luaL_getmetatable(l, m_class_name.c_str());
    assert(type != LUATYPE::LUATYPE_NIL);
    lua_setmetatable(l, -2);

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

    Callable* callobj = static_cast<Callable*>(upvalue);
    return DoCCallable(l, obj, callobj);
}

template<typename CXXClassType>
int LuaClass<CXXClassType>::metatable_index(lua_State* l)
{
    int ret = 0;
    const char* key = lua_tostring(l, -1);
    
    assert(key != nullptr);

    auto [err, callable] = GetCallableByName(key);
    if (!callable) {
        lua_pushnil(l);
        return 1;
    }

    switch (callable->type)
    {
    // 如果是访问函数，返回函数
    case CallType::emCallType_MemberFunc:
        lua_pushlightuserdata(l, (void*)(callable));
        lua_pushcclosure(l, cxx2lua_call, 1);
        ret = 1;
        break;
    // 如果是访问变量，执行函数
    case CallType::emCallType_ReadonlyFunc:
        ret = DoCallable(l, callable);
        if (ret < 1) lua_pushnil(l);
        break;
    default:
        lua_pushnil(l);
        break;
    }

    return ret;
}

template<typename CXXClassType>
LuaRetPair<ClassMgr::Callable*> LuaClass<CXXClassType>::GetCallableByName(const std::string& key)
{
    return ClassMgr::GetInstance()->GetCallable(m_class_name, key);
}

template<typename CXXClassType>
int LuaClass<CXXClassType>::DoCallable(lua_State* l, Callable* cb)
{
    CXXClassType** userdata = static_cast<CXXClassType**>(luaL_checkudata(l, 1, m_class_name.c_str()));
    luaL_argcheck(l, userdata != nullptr, 1, "invalid userdata!");
    if (userdata == nullptr)
        return 0;
    
    return DoCCallable(l, *userdata, cb);
}

template<typename CXXClassType>
int LuaClass<CXXClassType>::DoCCallable(lua_State* l, CXXClassType* object, Callable* cb)
{
    auto fnmem = static_cast<MemberFunc>(cb->func);
    return ((object)->*(cb->func))(l);
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
int LuaClass<CXXClassType>::cxx2lua_destructor(lua_State* l)
{
    if (luaL_getmetafield(l, 1, "do not trash")) {
        lua_pushvalue(l, 1);
        lua_gettable(l, -2);
        if (!lua_isnil(l, -1))
            return 0;
    }

    // auto** userdata = static_cast<CXXClassType**>(lua_touserdata(l, 1));
    // auto* obj = *userdata;
    // if (obj)
    //     delete obj;

    return 0;
}

} // namespace bbt::cxxlua::detail