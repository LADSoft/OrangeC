# clocc

 **clocc** is a wrapper around the msvc cl command line.   It reinterprets the command line arguments and issues them to occ

## Command Line Options

 The general format of an **clocc** command line is:
 
>     clocc [options] [list of files]
 
where options are various command line options that can be used to customize the behavior, and the list of files lists files to be compiled/assembled/linked

### Compiling options

Normally the compiler will compile a C or C++ program, and link it.   There are some options to control that behavior, however.

>     clocc -c file.c

compiles a C file without linking

>     clocc -Fooutput.o file.c

compiles a C file without linking and names the output output.o


>     clocc file.cpp

compiles a C++ file, linking it


>     clocc /Feoutput.exe file.cpp

compiles a C++ file, linking it and names the output output.exe

>     clocc /P file.cpp

preprocess file.cpp

>     clocc /Fioutput.i file.cpp

preprocess file.cpp, naming the output output.i

>     clocc /Fahi.asm file.cpp

assemble the file file.cpp, nameing the output hi.asm
note this command does not work precisely the way CL.EXE works;
cl.exe compiles via assembly (generating the output file, a .o file, and a .exe file;
clocc only generates an assembly file.

>     clocc /Dmymacro=myvalue  file.cpp

compile file.cpp after defining the preprocessor macro 'mymacro' to have a value 'myvalue'

>     clocc /Umymacro

compile file.cpp after causing the preprocessor macro mymacro to not be definable

>     clocc /I.\hello /I..\hello file.cpp

compile file.cpp, giving a list of include paths to search for include files on

>     clocc /F1000000 file.cpp

the output file file.exe should have a maximum stack size of 1000000 bytes (normally you would use a power of two)

>     clocc /K file.cpp

CHAR should be defined as unsigned character

>     clocc /LD file.cpp

the output file will be a dll and have a .dll extension

>     clocc /MD file.c

use MSVCRT.DLL as the runtime library.   Usually not too useful for C++ programs...

>     clocc /TC file.cpp

compile all files on the commad line as if they are C language files

>     clocc /TP file.cpp

compile all files on the commad line as if they are C++ language files

>     clocc /std:value file.x

choose the standards value to use.   The compiler mode will still be chosen by the file extension, or the TC or TP switches

valid values are:
    C89
    C99
    C11
    C++11
    C++14

>     clocc /Za file.c

compile, disabling extensions

>     clocc /Ze file.c

compile, enabling extenstions

>     clocc /Zi   file.c

compile for debug
note this departs a little from cl.exe, in that it also forces optimizations off

>     clocc /wxxx   file.c

set warning options.

     /wd#     disable a warning
     /we#     treat a warning as an error

note that warning numbers aren't translated; it is assumed you will use the OrangeC version of the warning number

>     clocc   /Wxxx file.c

set more warning options

     /Wall    enable all warnings
     /WX      treat warnings as errors

>     clocc   /Ox file.c

set optimizer mode

valid modes are:
     
     /Od       disable optimizations
     /O1       optimize for size
     /Os       optimize for size
     /O2       optimize for speed
     /Ot       optimize for speed
     /Ox       optimize for speed
     /Oy[-]    enable[disable] stack frames

>    clocc    /RTCs file.c

compile for stack object runtime checking

>    clocc    /RTCu   file.c

compile for uninitialized variable checking

pass an option to the linker
this switch will pass some basic options to the linker, but since it passes them to olink
rather than linkocc this will be a subset of the available options in linkocc.

see * [linkocc](linkocc.md) for details about linker options

The following options are supported:

     /DEBUG
     /LIBPATH
     /BASE
     /FILEALIGN
     /IMPLIB
     /MAP
     /STACK
     /SUBSYSTEM

The following options are accepted but ignored:

     /ENTRY
     /EXPORT
     /MACHINE
     /MANIFESTFILE
     /NODEFAULTLIB
     /OPT
     /PROFILE
     /RELEASE
     /SAFESEH
     /STUB
     /WX
     /DEF

### Compatibility commands

Several commands are parsed, but don't do anything.   These include

    cl /E file.c
    cl /Fmmapfile file.c
    cl /Fm:mapfile file.c
    cl /MPx file.c
    cl /Tcfile1.cpp file.c file1.cpp
    cl /Tpfile1.c   file.cpp file1.c
    cl /wo# file1.c
    cl /wl# file1.c
    cl /w# file1.c
    cl /WL file1.c
