#pragma once
#include <bbt/cxxlua/detail/Config.hpp>

namespace bbt::cxxlua::detail
{

class LuaRef
{
public:
    explicit LuaRef(std::weak_ptr<LuaStack> stack, int index);
    LuaRef(const LuaRef& other);
    LuaRef();
    ~LuaRef() {}

    /**
     * @brief 设置栈和索引
     */
    bool SetIndex(std::shared_ptr<LuaStack> stack, int index);
    bool SetIndex(int index);
    bool SetStack(std::shared_ptr<LuaStack> stack);

    /**
     * @brief 获取栈上绝对索引
     */
    int  GetIndex() const;

    /**
     * @brief 获取类型
     */
    LUATYPE GetType() const;

    /**
     * @brief 当前引用是否存在于栈上
     */
    operator bool();
private:
    /* 获取lua栈上的绝对索引 */
    int AbsIndex(int index);

    /* 是否有效 */
    static bool IsInvaild(std::weak_ptr<LuaStack> stack, int index);
private:
    int m_index{0}; // lua栈上索引
    std::weak_ptr<LuaStack> m_stack;
};


} // namespace bbt::cxxlua::detail