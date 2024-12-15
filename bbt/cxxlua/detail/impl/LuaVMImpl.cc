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

LUATYPE LuaVmImpl::GetType(int index) const
{
    return m_stack->GetType(index);
}

LUATYPE LuaVmImpl::GetType(const LuaRef& ref) const
{
    if (ref)
        return m_stack->GetType(ref.GetIndex());

    return LUATYPE::LUATYPE_NONE;
}


int LuaVmImpl::ToAbsIndex(int index)
{
    return m_stack->AbsIndex(index);
}

int LuaVmImpl::StackSize()
{
    return m_stack->Size();
}

void LuaVmImpl::NewTable() const
{
    m_stack->NewLuaTable();
}

LuaErrOpt LuaVmImpl::NewMetatable(const std::string& metatable_name) const
{
    if (0 == m_stack->NewMetatable(metatable_name))
        return LuaErr{"metatable is exist!", ERRCODE::Comm_Failed};

    return std::nullopt;
}

LuaErrOpt LuaVmImpl::SetMetatable(const LuaRef& obj, const std::string& metatable_name) const
{
    auto err = m_stack->Copy2Top(obj);
    if (err != std::nullopt)
        return err;

    m_stack->SetMetatable(metatable_name);
    return std::nullopt;
}

}