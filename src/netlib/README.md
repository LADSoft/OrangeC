# DotNetPELib

DotNetPELib is a library which abstracts managed information such as namespaces, classes, fields, methods, properties, and instructions.  The information can then be used to generate assembly language source files, or PE executables or DLLs.

The library has been tested as the backend for the OCCIL compiler, and is reasonably functional.  That said this version of the library does have various limitations; not all aspects of the managed environment are enabled at this time.

In addition to being able to generate .net programs, version 2.0 of the library has support for importing managed assemblies, and strong names.   See 'changelog.txt' for a full description of the changes.

Version 2.2 of the library introduces read/write of a simple object file format in the familiar object-file-per-module paradigm, along with a linker.

A simple test program is included which generates several example executables in both IL and EXE format.

The project files are for Visual Studio 2017 community edition.   This source code does use some of the new features in C++11.

