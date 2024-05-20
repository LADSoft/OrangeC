OrangeC
=======

OrangeC Compiler And Tool Chain

The Orange C compiler is a C/C++ compiler which is an entirely new code base.   It includes all the standard tools including compiler, linker, librarian, rc compiler and also includes a new concept called a 'downloader', which post-processes the output of the link stage to make a final target executable or binary image.

The long-term goal is to make this toolchain easily retargetable, however for now it is just a WIN32 compiler.

This toolchain is mostly standard C/C++, with some windows specific code in the make program.  It has been compiled by several other compilers include msvc, gcc, and clang.

The toolchain handles up to the C11 standard and C++2014.

Orange C is released under the GNU license version 3.

[Click here to see documentation](Tools.md)