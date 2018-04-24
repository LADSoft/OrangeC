## OCC Extended Keywords

 
 Extended keywords extend ANSI C in a variety of ways that are sometimes useful for example to add new functionality (such as alloca or typeof) or to ease integration with operating systems and programming languages (for example \_stdcall or \_pascal).


### \_absolute

  
     create a variable at an absolute address.  For example:
 
     \_absolute(0x4c21) int a ;
 
     places the variable 'a' at address 0x4c21.  No storage is generated for such variables and no relocation is done.


### alloca

  
     Allocate memory from automatic storage (the processor stack).  The primary motivation for using this function is that it is much faster than the malloc() function.
 
     alloca is implicitly defined by the compiler as follows:
 
     void \*alloca(size\_t size);
 
     For example:
 
     int size = 24;
     int \*p = alloca(size \* sizeof(int));
 
     will allocate enough space to store an array of 24 integers.
 
     alloca allocates space without checking there is enough.  If the space used by calls to this pseudo-function plus the space used by lower level functions and their data exceeds the stack size, the program will probably crash.  
 
     Memory allocated by alloca is normally freed at the end of the function it appears in, which makes it possible to allocate a lot of data in a loop.   However, if a block has both a call to alloca and uses variable length arrays, at the end of the block the variable length arrays will be freed, which will also result in freeing the memory allocated by alloca.


### \_cdecl

 
     use standard C language linking mechanism (here for compatibility with other compilers).  In this linking mechanism, a leading underscore is prepended to the name, which is case sensitive.  The caller is responsible for cleaning up the stack.  For example:
 
     void \_cdecl myfunc() ;
 
     creates a function myfunc with standard linkage.


### \_export

 
     make an export record for the linker to process.  The current record becomes an entry in the EXE files export table.  For example:
 
     void \_export myfunc() ;
     
     will cause myfunc to be an exported function.


### \_genbyte

 
 
     Generate a byte of data into the code segment associated with the current function.  For example:
 
     void myfunc()
     {
     .
     .
     .
     \_genbyte(0x90) ;
     .
     .
     .
     }
 
     puts a NOP into the code stream.


### \_import

 
     This can be used for one of two purposes.  First it can make import record for the linker to process, which will result in the appropriate DLL being loaded at run-time.  Second, it can be used to declare a variable from a DLL so that the compiled code can access it.  For example:
 
     int \_import myvariable ;
 
     declares myvariable as being imported.  While
 
     int \_import("mylib.dll") myvariable ;
 
     declares myvariable as being imported from mylib.dll.


### \_interrupt & \_fault

 
     Create an interrupt function.  Pushes registers and uses an IRET to return from the function.  For example:
 
     \_interrupt void myfunc() 
     {
     }
 
     Creates a function myfunc which can be used as an interrupt handler.
 
     \_fault is similar to \_interrupt, but also pops the exception word from the stack.  Used when returning from certain processor fault vectors


### \_loadds

  
     For an Interrupt function, force DS to be loaded at the beginning of the interrupt. This will be done by adding 0x10 to the CS selector to make a new DS selector.  For example:
 
     \_loadds \_interrupt void myfunc() 
     {
     }
 
     will create an interrupt function that loads DS


### \_pascal

          
     use PASCAL linking mechanism.  This linking mechanism converts the function name to upper case, removes the leading underscore, pushes arguments in reverse order from standard functions, and uses callee stack cleanup.  For example:
 
     void \_pascal myfunc() ;
 
     Creates a function myfunc with pascal linkage.


### \_stdcall

  
     Use STDCALL linking mechanism.  This linking mechanism removes the leading underscore and uses callee stack cleanup.  For example:
 
     void \_stdcall myfunc() ;
 
     Creates a function myfunc with pascal linkage.


### typeof

  
     the typeof operator may be used anywhere a type declaration is used, e.g. as the base type for a variable or in a cast expression.  It allows you to access the variable's type without explicitly knowing what that type is.  For example:
 
     long double aa ;
     typeof(aa) bb;
 
     declares bb as long double type.
 
   