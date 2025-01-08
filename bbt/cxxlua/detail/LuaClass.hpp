#pragma once
#include <bbt/cxxlua/detail/ClassMgr.hpp>

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
class LuaClass:
    public LuaMeta
{
public:
    typedef std::weak_ptr<CXXClassType> ClassWeakPtr;
    struct UDataRef {
        /**
         * UDataRef是Lua Userdata对象，Lua中UData实际上
         * 通过弱引用来引用到c++对象，这样当c++对象释放时
         * 也不会导致访问到非法地址
         */
        ClassWeakPtr lua_weak_ref_cxxobj;
    };

    typedef ClassMgr::Callable Callable;
    typedef std::unordered_map<std::string, ClassMgr::Callable> FuncsMap;
    typedef typename FuncsMap::value_type FuncsMapEntry;
    typedef int (CXXClassType::*LuaClassFunc)(lua_State*);

    LuaClass();
    ~LuaClass();

    /* 注册函数 */
    static bool Register(std::shared_ptr<LuaStack>& stack);
    static Callable GenCallable(LuaClassFunc f, const std::string& name, CallType type);
    virtual bool PushMe(lua_State* l, ClassWeakPtr weak_this) final;

protected: /* 给派生类用来注册函数（lua绑定类需要关注） */
    /* 添加一些成员函数 */
    static LuaErrOpt InitFuncs(std::initializer_list<FuncsMapEntry> list);
    /* 添加可读变量 */
    static LuaErrOpt InitField(std::initializer_list<FuncsMapEntry> list);
    /* 设置类名（元表名） */
    static LuaErrOpt InitClass(const std::string& classname);

    /* 派生类需要实现此函数，用来注册lua类 */
    // static LuaErrOpt CXXLuaInit();

private: /* 自动生成的函数（使用时无需关心） */

    /* 元方法 to_string */
    static int cxx2lua_to_string(lua_State* l);
    /* 元方法 __gc */
    static int cxx2lua_destructor(lua_State* l);
    /* lua call c++ 的转发函数 */
    static int cxx2lua_call(lua_State* l);

    static int metatable_index(lua_State* l);

    static LuaRetPair<Callable*> GetCallableByName(const std::string& key);
    static int DoCallable(lua_State* l, Callable* cb);
    static int DoCCallable(lua_State* l, CXXClassType* object, Callable* cb);

    static UDataRef*    CreateRef(ClassWeakPtr weak);
    static void         ReleaseRef(UDataRef* ref);
private:
    /**
     * 特殊函数:
     * init lua类构造函数
     */
    static std::string              m_class_name;
};

template<typename T>
std::string LuaClass<T>::m_class_name{""};

} // namespace bbt::cxxlua::detail

#include <bbt/cxxlua/detail/template/__TLuaClass.hpp>