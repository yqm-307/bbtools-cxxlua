#pragma once
#include "bbt/cxxlua/detail/TypeHelper.hpp"
#include "bbt/base/file/FileHelper.hpp"
#include "LuaErr.hpp"
#include "LuaRef.hpp"
#include "LuaTable.hpp"
#include "LuaValue.hpp"

namespace bbt::cxxlua::detail
{

class LuaStack:
    std::enable_shared_from_this<LuaStack>
{
    template<typename T> friend class LuaClass;
public:
    LuaStack(lua_State* l):lua(l){}
    ~LuaStack(){}

    /**
     * @brief 执行lua脚本
     * 
     * @param script 
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> DoScript(const std::string& script)
    {
        if(luaL_dostring(Context(), script.c_str()) != 0) {
            return LuaErr(lua_tostring(Context(), -1), ERRCODE::VM_ErrLuaRuntime);
        }
        
        return std::nullopt;
    }

    /**
     * @brief 加载一个lua文件
     * 
     * @param file_path 
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> LoadFile(const std::string& file_path)
    {
        int err = luaL_loadfile(Context(), file_path.c_str());
        if(err != LUA_OK) {
            return __ParseLuaLoadErr(err);
        }

        int ret = lua_pcall(Context(), 0, 0, 0);
        switch (ret)
        {
        case LUA_OK:
            break;    
        case LUA_ERRRUN:
            return LuaErr(lua_tostring(Context(), -1), ERRCODE::VM_ErrLuaRuntime);
        default:
            return LuaErr(std::to_string(ret), ERRCODE::Default);
        }

        return std::nullopt;
    }

    /**
     * @brief 加载一个文件夹下面所有的lua源代码
     * 
     * @param folder_path 
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> LoadFolder(const std::string& folder_path)
    {
        if(folder_path.empty() || !file::Exist(folder_path))
            return LuaErr("", ERRCODE::VM_ErrParams);

        auto file_list = file::GetFileByFolder(folder_path, false, {"lua"});

        for (auto &&filename : file_list)
        {
            auto err = LoadFile(filename);
            if(err != std::nullopt) {
                return err;
            }
        }

        return std::nullopt;
    }

    /**
     * @brief 用 index_value 索引栈顶的表，并将索引到的
     *  值压入栈顶（如果存在、栈顶的值是表），否则返回错误
     * 
     * @param index_value 键值
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 
     */
    std::pair<std::optional<LuaErr>, LUATYPE> Pop4Table(int index_value)
    {
        return __CheckTable(index_value);
    }

    /**
     * @brief 用 field_name 索引栈顶的表，并将索引到的
     *  值压入栈顶（如果存在、栈顶的值是表），否则返回错误
     * 
     * @param field_name 键值
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 
     */
    std::pair<std::optional<LuaErr>, LUATYPE> Pop4Table(const std::string&  field_name)
    {
        return __CheckTable(field_name);
    }

    /**
     * @brief 从全局表获取一个值，如果值类型与LuaType相等则
     *  压入栈顶，否则只返回值的类型。
     * 
     * @tparam LuaType 弹出值的类型
     * @param table_name 
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 如果类型错误，LUATYPE是错误的类型
     */
    template<LUATYPE LuaType>
    std::pair<std::optional<LuaErr>, LUATYPE> CheckGlobalValue(const std::string& value_name);

    /**
     * @brief 将一个c++变量直接插入到lua全局表中
     * 
     * @tparam LuaType 
     * @tparam T 
     * @param value_name lua中全局变量的名字
     * @param value 变量的值
     * @return std::optional<LuaErr> 
     */
    template<typename T>
    std::optional<LuaErr> SetGlobalValue(const std::string& value_name, T value);
    std::optional<LuaErr> SetGlobalValue(const std::string& value_name, const LuaRef& value)
    {
        LUATYPE tp = value.GetType();

        if(Push(value) != tp) {
            return LuaErr("", ERRCODE::Type_UnExpected);
        }

        return __SetGlobalValue(value_name);
    }

    /**
     * @brief 将栈中index初位置的元素插入到全局表中，并命名为value_name
     * 
     * @param value_name lua中全局变量的名字
     * @param index 变量的引用
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> SetGlobalValueByIndex(const std::string& value_name, const LuaRef& index)
    {
        if(value_name.empty()) {
            return LuaErr("value name is invalid!", ERRCODE::VM_ErrParams);
        }

        Copy2Top(index);
        lua_setglobal(Context(), value_name.c_str());
        return std::nullopt;
    }

    /**
     * @brief 调用lua函数
     * 
     * @tparam Args 
     * @param nparam 
     * @param nresult 
     * @param args 
     * @return std::optional<LuaErr> 
     */
    template<typename ... Args>
    std::optional<LuaErr> LuaCall(int nparam, int nresult, Args... args);

    /**
     * @brief 加载lua lib
     * 
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> LoadLuaLib()
    {
        luaL_openlibs(Context());
        return std::nullopt;
    }

    /**
     * @brief 向栈顶的table（默认栈顶元素是table）插入一个key，value（ps:会覆盖）
     * 
     * @tparam KeyType 
     * @tparam ValueType 
     * @param key 键  
     * @param value 值 
     * @return std::optional<LuaErr> 
     */
    template<typename KeyType, typename ValueType>
    std::optional<LuaErr> Insert2Table(KeyType key, ValueType value);

    std::optional<LuaErr> RegistLuaTable(std::shared_ptr<LuaTable> table)
    {
        auto [it, ok] = m_table_template_map.insert(std::make_pair(table->m_table_name, table));

        if (!ok) {
            return LuaErr("table already existed!", ERRCODE::Comm_Failed);
        }

        return std::nullopt;
    }

    std::optional<LuaErr> Push2GlobalByName(const std::string& template_name, const std::string& global_name)
    {
        auto it = m_table_template_map.find(template_name);    
        if (it == m_table_template_map.end()) {
            return LuaErr("key not found!", ERRCODE::Comm_Failed);
        }

        auto table = it->second;
        
        /* 创建一个lua table */
        NewLuaTable();
        auto ref = GetTop();

        if (!table->m_cfunction_set.empty()) {
            for (auto&& pair : table->m_cfunction_set) {
                Insert2Table(pair.first.c_str(), pair.second);
            }
        }

        if (!table->m_field_set.empty()) {
            for (auto&& pair : table->m_field_set) {
                Insert2Table(pair.first.c_str(), pair.second);
            }
        }

        SetGlobalValue(table->m_table_name, ref);

        if (table->m_table_init_func) {
            return table->m_table_init_func(std::unique_ptr<LuaStack>(this));
        }

        return std::nullopt;
    }


    /* 创建一个lua table并压入栈顶 */
    void NewLuaTable() { lua_newtable(Context()); }
    /* 返回0说明元表已经存在，否则返回1并压入栈顶 */
    int NewMetatable(const std::string& name){ return luaL_newmetatable(Context(), name.c_str()); }
    int SetMetatable(int idx) { return lua_setmetatable(Context(), idx); }
    /* 将idx处元素拷贝，并压入栈顶 */
    std::optional<LuaErr> Copy2Top(const LuaRef& ref)
    {
        int index = ref.GetIndex();
        if(index > Size()) {
            return LuaErr("", ERRCODE::Stack_ErrIndex);
        }

        lua_pushvalue(Context(), index);
        return std::nullopt;
    }

    /* 获取栈顶元素的idx */
    LuaRef GetTop(){ return LuaRef(lua_gettop(Context()), GetType(g_lua_top_ref)); }
    /* 获取栈上idx处元素类型 */
    LUATYPE GetType(const LuaRef& ref){ return (LUATYPE)lua_type(Context(), ref.GetIndex()); }

    size_t Size() { return lua_gettop(Context()); }
    bool Empty() { return (Size() == 0); }
    bool IsSafeRef(const LuaRef& ref) 
    {
        size_t size = Size();
        if(size < ref.GetIndex()) {
            return false;
        }

        return true;
    }



    void Pop(int n) { lua_pop(Context(), n); }
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////    从栈中弹出基本类型    /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
public:
    std::optional<LuaErr> Pop(LuaValue& value)
    {
        /* 获取栈顶元素类型 */
        LUATYPE type = GetType(g_lua_top_ref);
        switch (type)
        {
        case LUATYPE_BOOL:
            value.type = _Pop(value.basevalue.boolean);
            break;
        case LUATYPE_CSTRING:
            value.type = _Pop(value.str);
            break;
        case LUATYPE_FUNCTION:
            value.type = _Pop(value.cfunc);
            break;
        case LUATYPE_NUMBER:
            value.type = _Pop(value.basevalue.integer);
            break;
        case LUATYPE_NIL:
            value.type = _Pop();
            break;
        default:
            return LuaErr("Pop() unsupported type!", ERRCODE::Comm_Failed);
            break;
        }

        return std::nullopt;
    }

protected:
    LUATYPE _Pop(bool& value)
    {
        LUATYPE type = GetType(g_lua_top_ref);
        value = lua_toboolean(Context(), -1);
        lua_pop(Context(), 1);
        return type;
    }
    LUATYPE _Pop(int& value)
    {
        LUATYPE type = GetType(g_lua_top_ref);
        value = lua_tointeger(Context(), -1);
        lua_pop(Context(), 1);
        return type;
    }
    LUATYPE _Pop(double& value)
    {
        LUATYPE type = GetType(g_lua_top_ref);
        value = lua_tonumber(Context(), -1);
        lua_pop(Context(), 1);
        return type;
    }
    LUATYPE _Pop(std::string& value) 
    {
        LUATYPE type = GetType(g_lua_top_ref);
        value = lua_tostring(Context(), -1);
        lua_pop(Context(), 1);
        return type;
    }
    LUATYPE _Pop(const char* value) 
    {
        LUATYPE type = GetType(g_lua_top_ref);
        value = lua_tostring(Context(), -1);
        lua_pop(Context(), 1);
        return type;
    }
    LUATYPE _Pop(lua_CFunction& value)
    {
        LUATYPE type = GetType(g_lua_top_ref);
        value = lua_tocfunction(Context(), -1);
        lua_pop(Context(), 1);
        return type;
    }
    LUATYPE _Pop(void)
    {
        LUATYPE type = GetType(g_lua_top_ref);
        lua_pop(Context(), 1);
        return type;
    }

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       访问table          /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief 对当前栈顶表进行多级访问操作
     * 
     * @tparam Args 
     * @param value 
     * @param args 
     * @return std::optional<LuaErr> 
     */
    template<typename ...Args>
    std::optional<LuaErr> GetByKey4Table(LuaValue& value, Args ...args)
    {
        return _GetByKey4Table(value, args...);
    }

private:
    template<typename TKey, typename ...Args>
    std::optional<LuaErr> _GetByKey4Table(LuaValue& value, TKey key, Args ...args)
    {
        if (LUATYPE_LUATABLE != GetType(g_lua_top_ref))
            return LuaErr("[LuaStack::_GetByKey4Table] top value not lua table!", ERRCODE::Comm_Failed);

        // 压入key，获取table值
        Push(key);
        LUATYPE type = (LUATYPE)lua_gettable(Context(), -2);

        // 取出再尝试获取table值
        auto err = _GetByKey4Table(value, args...);
        Pop(1);

        return err;
    }

    template<typename TKeyValue>
    std::optional<LuaErr> _GetByKey4Table(LuaValue& value, TKeyValue key)
    {
        if (LUATYPE_LUATABLE != GetType(g_lua_top_ref))
            return LuaErr("top value not lua table!", ERRCODE::Comm_Failed);
        
        Push(key);

        LUATYPE type = (LUATYPE)lua_gettable(Context(), -2);

        return Pop(value);
    }
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       全局操作            ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
public:
    LUATYPE GetGlobal(const std::string& value_name)
    {
        return __GetGlobalValue(value_name);
    }

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
protected:
    lua_State* Context(){ return lua; }

    /* 栈操作 */
    LUATYPE Push(int32_t value) 
    {
        lua_pushinteger(Context(), value);
        return GetType(g_lua_top_ref);
    }
    LUATYPE Push(int64_t value)
    {
        return Push((int32_t)value);
    }
    LUATYPE Push(uint32_t value)
    {
        lua_pushinteger(Context(), value);
        return GetType(g_lua_top_ref);
    }
    LUATYPE Push(uint64_t value)
    {
        return Push((uint32_t)value);
    }
    LUATYPE Push(double value)
    {
        lua_pushnumber(Context(), value);
        return GetType(g_lua_top_ref);
    }
    LUATYPE Push(const std::string& value)
    {
        const char* ret = lua_pushstring(Context(), value.c_str());
        if(ret == NULL) {
            return LUATYPE::LUATYPE_NIL;
        }
        return GetType(g_lua_top_ref);
    }
    LUATYPE Push(const char* value)
    {
        const char* ret = lua_pushstring(Context(), value);
        if(ret == NULL) {
            return LUATYPE::LUATYPE_NIL;
        }
        return GetType(g_lua_top_ref);
    }
    LUATYPE Push(lua_CFunction cfunc)
    {
        lua_pushcfunction(Context(), cfunc);
        return GetType(g_lua_top_ref);
    }
    LUATYPE Push(const LuaRef& lua_ref)
    {
        lua_pushvalue(Context(), lua_ref.GetIndex());
        return GetType(g_lua_top_ref);
    }

    void PushMany() {}

    template<typename T, typename ... Args>
    void PushMany(T arg, Args ...args);

    /**
     * @brief 判断值是否为安全的可入栈的
     * 
     * @param ref 
     * @return true 
     * @return false 
     */
    bool __IsSafeValue(const LuaRef& ref) { return IsSafeRef(ref); }
    bool __IsSafeValue(int ref) { return true; }
    bool __IsSafeValue(double ref) { return true; }
    bool __IsSafeValue(const std::string& ref) { return (!ref.empty()); }
    bool __IsSafeValue(lua_CFunction ref) { return (ref != nullptr); }


    template<typename KeyType, typename ValueType>
    std::optional<LuaErr> __Insert(KeyType key, ValueType value);


    /**
     * @brief 将一个全局变量压入栈顶并返回其类型
     * 
     * @param value_name 要压入栈顶的变量名
     * @return LUATYPE 
     */
    LUATYPE __GetGlobalValue(const std::string& value_name)
    {
        if(value_name.empty()) {
            return LUATYPE::LUATYPE_NONE; // params error
        }

        int type = lua_getglobal(Context(), value_name.c_str());
        if(CXXLUAInvalidType(type)) {
            return LUATYPE::LUATYPE_NIL;
        }

        return (LUATYPE)type;
    }

    /* 将栈顶的值压入全局表中，并以value_name命名该变量，使其可以在lua中访问到 */
    std::optional<LuaErr> __SetGlobalValue(const std::string& value_name)
    {
        lua_setglobal(Context(), value_name.c_str());
        return std::nullopt;
    }

    /**
     * @brief 弹出栈顶表的一个键值对，并将值压入栈顶，返回其类型
     * 
     * @param field_name 键的变量名/值
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 
     */
    std::pair<std::optional<LuaErr>, LUATYPE> __CheckTable(const std::string& field_name)
    {
        int type = lua_type(Context(), -1);
        if(CXXLUAInvalidType(type)) {
            return {LuaErr("", ERRCODE::Type_UnExpected), (LUATYPE)type};
        }

        lua_pushstring(Context(), field_name.c_str());
        if(lua_gettable(Context(), -2)) {
            return {LuaErr(lua_tostring(Context(), -1), ERRCODE::VM_ErrLuaRuntime), (LUATYPE)type};
        }

        type = lua_type(Context(), -1);
        return {std::nullopt, (LUATYPE)type};
    }
    /**
     * @brief __CheckTable的重载，仅键类型不同
     * 
     * @param index_value 
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 
     */
    std::pair<std::optional<LuaErr>, LUATYPE> __CheckTable(int index_value)
    {
        int type = lua_type(Context(), -1);
        if(CXXLUAInvalidType(type)) {
            return {LuaErr("", ERRCODE::Type_UnExpected), (LUATYPE)type};
        }

        lua_pushinteger(Context(), index_value);
        int err = lua_gettable(Context(), -2);
        if(err != LUA_OK) {
            return {LuaErr(lua_tostring(Context(), -1), ERRCODE::VM_ErrLuaRuntime), (LUATYPE)type};
        }

        type = lua_type(Context(), -1);
        return {std::nullopt, (LUATYPE)type};
    }


    /**
     * @brief 解析lua_errcode到LuaErr
     * 
     * @param lua_errcode 
     * @return LuaErr 
     */
    LuaErr __ParseLuaLoadErr(int lua_errcode)
    {
        LuaErr err;
        switch(lua_errcode) {
        case LUA_ERRSYNTAX:
            err.Reset(lua_tostring(Context(), -1), ERRCODE::VM_ErrSyntax);
            break;
        case LUA_ERRMEM:
            err.Reset("", ERRCODE::VM_ErrMem);
            break;
        default:
            err.Reset("", ERRCODE::Default);
            break;
        }

        return err;
    }

    /**
     * @brief CallLuaFunction 展开终止函数
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> __CallLuaFunction(int nparam, int nresult)
    {
        int ret = lua_pcall(Context(), nparam, nresult, 0);
        switch (ret)
        {
        case LUA_OK:
            break;    
        case LUA_ERRRUN:
            return LuaErr(lua_tostring(Context(), -1), ERRCODE::VM_ErrLuaRuntime);
        default:
            return LuaErr(std::to_string(ret), ERRCODE::Default);
        }

        return std::nullopt;
    }

    /**
     * @brief CallLuaFunction 展开辅助函数
     * @return std::optional<LuaErr> 
     */
    template<typename ... Args>
    std::optional<LuaErr> __CallLuaFunction(int nparam, int nresult, Args... args);

private:
    lua_State* lua{nullptr};
    std::unordered_map<std::string, std::shared_ptr<LuaTable>> m_table_template_map;
};

}

#include "./LuaStack_Def.hpp"