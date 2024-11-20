#include <bbt/cxxlua/detail/LuaStack.hpp>
#include <bbt/cxxlua/detail/LuaRef.hpp>

namespace bbt::cxxlua::detail
{

std::shared_ptr<LuaStack> LuaStack::Create(lua_State* l)
{
    if (l == nullptr)
        return std::shared_ptr<LuaStack>(new LuaStack());

    return std::shared_ptr<LuaStack>(new LuaStack(l));
}

LuaStack::LuaStack(lua_State* l):
    lua(l)
{
}

LuaStack::LuaStack():
    lua(luaL_newstate())
{
}


LuaStack::~LuaStack()
{
    lua_close(lua);
}

std::optional<LuaErr> LuaStack::DoScript(const std::string& script)
{
    if(luaL_dostring(Context(), script.c_str()) != 0) {
        return LuaErr(lua_tostring(Context(), -1), ERRCODE::VM_ErrLuaRuntime);
    }
    
    return std::nullopt;
}

std::optional<LuaErr> LuaStack::LoadFile(const std::string& file_path)
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

std::optional<LuaErr> LuaStack::LoadFolder(const std::string& folder_path)
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

std::pair<std::optional<LuaErr>, LUATYPE> LuaStack::Pop4Table(int index_value)
{
    return __CheckTable(index_value);
}

std::pair<std::optional<LuaErr>, LUATYPE> LuaStack::Pop4Table(const std::string&  field_name)
{
    return __CheckTable(field_name);
}

std::optional<LuaErr> LuaStack::SetGlobalValue(const std::string& value_name, const LuaRef& value)
{
    LUATYPE tp = value.GetType();

    if(Push(value) != tp) {
        return LuaErr("", ERRCODE::Type_UnExpected);
    }

    return __SetGlobalValue(value_name);
}

std::optional<LuaErr> LuaStack::SetGlobalValueByIndex(const std::string& value_name, const LuaRef& index)
{
    if(value_name.empty()) {
        return LuaErr("value name is invalid!", ERRCODE::VM_ErrParams);
    }

    Copy2Top(index);
    lua_setglobal(Context(), value_name.c_str());
    return std::nullopt;
}

std::optional<LuaErr> LuaStack::LoadLuaLib()
{
    luaL_openlibs(Context());
    return std::nullopt;
}

std::optional<LuaErr> LuaStack::RegistLuaTable(std::shared_ptr<LuaTable> table)
{
    auto [it, ok] = m_table_template_map.insert(std::make_pair(table->m_table_name, table));

    if (!ok) {
        return LuaErr("table already existed!", ERRCODE::Comm_Failed);
    }

    return std::nullopt;
}

std::optional<LuaErr> LuaStack::Push2GlobalByName(const std::string& template_name, const std::string& global_name)
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

void LuaStack::NewLuaTable()
{
    lua_newtable(Context());
}

int LuaStack::NewMetatable(const std::string& name)
{
    return luaL_newmetatable(Context(), name.c_str());
}

int LuaStack::SetMetatable(int idx)
{
    return lua_setmetatable(Context(), idx);
}

std::optional<LuaErr> LuaStack::Copy2Top(const LuaRef& ref)
{
    int index = ref.GetIndex();
    if(index > Size()) {
        return LuaErr("", ERRCODE::Stack_ErrIndex);
    }

    lua_pushvalue(Context(), index);
    return std::nullopt;
}

LuaRef LuaStack::GetTop()
{
    return LuaRef{weak_from_this(), lua_gettop(Context())};
}

LUATYPE LuaStack::GetType(const int& index)
{
    return (LUATYPE)lua_type(Context(), index);
}

LUATYPE LuaStack::GetType(const LuaRef& ref)
{
    return GetType(ref.GetIndex());
}

size_t LuaStack::Size()
{
    return lua_gettop(Context());
}

bool LuaStack::Empty()
{
    return (Size() == 0);
}

bool LuaStack::IsSafeRef(const LuaRef& ref) 
{
    size_t size = Size();
    if(size < ref.GetIndex()) {
        return false;
    }

    return true;
}

void LuaStack::Pop(int n)
{
    lua_pop(Context(), n);
}

int LuaStack::AbsIndex(int index)
{
    return lua_absindex(Context(), index);
}

LuaRetPair<LuaRef> LuaStack::GetRef(int index)
{
    auto err = __CheckIndex(index);
    if (err)
        return {err, LuaRef{}};
    
    return {std::nullopt, LuaRef{weak_from_this(), index}};
}

LuaRetPair<LuaValue> LuaStack::GetValue(int index)
{
    auto value = LuaValue{};
    auto err = __CheckIndex(index);
    if (err)
        return {err, value};
    
    Value v;
    LUATYPE type = GetType(index);
    switch (type)
    {
    case :
        /* code */
        break;
    
    default:
        break;
    }

    return {std::nullopt, LuaValue()};
}

std::optional<LuaErr> LuaStack::Pop(LuaValue& value)
{
    Value v;
    /* 获取栈顶元素类型 */
    LUATYPE type = GetType(-1);
    switch (type)
    {
    case LUATYPE_BOOL:
        _Pop(v.basevalue.boolean);
        value.SetValue(v.basevalue.boolean);
        break;
    case LUATYPE_CSTRING:
        _Pop(v.str);
        value.SetValue(v.str);
        break;
    case LUATYPE_FUNCTION:
        _Pop(v.cfunc);
        value.SetValue(v.cfunc);
        break;
    case LUATYPE_NUMBER:
        _Pop(v.basevalue.number);
        value.SetValue(v.basevalue.number);
        break;
    case LUATYPE_NIL:
        _Pop();
        value.SetValue(nil);
        break;
    default:
        return LuaErr("Pop() unsupported type!", ERRCODE::Comm_Failed);
        break;
    }

    return std::nullopt;
}

LuaErrOpt LuaStack::_ToValue(bool& value, int index)
{
    LUATYPE type = GetType(index);
    value = lua_toboolean(Context(), index);
    if (type != LUATYPE_BOOL)
        return LuaErr{"unexpected type!", ERRCODE::Type_UnExpected}; // 非预期类型

    return std::nullopt; 
}

LuaErrOpt LuaStack::_ToValue(int& value, int index)
{
    LUATYPE type = GetType(index);
    value = lua_tointeger(Context(), index);
    if (type != LUATYPE_NUMBER)
        return LuaErr{"unexpected type!", ERRCODE::Type_UnExpected}; // 非预期类型

    return std::nullopt; 
}

LuaErrOpt LuaStack::_ToValue(double& value, int index)
{
    LUATYPE type = GetType(index);
    value = lua_tonumber(Context(), index);
    if (type != LUATYPE_NUMBER)
        return LuaErr{"unexpected type!", ERRCODE::Type_UnExpected}; // 非预期类型

    return std::nullopt; 
}

LuaErrOpt LuaStack::_ToValue(std::string& value, int index)
{
    LUATYPE type = GetType(index);
    value = lua_tostring(Context(), index);
    if (type != LUATYPE_CSTRING)
        return LuaErr{"unexpected type!", ERRCODE::Type_UnExpected}; // 非预期类型

    return std::nullopt; 
}

LuaErrOpt LuaStack::_ToValue(const char* value, int index)
{
    LUATYPE type = GetType(index);
    value = lua_tostring(Context(), index);
    if (type != LUATYPE_CSTRING)
        return LuaErr{"unexpected type!", ERRCODE::Type_UnExpected}; // 非预期类型

    return std::nullopt; 
}

LuaErrOpt LuaStack::_ToValue(lua_CFunction& value, int index)
{
    LUATYPE type = GetType(index);
    value = lua_tocfunction(Context(), index);
    if (type != LUATYPE_CSTRING)
        return LuaErr{"unexpected type!", ERRCODE::Type_UnExpected}; // 非预期类型

    return std::nullopt; 
}

LuaErrOpt LuaStack::_ToValue(Nil& nil, int index)
{
    LUATYPE type = GetType(index);
    if (type != LUATYPE_NIL)
        return LuaErr{"unexpected type!", ERRCODE::Type_UnExpected}; // 非预期类型

    return std::nullopt; 
}

LuaRetPair<LUATYPE> LuaStack::_Pop(bool& value)
{
    auto err = _ToValue(value, -1);
    if (err)
        return {err, LUATYPE_NONE};

    Pop(1);
    return {std::nullopt, LUATYPE_BOOL};
}

LuaRetPair<LUATYPE> LuaStack::_Pop(int& value)
{
    auto err = _ToValue(value, -1);
    if (err)
        return {err, LUATYPE_NONE};

    Pop(1);
    return {std::nullopt, LUATYPE_NUMBER};
}

LuaRetPair<LUATYPE> LuaStack::_Pop(double& value)
{
    auto err = _ToValue(value, -1);
    if (err)
        return {err, LUATYPE_NONE};

    Pop(1);
    return {std::nullopt, LUATYPE_NUMBER};
}

LuaRetPair<LUATYPE> LuaStack::_Pop(std::string& value) 
{
    auto err = _ToValue(value, -1);
    if (err)
        return {err, LUATYPE_NONE};

    Pop(1);
    return {std::nullopt, LUATYPE_CSTRING};
}

LuaRetPair<LUATYPE> LuaStack::_Pop(const char* value) 
{
    auto err = _ToValue(value, -1);
    if (err)
        return {err, LUATYPE_NONE};

    Pop(1);
    return {std::nullopt, LUATYPE_CSTRING};
}

LuaRetPair<LUATYPE> LuaStack::_Pop(lua_CFunction& value)
{
    auto err = _ToValue(value, -1);
    if (err)
        return {err, LUATYPE_NONE};

    Pop(1);
    return {std::nullopt, LUATYPE_FUNCTION};
}

LuaRetPair<LUATYPE> LuaStack::_Pop(void)
{
    auto err = _ToValue(nil, -1);
    if (err)
        return {err, LUATYPE_NONE};

    Pop(1);
    return {std::nullopt, LUATYPE_NIL};
}

LUATYPE LuaStack::GetGlobal(const std::string& value_name)
{
    return __GetGlobalValue(value_name);
}

lua_State* LuaStack::Context()
{
    return lua;
}

/* 栈操作 */
LUATYPE LuaStack::Push(int32_t value) 
{
    lua_pushinteger(Context(), value);
    return GetType(-1);
}

LUATYPE LuaStack::Push(int64_t value)
{
    return Push((int32_t)value);
}

LUATYPE LuaStack::Push(uint32_t value)
{
    lua_pushinteger(Context(), value);
    return GetType(-1);
}

LUATYPE LuaStack::Push(uint64_t value)
{
    return Push((uint32_t)value);
}

LUATYPE LuaStack::Push(double value)
{
    lua_pushnumber(Context(), value);
    return GetType(-1);
}

LUATYPE LuaStack::Push(const std::string& value)
{
    const char* ret = lua_pushstring(Context(), value.c_str());
    if(ret == NULL) {
        return LUATYPE::LUATYPE_NIL;
    }
    return GetType(-1);
}

LUATYPE LuaStack::Push(const char* value)
{
    const char* ret = lua_pushstring(Context(), value);
    if(ret == NULL) {
        return LUATYPE::LUATYPE_NIL;
    }
    return GetType(-1);
}

LUATYPE LuaStack::Push(lua_CFunction cfunc)
{
    lua_pushcfunction(Context(), cfunc);
    return GetType(-1);
}

LUATYPE LuaStack::Push(const LuaRef& lua_ref)
{
    lua_pushvalue(Context(), lua_ref.GetIndex());
    return GetType(-1);
}

LUATYPE LuaStack::Push(const Nil& nil)
{
    lua_pushnil(Context());
    return LUATYPE::LUATYPE_NIL;
}


bool LuaStack::__IsSafeValue(const LuaRef& ref)
{
    return IsSafeRef(ref);
}

bool LuaStack::__IsSafeValue(int ref)
{
    return true;
}

bool LuaStack::__IsSafeValue(double ref)
{
    return true;
}

bool LuaStack::__IsSafeValue(const std::string& ref)
{
    return (!ref.empty());
}

bool LuaStack::__IsSafeValue(lua_CFunction ref)
{
    return (ref != nullptr);
}

LUATYPE LuaStack::__GetGlobalValue(const std::string& value_name)
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
std::optional<LuaErr> LuaStack::__SetGlobalValue(const std::string& value_name)
{
    lua_setglobal(Context(), value_name.c_str());
    return std::nullopt;
}

std::pair<std::optional<LuaErr>, LUATYPE> LuaStack::__CheckTable(const std::string& field_name)
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

std::pair<std::optional<LuaErr>, LUATYPE> LuaStack::__CheckTable(int index_value)
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

LuaErr LuaStack::__ParseLuaLoadErr(int lua_errcode)
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

std::optional<LuaErr> LuaStack::__CallLuaFunction(int nparam, int nresult)
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

LuaErrOpt LuaStack::__CheckIndex(int index)
{
    // lua_absindex 并不检测有效性
    index = lua_absindex(Context(), index);
    if (index <= 0 || index > Size())
        return LuaErr{"bad index", ERRCODE::Stack_ErrIndex};

    return std::nullopt;
}


}