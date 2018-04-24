# Tools - Table Of Contents

 
 This document serves as a table of contents, for the documentation on the Orange toolchain.


## General Tools

 
 General tools compile or assemble code, and manage the resulting object files.
 
     [OCC](occ/OCC.md) is an optimizing x86 C compiler.
     [OAsm](oasm/OAsm.md) is an x86 assembler.  It uses a syntax that is very similar to the Netwide Assembler (NASM)
     [OLib](general/OLib.md) documentation is an object file librarian.
     [OLink](olink/OLink.md) documentation - object file linker.


## Linker Postprocessing Tools
 

 Linker postprocessing tools take the linker output, and make some sort of device or OS-specific binary image that serves as the final executable image.
 
*     [DLHex](general/DLHex.md) is the utility to make hex and binary files, for ROM-based images
*     [DLMZ](general/DLMZ.md) is the utility to make 16-bit MSDOS executables.
*     [DLLE](general/DLLE.md) is the utility to make 32-bit MSDOS executables that aren't windows compatible.
*     [DLPE](general/DLPE.md) is the utility to make Windows 32-bit executables.


## Utilities

 
 Utilities are external utilites that aren't really needed for developing programs, but are somewhat useful.
 
*     [OCPP](general/OCPP.md) is a C and assembly language preprocessor.  It understands C89, C99, and OAsm preprocessor directive syntaxes.
*     [OGrep](ogrep/OGrep.md) looks for regular expressions within source code files.
*     [OMake](omake/OMake.md) is a make utility similar to GNU make.


## WIN32 Specific tools
 

 
 WIN32 specific tools are tools that aid in the development of WIN32 programs.
 
 *    [OImpLib](general/OImpLib.md) is a WIN32 import librarian.
 *    [ORC](general/ORC.md) is a WIN32 resource compiler.


## Tools Setup


 
 The tools come with configuration files that set up their run-time enviroment, however there are a couple of external dependencies.  First, the tools bin directory must be on the path.  Second, an environment variable ORANGEC must be set up to point to the tools directory.

For example, if the tools are installed in c:\\orangec, the following command line commands could be used to set up their environment:

> set ORANGEC=c:\\orangec
> PATH=c:\\orangec\\bin;%PATH%

 
