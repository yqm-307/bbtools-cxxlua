#pragma once
#include <bbt/base/file/FileHelper.hpp>
#include <bbt/cxxlua/detail/TypeHelper.hpp>
#include <bbt/cxxlua/detail/LuaErr.hpp>
#include <bbt/cxxlua/detail/LuaTable.hpp>
#include <bbt/cxxlua/detail/LuaValue.hpp>

namespace bbt::cxxlua::detail
{

/**
 * LuaStack 操作封装Api
 * 
 * 操作大类：
 *  1、其他
 *  2、栈操作
 *  3、表操作
 *  4、函数操作
 */

class LuaStack:
    std::enable_shared_from_this<LuaStack>
{
    template<typename T> friend class LuaClass;
public:
    LuaStack(lua_State* l);
    ~LuaStack();

#pragma region // 其他操作
    /**
     * @brief 执行lua脚本
     * 
     * @param script 
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> DoScript(const std::string& script);

    /**
     * @brief 加载一个lua文件
     * 
     * @param file_path 
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> LoadFile(const std::string& file_path);

    /**
     * @brief 加载一个文件夹下面所有的lua源代码
     * 
     * @param folder_path 
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> LoadFolder(const std::string& folder_path);

    /* 创建一个lua table并压入栈顶 */
    void NewLuaTable();

    /* 返回0说明元表已经存在，否则返回1并压入栈顶 */
    int NewMetatable(const std::string& name);

    int SetMetatable(int idx);

    /* 将idx处元素拷贝，并压入栈顶 */
    std::optional<LuaErr> Copy2Top(const LuaRef& ref);

    /* 获取栈顶元素的idx */
    LuaRef GetTop();

    /* 获取栈上idx处元素类型 */
    LUATYPE GetType(const int& index);
    LUATYPE GetType(const LuaRef& ref);

    /* 获取栈长度 */
    size_t Size();

    /* 栈是否为空 */
    bool Empty();

    /* 判断ref是否合法 */
    bool IsSafeRef(const LuaRef& ref);
#pragma endregion

    /**
     * @brief 用 index_value 索引栈顶的表，并将索引到的
     *  值压入栈顶（如果存在、栈顶的值是表），否则返回错误
     * 
     * @param index_value 键值
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 
     */
    std::pair<std::optional<LuaErr>, LUATYPE> Pop4Table(int index_value);

    /**
     * @brief 用 field_name 索引栈顶的表，并将索引到的
     *  值压入栈顶（如果存在、栈顶的值是表），否则返回错误
     * 
     * @param field_name 键值
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 
     */
    std::pair<std::optional<LuaErr>, LUATYPE> Pop4Table(const std::string&  field_name);

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
    std::optional<LuaErr> SetGlobalValue(const std::string& value_name, const LuaRef& value);

    /**
     * @brief 将栈中index初位置的元素插入到全局表中，并命名为value_name
     * 
     * @param value_name lua中全局变量的名字
     * @param index 变量的引用
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> SetGlobalValueByIndex(const std::string& value_name, const LuaRef& index);

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
    std::optional<LuaErr> LoadLuaLib();

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

    std::optional<LuaErr> RegistLuaTable(std::shared_ptr<LuaTable> table);

    std::optional<LuaErr> Push2GlobalByName(const std::string& template_name, const std::string& global_name);




/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////    栈操作                /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#pragma region // 栈操作

    int AbsIndex(int index);
    // 读操作
    LuaRetPair<LuaRef> GetRef(int index);

    std::optional<LuaErr> Pop(LuaValue& value); // 从栈中弹出一个元素，并设置到value中
    void Pop(int n);    // 从栈中弹出n个元素
protected:
    LUATYPE _Pop(bool& value);
    LUATYPE _Pop(int& value);
    LUATYPE _Pop(double& value);
    LUATYPE _Pop(std::string& value);
    LUATYPE _Pop(const char* value);
    LUATYPE _Pop(lua_CFunction& value);
    LUATYPE _Pop(void);
    // 写操作
#pragma endregion

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
    std::optional<LuaErr> _GetByKey4Table(LuaValue& value, TKey key, Args ...args);

    template<typename TKeyValue>
    std::optional<LuaErr> _GetByKey4Table(LuaValue& value, TKeyValue key);

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       全局操作            ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
public:
    LUATYPE GetGlobal(const std::string& value_name);

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
protected:
    lua_State* Context();

    /* 栈操作 */
    LUATYPE Push(int32_t value);
    LUATYPE Push(int64_t value);
    LUATYPE Push(uint32_t value);
    LUATYPE Push(uint64_t value);
    LUATYPE Push(double value);
    LUATYPE Push(const std::string& value);
    LUATYPE Push(const char* value);
    LUATYPE Push(lua_CFunction cfunc);
    LUATYPE Push(const LuaRef& lua_ref);
    LUATYPE Push(const Nil& nil);

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
    bool __IsSafeValue(const LuaRef& ref);
    bool __IsSafeValue(int ref);
    bool __IsSafeValue(double ref);
    bool __IsSafeValue(const std::string& ref);
    bool __IsSafeValue(lua_CFunction ref);


    template<typename KeyType, typename ValueType>
    std::optional<LuaErr> __Insert(KeyType key, ValueType value);


    /**
     * @brief 将一个全局变量压入栈顶并返回其类型
     * 
     * @param value_name 要压入栈顶的变量名
     * @return LUATYPE 
     */
    LUATYPE __GetGlobalValue(const std::string& value_name);

    /* 将栈顶的值压入全局表中，并以value_name命名该变量，使其可以在lua中访问到 */
    std::optional<LuaErr> __SetGlobalValue(const std::string& value_name);

    /**
     * @brief 弹出栈顶表的一个键值对，并将值压入栈顶，返回其类型
     * 
     * @param field_name 键的变量名/值
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 
     */
    std::pair<std::optional<LuaErr>, LUATYPE> __CheckTable(const std::string& field_name);

    /**
     * @brief __CheckTable的重载，仅键类型不同
     * 
     * @param index_value 
     * @return std::pair<std::optional<LuaErr>, LUATYPE> 
     */
    std::pair<std::optional<LuaErr>, LUATYPE> __CheckTable(int index_value);


    /**
     * @brief 解析lua_errcode到LuaErr
     * 
     * @param lua_errcode 
     * @return LuaErr 
     */
    LuaErr __ParseLuaLoadErr(int lua_errcode);

    /**
     * @brief CallLuaFunction 展开终止函数
     * @return std::optional<LuaErr> 
     */
    std::optional<LuaErr> __CallLuaFunction(int nparam, int nresult);

    /**
     * @brief CallLuaFunction 展开辅助函数
     * @return std::optional<LuaErr> 
     */
    template<typename ... Args>
    std::optional<LuaErr> __CallLuaFunction(int nparam, int nresult, Args... args);
private:
    LuaErrOpt __CheckIndex(int index);

private:
    lua_State* lua{nullptr};
    std::unordered_map<std::string, std::shared_ptr<LuaTable>> m_table_template_map;
};

}

#include <bbt/cxxlua/detail/__TLuaStack.hpp>