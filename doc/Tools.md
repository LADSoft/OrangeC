# Tools - Table Of Contents
 
 This document serves as a table of contents, for the documentation on the Orange toolchain.


## General Tools
 
 General tools compile or assemble code, and manage the resulting object files.
 
* [OCC](occ/OCC.md) is an optimizing x86 C compiler.
* [OAsm](oasm/OAsm.md) is an x86 assembler.  It uses a syntax that is very similar to the Netwide Assembler (NASM)
* [OLib](general/OLib.md) documentation is an object file librarian.
* [OLink](olink/OLink.md) documentation - object file linker.


## Linker Postprocessing Tools

 Linker postprocessing tools take the linker output, and make some sort of device or OS-specific binary image that serves as the final executable image.
 
* [DLHex](general/DLHex.md) is the utility to make hex and binary files, for ROM-based images
* [DLMZ](general/DLMZ.md) is the utility to make 16-bit MSDOS executables.
* [DLLE](general/DLLE.md) is the utility to make 32-bit MSDOS executables that aren't windows compatible.
* [DLPE](general/DLPE.md) is the utility to make Windows 32-bit executables.


## Utilities

 Utilities are external utilites that aren't generally needed for developing programs, but are somewhat useful.
 
* [OCPP](general/OCPP.md) is a C and assembly language preprocessor.  It understands C89, C99, and OAsm preprocessor directive syntaxes.
* [OGrep](ogrep/OGrep.md) looks for regular expressions within source code files.
* [OMake](omake/OMake.md) is a make utility similar to GNU make.
* [onm](general/onm.md) dumps the symbols from object files
* [ieeeconvert](general/ieeeconvert.md) convert object files between binary and ascii formats

## Compiler wrappers

  Compiler wrappers are wrappers that convert the command line for other compilers to a format that OCC can use.
  Currently we support the basic tools from GCC and MSVC as wrappers.

### GCC

* [gccocc](wrappers/gccocc.md) wrapper for the gcc command line
* [arocc](wrappers/arocc.md) wrapper for the ar command line

### MSVC

* [clocc](wrappers/clocc.md) wrapper for the cl.exe command line
* [libocc](wrappers/libocc.md) wrapper for the lib.exe command line
* [linkocc](wrappers/linkocc.md) wrapper for the link.exe command line

## WIN32 Specific tools
 
 WIN32 specific tools are tools that aid in the development of WIN32 programs.
 
* [OImpLib](general/OImpLib.md) is a WIN32 import librarian.
* [ORC](general/ORC.md) is a WIN32 resource compiler.
* [Coff2IEEE](general/Coff2IEEE.md) converts import libraries from the COFF format to the Orange C format.
* [OBRC](general/Obrc.md) Orange C Browse Compiler
* [OCCPR](general/Occpr.md) Orange C Code Completion compiler

## Debugging without a debugger

* [lsdbghelper.dll](general/lsdbghelper.md) is used to create a stack trace

## Tools Setup
 
 The tools come with configuration files that set up their run-time enviroment. These are automatically read relative to the binaries, therefore only the OrangeC bin directory must be on PATH.

For example, if the tools are installed in c:\\orangec, the following command line command is enough to set up the OrangeC environment:

>     PATH=c:\\orangec\\bin;%PATH%

There is also an ORANGEC environment variable.   Usually it doesn't need to be set, but if you have multiple installations of orange c on the path you may want to set it to avoid ambiguity.   It is set to the OrangeC root directory:

>     set ORANGEC=c:\\orangec