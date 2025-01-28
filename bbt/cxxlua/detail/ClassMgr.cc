#include <bbt/cxxlua/detail/ClassMgr.hpp>

namespace bbt::cxxlua::detail
{

ClassMgr::~ClassMgr()
{
    for (auto&& pair : m_class_map)
    {
        delete pair.second;
    }
}


std::unique_ptr<ClassMgr>& ClassMgr::GetInstance()
{
    static std::unique_ptr<ClassMgr> _inst{nullptr};
    if (_inst == nullptr)
        _inst = std::unique_ptr<ClassMgr>(new ClassMgr());

    return _inst;
}

ClassMgr::MetaInfo* ClassMgr::GetMetaInfoAndIfNotMakeIt(const std::string& classname)
{
    MetaInfo* metainfo = nullptr;

    auto it = m_class_map.find(classname);
    if (it == m_class_map.end()) {
        metainfo = new MetaInfo();
        m_class_map.insert(std::make_pair(classname, metainfo));
    }
    else
    {
        metainfo = it->second;
    }
    
    return metainfo;
}

LuaErrOpt ClassMgr::_RegistMetaFunc(MetaInfo* meta, const std::string& classname, const FuncsMapEntry& entry)
{
    if (meta == nullptr)
        return LuaErr{"meta is null!", ERRCODE::ClassMgr_Error};

    if (meta->m_funcmap.find(entry.first) != meta->m_funcmap.end())
        return LuaErr{"class [" + classname + "] func [" + entry.first + "] is already registered!", ERRCODE::ClassMgr_Error};

    meta->m_funcmap.insert(entry);
    
    return std::nullopt;
}


bool ClassMgr::IsRegistered(const std::string& classname)
{
    return m_class_map.find(classname) != m_class_map.end();
}

LuaErrOpt ClassMgr::RegistClassFunc(const std::string& classname, const FuncsMapEntry& entry)
{
    auto* meta = GetMetaInfoAndIfNotMakeIt(classname);
    return _RegistMetaFunc(meta, classname, entry);
}

LuaErrOpt ClassMgr::RegistClassFunc(const std::string& classname, std::initializer_list<FuncsMapEntry> list)
{
    auto* meta = GetMetaInfoAndIfNotMakeIt(classname);

    for (auto&& entry : list) {    
        auto err = _RegistMetaFunc(meta, classname, entry);
        if (err)
            return err;
    }

    return std::nullopt;
}

LuaRetPair<ClassMgr::Callable*> ClassMgr::GetCallable(const std::string& classname, const std::string& key)
{
    if (!IsRegistered(classname))
        return {LuaErr{"not regist class=" + classname, ERRCODE::ClassMgr_Error}, nullptr};

    auto map = m_class_map[classname]->m_funcmap;

    auto it = map.find(key);
    if (it == map.end())
        return {LuaErr{"class [" + classname + "] no func or field [" + key + "]" , ERRCODE::ClassMgr_Error}, nullptr};

    return {std::nullopt, &(it->second)};
}

}