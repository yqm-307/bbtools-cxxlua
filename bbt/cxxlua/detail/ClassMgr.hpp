#pragma once
#include <set>
#include <bbt/cxxlua/detail/LuaStack.hpp>
#include <bbt/cxxlua/detail/LuaRef.hpp>


namespace bbt::cxxlua::detail
{

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
        std::string funcname;
        CallType type;
    };

    typedef std::unordered_map<std::string, Callable> FuncsMap;
    typedef typename FuncsMap::value_type FuncsMapEntry;
public:

    ~ClassMgr();
    static std::unique_ptr<ClassMgr>& GetInstance();

    LuaRetPair<Callable*> GetCallable(const std::string& classname, const std::string& key);

    LuaErrOpt RegistClassFunc(const std::string& classname, const FuncsMapEntry& list);
    LuaErrOpt RegistClassFunc(const std::string& classname, std::initializer_list<FuncsMapEntry> list);

    bool IsRegistered(const std::string& classname);
private:
    struct MetaInfo {
        FuncsMap           m_funcmap;
    };
    ClassMgr() = default;

    MetaInfo* GetMetaInfoAndIfNotMakeIt(const std::string& classname);
    LuaErrOpt _RegistMetaFunc(MetaInfo* meta, const std::string& classname, const FuncsMapEntry& entry);
private:

    std::unordered_map<std::string, MetaInfo*>   m_class_map;
};

}