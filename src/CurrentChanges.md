# Changes to compiler since the September 23rd, 2023 release.

Well this isn't really a nightly build...   to avoid racking up minutes we are only building when something changes.   Should be close enough.

# Release Notes

We are heading for the 7.0.0 release.    The major features in this release will be support for c++17 and support for building .NET core images with occil.   Implementation is done for both features and the functionality is there, the major holdup as far as a release goes is to finish testing the c++17 changes.


Here is the list of all the changes that have been made since the last release:


    fixes #1020, remove ocide links from install script.   Also adds support for latest occil and adds examples to the install directory
    update .gitignore, src/dosxclude.lst
    adjust the occil tests for compiler not adding .C extention any more
    Add capability to figure out an object file has a bad format to objlib
    handle . characters in filename, don't add .c extension automatically, pass files with extensions we don't recognize on to the linker
    add ifdef to output one ICF file per input file, ifdef out an MSIL function call
    compile OCC with c++14 when using OCC to compile
    Merge pull request #1029 from rochus-keller/busy_frontend : Minimal Linux implementation of SharedMemory and fixes to make     ifdef'ed DIR_SEP and PATH_SEP with TARGET_OS_WINDOWS
    fixes #1028, *(0,(struct nn *)b) is an lvalue for assignment purposes, rename some enumerations
    no / options on Linux; minimal SharedMemory imp (non-sharing); orangefront seems to work now
    Rework to make compiler/optimizer frontend build on MSVC 2015, Clang 4 and GCC 12
    Rework occil to support code generation for .net core 6.0 and later
    search GAC for system dlls before searching path; don't compile libhostfxr on linux; fix a typo
    fix; build with GCC 10 on x64
    fixes #966, occil optimize locals better, generate CONSOLE apps by default
    code rewrite, remove 'enum' keyword when possible and ifdef out constexpr when compiling for MSVC15.0
    remove qualifier from references to std::tolower (MSVC15.0 support)
    fix global unions and implement them at function scope; a few bug fixes
    call class cast operators more often
    resolve functions as function args before doing type checking
    fix a couple of use cases where 'const' does not get added to an auto type
    allow parenthesis when specifying a string constant during initialization of a string array
    fix compile warning on mingw64
    fix 'raw' string parsing
    fix a crash
    'hook' operator should resolve c++ functions
    add _TRUNCATE macro
    properly const qualify arguments to wcsicmp and friends
    Merge pull request #1023 from chasonr/zrdx-doc | Add converted and translated versions of zrdx.txt
    fixes #1025, crash when compiling lua 5.4.4
    adjust tests for compiler floating point bug fix
    fix floating point truncate, was looking at the LSB instead of one past the LSB
    Merge pull request #1022 from GitMensch/patch-2 | Update occil.md, textual and formatting changes
    refactor template.cpp into four files
    bug fixes while building library/tests/applications
    fix bug where lambda structures could have a zero length
    make compiler adjustments to compile libcpp with c++17 settings
    lambda functions: bug fixes to captured 'this' handling
    lambda functions: add captured *this
    lambda functions: add new error messages
    Merge pull request #969 from GitMensch/patch-1 | Update OCC.md : 
    #147, comparisons for exception specifier part of type
    fix bug with duplicate structure member
    add feature checking
    update some diagnostics to work properly
     #147 add inline variables
     #147 template class deduction
     also clean up enumerations related to the template structures (don't
     reuse KEYWORD enumerations)
    fix a problem with error messages not being specified correctly
    cgamma: cimag(x) was used as an lvalue
    #147, copy elision and better handling for warnings about explicit constructors
    fix some problems with debug line information
    #147 declaration in if and switch stmt
     #147 add structured binding
    bug fixes to integer template args
    #147 add variadic folding
    temporary fix to make TT_DECLARE compile in all compilers
    #147, rework expression parsing in preparation for adding variadic template folding
    #147 add 'if constexpr(...)'
    #147, add 'auto' as a non-type template paramater
    fixes #955, bugs found when compiling the latest sqlite amalgamate with occil
    add __restrict keyword
    speed up the lazy optimizer a bit
    fix bug that led to builds failing randomly
    add MEM_RESERVE flag to virtualalloc allocation
    don't use top-down allocation with virtualalloc
    merge C23 support to main line
    fix some problems with range-based for when the range is a braced initializer list, fix some other C++17 changes

