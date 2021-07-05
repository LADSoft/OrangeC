## OCC Compilation Modes

### +A    disable non-ansi extensions
 
By default the compiler allows several extensions to ansi, to make coding easier.  If you want strict adherence to ansi, use this switch.  For example:
 
     OCC +A myfunc.c
 
     will enable ANSI mode
 
### +9    C99 Compatibility
 
By default the compiler compiles for pre-99 standard.  If you want extended features available in the later C99 standard, use this switch.  For example:
 
     OCC /9 myfunc.c
 
     Will enable C99 mode.

### +1    C11 Compatibility

     OCC /1 myfunc.c
 
     Will enable C11 mode.

### -x     force compilation mode

     OCC -x c++  myfunc.c

     compiles myfunc.c in c++ mode

     occ -x c myfunc.cpp

     compiles myfunc.cpp in c mode

### -nostdinc   disable system include files

     OCC -nostdinc myfunc.c

     compiles myfunc.c, without access to standard include files

     OCC -nostdinc++ myfunc.cpp

     compiles myfunc.cpp, without access to standard include files

### -std    set standards compliance flags

     OCC -std=somestandard myfunc 

     selects a standard to compile against.

     The available standards are:

         c89       - Ansi C, 1989
         c99       - Ansi C, 1999
         c11       - Ansi C, 2011
         c++11     - c++, 2011
         c++14     - c++, 2014

      also, the C++ standard settings don't actually compare the source code against the
      standard, their only function is to set the value of the __cplusplus macro.