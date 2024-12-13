#include <bbt/cxxlua/detail/impl/LuaVMImpl.hpp>
#include <bbt/cxxlua/detail/LuaRef.hpp>

namespace bbt::cxxlua::detail
{

LuaVmImpl::LuaVmImpl():
    m_stack(LuaStack::Create())
{
}

LuaVmImpl::~LuaVmImpl()
{
}

LuaVmImpl::LuaVmImpl(LuaVmImpl&& impl)
{
    m_stack = std::move(impl.m_stack);
}

LuaVmImpl::LuaVmImpl(const LuaVmImpl& impl)
{
    m_stack = impl.m_stack;
}

LuaVmImpl& LuaVmImpl::operator=(LuaVmImpl&& impl)
{
    m_stack = std::move(impl.m_stack);
    return *this;
}

LuaVmImpl& LuaVmImpl::operator=(const LuaVmImpl& impl)
{
    m_stack = impl.m_stack;
    return *this;
}

LuaRetPair<LuaRefOpt> LuaVmImpl::GetRef(int index)
{
    return m_stack->GetRef(index);
}

}