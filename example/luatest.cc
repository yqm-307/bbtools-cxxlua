#include <bbt/cxxlua/CXXLua.hpp>
#include <optional>

#define IF_NOT_EXIT(err, handle) \
auto err = handle; \
if(err != std::nullopt) { \
    printf("%s\n", err.value().What().c_str()); \
    return -1; \
}

int main()
{
    bbt::cxxlua::LuaVM lua;
    /* lua虚拟机 加载 lua 库函数 */
    auto load_lib_err = lua.LoadLuaLibrary();
    if(load_lib_err != std::nullopt) {
        printf("%s\n", load_lib_err.value().What().c_str());
        return -1;
    }

    /* lua虚拟机编译lua脚本，并装载到lua虚拟机的字节码块 */
    auto err = lua.LoadFolder("example/script");
    if(err != std::nullopt) {
        printf("%s\n", err.value().What().c_str());
        return -1;
    }

    /* 在该lua虚拟机中执行函数 */
    auto call_func_err = lua.CallLuaFunction(
        "PrintInfo",
        0,
        [](std::unique_ptr<bbt::cxxlua::detail::LuaStack>& ptr) { 
            printf("参数解析\n"); 
            return std::nullopt; 
        },
        1, "asdasd", 2, 1.231, "is`t too cool!");
    if(call_func_err != std::nullopt) {
        printf("%s\n", call_func_err.value().What().c_str());
        return -1;
    }

    auto err3 = lua.DoScript(R"(
        G_TB = {
            "test1" = 1,
            "test2" = {
                "test3" = 3,
            },
            [4] = "test4",
        }
    )");

    bbt::cxxlua::LuaValue value_test1;
    IF_NOT_EXIT(err4, lua.GetByKey4Table(value_test1, "G_TB", "test1"));

    double number;
    std::string str;

    assert(value_test1.GetValue(number));
    printf("test1 = %d\n", (int)number);
    IF_NOT_EXIT(err5, lua.GetByKey4Table(value_test1, "G_TB", "test2", "test3"));
    assert(value_test1.GetValue(number));
    printf("test3 = %d\n", (int)number);
    IF_NOT_EXIT(err6, lua.GetByKey4Table(value_test1, "G_TB", 4));
    assert(value_test1.GetValue(str));
    printf("test1 = %s\n", str.c_str());    

}