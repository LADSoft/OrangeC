# Simple-MSIL-Compiler

This is a version of the Orange C compiler that does MSIL code generation for the .NET Framework.

Build Status: 
[![Build status](https://ci.appveyor.com/api/projects/status/omahydb3qgu5f6dx?svg=true)](https://ci.appveyor.com/project/LADSoft/simple-msil-compiler)


This is a WIP.  At present it mostly supports the C language.  

This version supports common RTL variables such as `stdin`, `stdout`, `stderr`, `errno`, and the variables used for the macros in `ctype.h`.   It also supports command line arguments.
 
This version supports marshalling of function pointers.  A small helper dll called `occmsil.dll` is involved in creating thunks for this.  This helper dll is built when you build the compiler. 

Calling unprototyped functions now results in an error.

The results are undefined if you try to use some extension such as `alloca`.

There may be a variety of bugs.

The sources for this version build independently from the main Orange C branch.   The include files from the ORANGEC compiler exist in this repository as well, an [Appveyor CI](https://ci.appveyor.com/project/LADSoft/simple-msil-compiler) project uses them and some other static data to build an installation setup file after each checkin.  All sources in this package have a VS2017 community edition solution.

Run the compiler `occil` on a simple C program (`test.c` is included as an example).

`occil` automatically loads symbols from `mscorlib`.

## Additions to the language to support .NET

* `__unmanaged` is used to clarify that a pointer in a structure is a pointer to an unmanaged function.   Usually the compiler can figure out whether a function pointer is managed or unmanaged, but in this case the definition is ambiguous 	and it defaults to managed.
* `__string` declare an MSIL string.  Constant strings will be loaded with the .NET `ldstr` instruction instead of being treated as C language strings.  Note that this means they are wide character strings.  You can natively concatenate strings, pass them to functions, and return them.  You could also use `mscorlib` functions to perform other functions.  The same syntax as used for 'C' language strings is used for these strings.   Usually the string usage can be auto detected from context, but in rare situations the compiler will consider such a string ambiguous and you have to cast it:   `(__string) "hi"`
* `__object` declare an MSIL object.  Has minimal use for the moment.   If you cast to `object` you will box the original value.
* `__cpblk` invokes the cpblk MSIL instruction.   It is invoked with arguments similar to `memcpy`.
*  `__initblk` invokes the `initblk` MSIL instruction.   It is invoked with arguments similar to `memset`.
* '__property' declares a .net property.   Only simple variables at global scope can be declared as properties; they are currently never instance variables. (properties imported from other assemblies can be instance variables).  the /N command line switch must be used to create a containing class (.Net will not allow properties outside of classes).   
For example:
	'__property int a; // creates a property, in this case a backing variable and appropriate getters and setters
				are automatically created.
	__property int b { get { return 5;} set { printf("%d\n", value); } }; // here 'value' is the value we are 
				setting it to.
* '__entrypoint' classifies a function as an entrypoint.   If you add this to a function the normal C startup will be ignored and the function will be called as the main function.
*  `native int` is a new type to support the 'native' int size of MSIL.
*  C++ `&` operator: When used on a function parameter, makes the parameter a `ref` parameter.  No other current use is possible.   For example: `int myfunc(int &a);`
*  C++ namespace qualifiers may be used to reference a function in a loaded assembly.  Since `mscorlib` is always preloaded, the following is always possible:   `System::Console::WriteLine("hello, world!");`.   It is also possible to use the using directive:  `using namespace System;` and then write"  `Console::WriteLine("Hello, world!");`
*  Basic types will automatically be converted to their 'boxed' type for various purposes.   For example:
	`using namespace System;
	int aa = 5;
	Console::WriteLine(aa.ToString());`
* Instance members may also be called: `aa.ToString();`.
* Managed arrays: when the array indexes appear before the variable name, the array is allocated as either an MSIL array or a multidimensional array of objects.   Such arrays can only be used or passed to functions; you cannot do anything that would be equivalent to taking the address of the related managed objects. For example: `int [5]aa;`.
* C++ constructors: Will use `newobj` to call a managed version of the constructor and create an object
* MSIL Strings: `@"<string>"` makes an msil string.   As in C#, it is taken literally.  If you need to use escape sequences but still want an MSIL string (because in some contexts the c-style string won't be auto-converted) use string concatenation: `@"""<C-style escaped string>"`
* `#pragma netlib <library>` allows loading a .net assembly programmattically.   For example to get access to MessageBox do the following:
	`#pragma netlib System.Windows.Forms`
	`using namespace System::Windows::Forms`
	...
	MessageBox.Show("hello","message box");
* occil handles most SEH constructs now.   For example:
	__try {
		File.Open("some file that doesn't exist", FileMode::Open);
	}
	__catch (Exception e) {
		Console::WriteLine(e.Message);
	}
	__finally {
		printf("Finally!");
	}
## Runtime Environment
Set the environment variable `OCCIL_ROOT` to the Orange C path for it to find include files and add the OCCIL executables to the path:

    set OCCIL_ROOT=c:\orangec
    path %OCCIL_ROOT%\bin;%PATH%

## Implementation Notes
This compiler will generate either a .EXE or .DLL file, or alternately a .il file suitable for viewing or compiling with ilasm.   Additionally, the compiler is capable of generating object files in the familiar object-file-per-module paradigm that can be linked with a linker called `netlink`.   This linker is also part of the package.   The compiler uses an independent library `dotnetpelib` to create the output.

### MSIL Limitations
1) Extended precision found in `long double` type is missing  in MSIL - `long double` is synonomous with `double`.
2) You can't put variable length argument lists on variables which are pointers to functions and then pass them to unmanaged code.  You can however use variable length argument lists on pointers to functions if you keep them managed.
3) Initialization of static non-const variables must be done programmatically rather than 'in place' the way a normal C compiler does it - so there are initialization functions generated for each module.   This impacts startup performance.

This compiler will compile either an EXE or a DLL.  The package generally defaults to compiling everything into the unnamed MSIL namespace, however, for interoperability with C# it is necessary to wrap the code into a namespace and an outer class.  A comm
and line switch conveniently specifies this wrapper.

This compiler is capable of auto-detecting DLL entry points for unmanaged DLLs, so for example you can specify on the command line that the compiler should additionally import from things like `kernel32.dll` and/or `user32.dll`.   This still requires header support so that prototypes can be specified correctly however.   This compiler is designed to work with the same headers that Orange C for the x86 uses.  

This compiler is capable of importing static functions from .NET assemblies.

By default the compiler automatically imports the entry points for `msvcrt.dll`, and the `occmsil.dll` used for function pointer thunking.  A .NET assembly `lsmsilcrtl` is used for runtime support - mostly it performs malloc and free in managed code and exports some functions useful for handling variable length argument lists and command lines.  `mscorlib`
is also automatically loaded and its static functions are available for use.

It is possible to have the compiler combine multiple files into a single output; in this way it performs as a psuedo-linker as well.   Simply specify all the input files on the command line.   The compiler takes wildcards on the command line so you can do something like this for example to compile all the files, linking against several Win32 DLLs, and giving it an outer namespace and class to be able to reference from C#:   

    occil /omyoutputfile *.c /Wd /Lkernel32;user32;gdi32 /Nmynamespace.myclass

The `/Wd` switch means make a DLL.  `/Wg` means Windows GUI.   `/Wc` (the default) means Windows console.   Adding `l` on the end of a `/W` switch (e.g. `/Wcl`) means load
`lscrtlil.dll` as the unmanaged runtime, instead of `msvcrt.dll`.   You might want to do this to get access to C99 and C11 functions.

The compiler will create structures and enumerations for things found in the C code, that can be used from C#.   Unlike older versions, in this version pointers are mostly typed instead of being pointers to void.

This compiler will also enable C# to call managed functions with variable length argument lists.  

The `/L` switch may now also be used to specify .NET assemblies to load.  `mscorlib.dll` is automatically loaded by the compiler.

The switch '-d' can be used to tell the compiler to stick to plain C and not accept C#-like extensions.

Beyond that this is a C11 compiler, but some things currently aren't implemented or are implemented in a limited fashion:

1) Complex numbers aren't implemented.
2) Atomics aren't implemented.
3) Thread and thread local storage aren't implemented.
4) Runtime library is `msvcrtl.dll`, and doesn't support C11 or C99 additions to the C RTL.
5) Arrays aren't implemented as managed arrays but as pointers to unmanaged memory.
6) Array types are actually implemented as .NET classes.
7) Variable length argument lists are done in the C# style rather than in the C style - except during calls to unmanaged functions.
8) Variable length argument lists get marshalling performed when being passed to unmanaged code, but this only handles simple types.
9) Thunks are generated for pointers-to-functions passed between managed and unmanaged code (e.g. for `qsort` and for `WNDPROC` style functions) but when the pointers are placed in a structure you need to give the compiler a hint.  Use `CALLBACK` in the f
unction pointer definition and make the callback a `stdcall` function.
10) In the thunks for the transition from unmanaged to managed code used by function pointers passed to unmanaged code marshalling is performed, but this only handles simple types.
11) Variable length arrays and `alloca` are implemented with a managed memory allocator instead of with the `localalloc` MSIL instruction.
12) Structures passed by value to functions get copied to temporary variables before the call.
13) Many compiler optimizations found in the native version of the compiler are currently turned off.
14) The compiler will not allow use of unprototyped functions.
