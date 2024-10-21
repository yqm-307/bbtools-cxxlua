/**
 * @file CXXLua.hpp
 * @author your name (you@domain.com)
 * @brief 使用 head only 模式实现
 * @version 0.1
 * @date 2023-11-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <bbt/cxxlua/detail/LuaVM.hpp>
#include <bbt/cxxlua/detail/LuaClass.hpp>

namespace bbt::cxxlua
{

typedef bbt::cxxlua::detail::LuaVM LuaVM;
typedef bbt::cxxlua::detail::LuaValue Value;

template<typename CXXClass>
using LuaClass = bbt::cxxlua::detail::LuaClass<CXXClass>;

typedef bbt::cxxlua::detail::LuaErr LuaErr;
typedef bbt::cxxlua::detail::LUATYPE LUATYPE;
typedef bbt::cxxlua::detail::LuaTable LuaTable;


} // namespace bbt::cxxlua