#pragma once
#include <bbt/cxxlua/detail/Config.hpp>

namespace bbt::cxxlua::detail
{

class LuaVmImpl
{
public:
    LuaVmImpl();
    ~LuaVmImpl();

    LuaVmImpl(LuaVmImpl&& impl);
    LuaVmImpl(const LuaVmImpl& impl);
    LuaVmImpl& operator=(LuaVmImpl&& impl);
    LuaVmImpl& operator=(const LuaVmImpl& impl);

    LUATYPE                 GetType(int index) const;
    LUATYPE                 GetType(const LuaRef& ref) const;
    int                     ToAbsIndex(int index);
    int                     StackSize();

    template<typename TValue>
    LuaErrOpt               Push(TValue value);

    template<typename TValue>
    LuaErrOpt               Pop(TValue& value);

    /**
     * @brief 获取栈上index位置的 LuaRef 对象
     * 
     * @param index 栈上元素位置，和lua接口传递index含义相同
     * @return LuaRef 对象引用，若index非法，返回的LuaRef指向nil
     */
    LuaRetPair<LuaRefOpt>   GetRef(int index);

    void                    NewTable() const;
    LuaErrOpt               NewMetatable(const std::string& metatable_name) const;
    LuaErrOpt               SetMetatable(const LuaRef& obj, const std::string& metatable_name) const;
    void                    SetMetatable(const LuaRef& table , const LuaRef& metatable);

    template<typename TKey>
    LuaErrOpt               SetTbField(const LuaRef& table_ref, const TKey& key, const LuaRef& value_ref) const;
    template<typename TKey>
    LuaRetPair<LUATYPE>     GetTbField(const LuaRef& table_ref, const TKey& key) const;


    // LuaErrOpt               SetGlobal(const std::string& name, const LuaRef& ref);
    // LuaRetPair<LuaRefOpt>   GetGlobal(const std::string& name);


    std::shared_ptr<LuaStack> m_stack;
};

}

#include <bbt/cxxlua/detail/template/__TLuaVMImpl.hpp>