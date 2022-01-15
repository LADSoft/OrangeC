# lsdbghelper.dll

lsdbghelper.dll is a helper dll that resides in the %ORANGE%\bin directory.   When present, it will be used if a program crashes, to generate a stack trace.    The stack trace will be slightly different, depending on how the program was compiled.

We will use this program, t14.cpp, as an example:

```
#include <stdio.h>
#include <string.h>
int a1()
{
	int a;
	printf("hi");
//	strcat(0, 0);
	*(char *)0 = 4;
}
int a2()
{
	int a;
	printf("hi");
	a1();
}
int a3()
{
	int a;
	printf("hi");
	a2();
}
int a4()
{
	int a;
	printf("hi");
	a3();
}
main()
{
	printf("hi");
	a4();
}
```

For a normal compilation, the stack trace will just give addresses.   Following is an an example of a program, t14.cpp, with a crash in main():

```
Access Violation:(C:\orangec\src\occparse\temp\t14.exe)
CS:EIP 0023:00401054  SS:ESP 002B:0019FF18
EAX: 00000002  EBX: 00000000  ECX: 0042CF0C  EDX: 00000000  flags: 00010206
EBP: 0019FF68  ESI: 00000000  EDI: 00401000
 DS:     002B   ES:     002B   FS:     0053   GS:     002B


CS:EIP  C6 05 00 00 00 00 04 33 C0 59 59 59 59 C3 68 0C

Stack trace:
                        401054
```

Here the first address is where it crashed; this will always be valid.   There may be more addresses given if the crash is nested inside multiple function calls.   But bear in release mode the compiler aggressively optimizes EBP and this may cause the stack trace to be incomplete.

There may not be more than one address here.   Causes could be a crash in an external dll; or it is in main(); or EBP has been used for something other than marking stack frames.

Compiling the program with the compiler switch _-pl-mx_ results in a detailed map file t14.map.  The first few lines of the 'Publics By Name' section of the file are as follows:

```
Publics By Value

401012   startupStruct
40101a   main
401062 X a1()
401075 X a2()
401095 X a3()
4010c2 X a4()
40113c X __startup
401374 X __crtexit
```

The failing address in the stack trace is between main() and aa1(), so, the failure is in the main() you can use a debugger to hone in on exactly where the crash is if you want.

Compiling with /C-E turns off the EBP optimizations, and gives a more accurate stack trace.

To get more detail, compile with _-g_ (debugging) enabled.   This will turn off inlining and other optimizations, and make it easier to hone in on the problem.

When t14.cpp is compiled with _-g_ the output is:

```
Access Violation:(C:\orangec\src\occparse\temp\t14.exe)
CS:EIP 0023:00401029  SS:ESP 002B:0019FEF4
EAX: 00000002  EBX: 00000000  ECX: 0042CF0C  EDX: 00000000  flags: 00010216
EBP: 0019FEF8  ESI: 00000000  EDI: 00401000
 DS:     002B   ES:     002B   FS:     0053   GS:     002B


CS:EIP  C6 05 00 00 00 00 04 8B E5 5D C3 55 8B EC 51 68

Stack trace:
                        401029: a1() + 0xf  module: t14.cpp, line: 8
                        401048: a2() + 0x14  module: t14.cpp, line: 14
                        401060: a3() + 0x14  module: t14.cpp, line: 20
                        401078: a4() + 0x14  module: t14.cpp, line: 26
                        40108f: main + 0x13  module: t14.cpp, line: 31
                        40128f: __startup + 0x1bb
```

here there are multiple addresses, because compiling with /g turned off inlining and used
the expanded versions of the functions.

here you can see that the stack trace has additional information about the crash.

if we want we can look at the map file again (use the _-pl-mx_ compiler switch)

```
Publics By Value

401012   startupStruct
40101a X a1()
401034 X a2()
40104c X a3()
401064 X a4()
40107c   main
```

here we can see that the first address, where the crash occurs, fits in the a1() function.
