#pragma once
#include <set>
#include <bbt/cxxlua/detail/LuaStack.hpp>
#include <bbt/cxxlua/detail/LuaRef.hpp>


namespace bbt::cxxlua::detail
{

/**
 * @brief 为了将C++类成员函数泛化为LuaMeta::MemberFunc
 * 
 */
class LuaMeta
{
public:
    typedef int (LuaMeta::*MemberFunc)(lua_State*);
};

/**
 * lua class metainfo manager
 */
class ClassMgr
{
public:
    struct Callable
    {
        LuaMeta::MemberFunc func;
        std::string         funcname;
        CallType            type;
        /* todo: 可拓展更多信息以支持功能 */
    };

    typedef std::unordered_map<std::string, Callable> FuncsMap;
    typedef typename FuncsMap::value_type FuncsMapEntry;
public:

    ~ClassMgr();
    static std::unique_ptr<ClassMgr>& GetInstance();

    /* 获取一个Callable对象 */
    LuaRetPair<Callable*>       GetCallable(const std::string& classname, const std::string& key);

    /* 注册一个Lua函数 */
    LuaErrOpt                   RegistClassFunc(const std::string& classname, const FuncsMapEntry& list);
    LuaErrOpt                   RegistClassFunc(const std::string& classname, std::initializer_list<FuncsMapEntry> list);
    bool                        IsRegistered(const std::string& classname);

private:
    struct MetaInfo { FuncsMap m_funcmap; };
    ClassMgr() = default;

    MetaInfo*                   GetMetaInfoAndIfNotMakeIt(const std::string& classname);
    LuaErrOpt                   _RegistMetaFunc(MetaInfo* meta, const std::string& classname, const FuncsMapEntry& entry);
private:

    std::unordered_map<std::string, MetaInfo*>   m_class_map;
};

}