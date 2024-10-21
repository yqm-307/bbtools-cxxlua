#pragma once
#include "LuaStack.hpp"

namespace bbt::cxxlua::detail
{


//XXX 目前的处理可能比较粗糙，因为直接使用了lua api，如果有时间，可以使用封装的LuaStack来操作
/**
 * @brief 需要再lua中注册的c++类，需要继承自此类来确保可以
 *  将自身的字段绑定到lua中，具体使用看example的luaclass.cc例程
 * 
 *  ps：此类不会被创建对象
 */
template<typename CXXClassType /*派生类类型*/>
class LuaClass
{
public:
    typedef int(CXXClassType::*MemberFunc)(lua_State*);
    typedef std::unordered_map<std::string, MemberFunc> FuncsMap;
    typedef typename FuncsMap::value_type FuncsMapEntry;
    typedef std::function<CXXClassType*(lua_State*)> ConstructFunction;

    LuaClass();
    ~LuaClass();

protected: /* 给派生类用来注册函数（lua绑定类需要关注） */
    /* 添加一些成员函数 */
    static bool InitFuncs(std::initializer_list<FuncsMapEntry> list);
    /* 设置类名（元表名） */
    static bool InitClass(const std::string& classname);
    /* 设置c++中初始化的构造函数（lua中的class.new(...)，需要处理参数) */
    static bool InitConstructor(const ConstructFunction& constructor);
    /* 注册函数 */
    static bool Register(std::unique_ptr<LuaStack>& stack);

    /* 派生类需要实现此函数，用来注册lua类 */
    // static void CXXLuaInit(std::unique_ptr<bbt::cxxlua::detail::LuaStack>& stack);

private: /* 自动生成的函数（使用时无需关心） */

    /* 元方法 to_string */
    static int cxx2lua_to_string(lua_State* l);
    /* 元方法 __gc */
    static int cxx2lua_destructor(lua_State* l);
    /* new 函数 */
    static int cxx2lua_constructor(lua_State* l);
    /* lua call c++ 的转发函数 */
    static int cxx2lua_call(lua_State* l);
private:
    /**
     * 特殊函数:
     * init lua类构造函数
     */
    static FuncsMap                 m_funcs;
    static ConstructFunction        m_construct_func;
    static std::string              m_class_name;
};

template<typename T>
std::string LuaClass<T>::m_class_name{""};

template<typename T>
typename LuaClass<T>::ConstructFunction LuaClass<T>::m_construct_func = nullptr;

template<typename T>
typename LuaClass<T>::FuncsMap LuaClass<T>::m_funcs{};


} // namespace bbt::cxxlua::detail

#include "./LuaClass_Def.hpp"