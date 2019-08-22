[[gnu::dllimport]] __declspec(dllimport) void func1();
[[gnu::dllexport]] __declspec(dllimport) void func2();
[[gnu::dllexport, gnu::dllimport]] void func3();
__attribute__((dllimport)) [[gnu::dllexport]] void func4();