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

    template<typename TValue>
    std::optional<LuaErr>   Push(TValue value);

    template<typename TValue>
    std::optional<LuaErr>   Pop(TValue& value);

    // CXXLUA_API LOW_LEVEL std::optional<LuaErr> GetTopType(LuaRef);

    /**
     * @brief 获取栈上index位置的 LuaRef 对象
     * 
     * @param index 栈上元素位置，和lua接口传递index含义相同
     * @return LuaRef 对象引用，若index非法，返回的LuaRef指向nil
     */
    LuaRetPair<LuaRefOpt>   GetRef(int index);

    // LuaErrOpt               SetGlobal(const std::string& name, const LuaRef& ref);
    // LuaRetPair<LuaRefOpt>   GetGlobal(const std::string& name);

    
    // CXXLUA_API LOW_LEVEL std::optional<LuaErr> Insert2Table(const LuaRef& table);
    std::shared_ptr<LuaStack> m_stack;
};

}

#include <bbt/cxxlua/detail/template/__TLuaVMImpl.hpp>