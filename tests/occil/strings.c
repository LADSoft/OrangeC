#include <stdio.h>

using namespace System;

__string myValue = "staticString";
void funccall(__string arg)
{
    System::Console::WriteLine(arg);
}
__string getString(int rr)
{
    return "hi" + rr + myValue;
}
int main(int argc, char **argv)
{
    Console::WriteLine("hello world!");
    __string aa = "hello World " + 2 + "!";
    Console::WriteLine(aa);
    Console::WriteLine(543);
    // occil can't resolve the context for the concatenation in this next one so type cast
    // it...
    funccall((__string)"hello world!" + "from function");
    funccall("another string");
    funccall(aa + "hello again!");
    Console::WriteLine(getString(273));
    Console::WriteLine("hi {1} {0}", 5, "hello there!");
    printf("%s %d %d", "hello world!",33,44);
}