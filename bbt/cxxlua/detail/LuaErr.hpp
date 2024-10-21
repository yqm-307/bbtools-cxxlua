#pragma once
#include "Config.hpp"

namespace bbt::cxxlua
{

enum ERRCODE
{
    Default = 0, 
    Comm_Succes  = 1,
    Comm_Failed  = 2,

    // lua vm
    VM_ErrParams = 3,  // 参数错误
    VM_ErrSyntax = 4,  // 语法错误
    VM_ErrMem    = 5,  // 内存分配错误
    VM_ErrLuaRuntime = 6, // lua runtime 错误

    // lua type assert
    Type_UnExpected = 10, // 操作值是非预期类型
    Type_NotTable   = 11, // 目标不是表
    Type_NotInt     = 12, // 目标不是integer
    Type_IsNil      = 13, // 目标是空表

    // lua stack
    Stack_ErrIndex  = 14, // 错误的栈下标
};

namespace detail
{

class LuaErr
{
public:
    LuaErr():
        m_traceback_err(std::nullopt), m_errcode(ERRCODE::Default) {}
    LuaErr(const char* cstr, bbt::cxxlua::ERRCODE code):
        m_traceback_err(cstr), m_errcode(code) {}
    LuaErr(const std::string& str, bbt::cxxlua::ERRCODE code):
        m_traceback_err(str), m_errcode(code) {}
    LuaErr(LuaErr&& other):
        m_traceback_err(std::move(other.m_traceback_err)),
        m_errcode(other.m_errcode){}
    LuaErr(const LuaErr& other):
        m_traceback_err(other.m_traceback_err),
        m_errcode(other.m_errcode){}
    ~LuaErr() {}

    void Clear(){m_traceback_err.value().clear(); m_errcode = bbt::cxxlua::ERRCODE::Default;}

    void Reset(const char* cstr, bbt::cxxlua::ERRCODE code = Default)
    { m_traceback_err = std::string(cstr); m_errcode = code; }

    void SetStr(const std::string& str) 
    { m_traceback_err = str; }

    void SetCode(ERRCODE code)
    { m_errcode = code; }

    void Swap(LuaErr&& other)
    { 
        if(other.m_traceback_err != std::nullopt)
            m_traceback_err.emplace(other.m_traceback_err.value());
        m_errcode = other.m_errcode;
    }

    std::string What()
    { return m_traceback_err == std::nullopt ? "" : m_traceback_err.value(); }

    ERRCODE Code()
    { return m_errcode; }
private:
    std::optional<std::string> m_traceback_err;
    bbt::cxxlua::ERRCODE m_errcode;
};

}

}
