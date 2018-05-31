## OCC \#pragma directives

 \#pragma preprocessor directives control the interpretation of source code, or extend the functionality of the compiler in some way.

### \#pragma once

 \#pragma once   automatically performs the function of an include guard

 when this is encountered in a file (usually at the top of the file) occ checks to see if this file has been encountered
 before, and if so stops processing it.   To determine if a file has been encountered before, occ does not consider the file name.  If the last modification time of the file, the file size, and a CRC32 taken over the file contents all match a file that has previously been cached due to encountering a \#pragma once directive, the file is considered to have been processed before and processing of it immediately stops.


### \#pragma error

  
 \#pragma error  <text>    allows conditional generation of errors.  For example:
 
     #ifndef WIN32
     #pragma error Not a win32 program
     #endif
 
 generates a compile time error if the WIN32 macro is not defined.


### \#pragma warning

 
 \#pragma warning <text>    allows conditional generation of errors.  For example:
 
     #ifndef LONG
     #pragma warning long type not defined
     #endif
 
 generates a compile time warning if the LONG macro is not defined.


### \#pragma aux

  
 \#pragma aux <funcname> = <alias>
     Creates an alias for a function.  The alias name is substituted for the function name in the OBJ and ASM output files.  For example:
 
     #pragma aux "myfunc"="mynewname"
 
 causes the linker to see the function as being called 'mynewname'.


### \#pragma pack

  
 \#pragma pack(n)    
 
 Sets the alignment for structure members and global variables.  The default alignment is 1. Changing the alignment can increase performance by causing variable and structure alignment to optimal sizes, at the expense of using extra memory.  However, altered alignment can sometimes cause problems, for example when a structure is used directly in a network packet or as the contents of a file.
 
 The actual alignment of any given variable depends both on the value of 'n' and on the size of the variable.  CC386 will pick the minimum of the two values for the alignment of any given variable; for example if n is 2 characters will be aligned on byte boundaries and everything else will be aligned on two byte boundaries.  If n is 4 characters will be on byte boundaries, words (short quantities) on two-byte boundaries, and dwords (ints) on four byte boundaries.
 
 \#pragma pack()    
 
 Resets the alignment to the last selection, or to the default.


### Startup and rundown pragmas

  
 \#pragma startup <function> <priority>
 
 \#pragma rundown <function> <priority>
 
 These two directives allow you to specify functions that are automatically executed by the RTL before and after the main program is executed.  The priority scheme allows you to order functions in a priority order.  When the RTL is executing startup or rundown functions it executes all functions at priority 1, then all functions at priority 2, then all functions at priority 3, and so forth.  To have a function executed before your program runs, use \#pragma startup, or to have it execute after the program exits, use \#pragma rundown.  You should use priorities in the range 50-200, as priorities outside that range are used by RTL functions and their execution (or lack thereof) may prevent some functions in the RTL from working properly.  For example:
 
     \#pragma startup myfunc 100
 
runs the function 'myfunc' after the RTL functions have initialized.  Myfunc would be defined as follows:
 
     void myfunc(void) ;
 
Note that \#pragma rundown is equivalent to atexit.