#include <bbt/cxxlua/detail/LuaRef.hpp>
#include <bbt/cxxlua/detail/LuaStack.hpp>

namespace bbt::cxxlua::detail
{

LuaRef::LuaRef(std::weak_ptr<LuaStack> stack, int index):
    m_stack(stack),
    m_index(index)
{
    /* 初始化必须是合法索引 */
    Assert(m_index != 0);
    Assert(!m_stack.expired());
}

LuaRef::LuaRef(const LuaRef& other):
    m_index(other.m_index),
    m_stack(other.m_stack)
{
    Assert(m_index != 0);
    Assert(!m_stack.expired());
}

LuaRef::LuaRef()
{
}


LUATYPE LuaRef::GetType() const
{
    auto stack = m_stack.lock();
    if (IsInvaild(m_stack, m_index) || stack == nullptr)
        return LUATYPE::LUATYPE_NONE;

    return stack->GetType(m_index);
}

LuaRef::operator bool()
{
    return !IsInvaild(m_stack, m_index);
}

bool LuaRef::SetIndex(std::shared_ptr<LuaStack> stack, int index)
{
    if (IsInvaild(stack, index))
        return false;

    m_index = index;
    m_stack = stack;
    return true;
}

bool LuaRef::SetIndex(int index)
{
    if (IsInvaild(m_stack, index))
        return false;

    m_index = index;
    return true;
}

bool LuaRef::SetStack(std::shared_ptr<LuaStack> stack)
{
    if (IsInvaild(stack, m_index))
        return false;

    m_stack = stack;
    return true;
}

int LuaRef::GetIndex() const
{
    return m_index;
}

int LuaRef::AbsIndex(int index)
{
    auto stack = m_stack.lock();
    if (!stack)
        return 0;

    return stack->AbsIndex(index);
}

bool LuaRef::IsInvaild(std::weak_ptr<LuaStack> stack_wkptr, int index)
{
    auto stack = stack_wkptr.lock();
    if (stack == nullptr || index <= 0)
        return true;
        
    if (stack->Size() < index)
        return true;

    return false;
}


}