### Multiline Macro Extensions

 
 Multiline macro extensions allow definition of types of macros that are more familiar to assembly language programmers.  Such macros may contain an arbitrary number of assembly language statements and preprocessor directives.  These macros have three parts:  the macro header, the macro body, and the macro invocation.
 
 The macro header and macro body are used when defining the macro.  For example a simple multiline macro that gives a new name to NOP might look as follows:
 
>     %macro MY_NOP 0
>         nop
>     %endmacro
 
 here MY_NOP is the name of the macro, which is case-sensitive since %macro was used rather than %imacro.  The zero in the header following MY_NOP indicates this macro has no parameters.  The body of the macro is the 'nop' statement, and the macro definition ends with %endmacro.
 
 After the macro is defined, it can be invoked as many times as necessary.  For example:
 
>     MY_NOP
>     MY_NOP
 
 causes the assembler to assemble two 'nop' statements based on the above definition.
 
 A macro can have parameters:
 
>     %macro MY_MOV 2
>         mov   %1, %2
>     %endmacro
 
 In this macro, the 2 in the header signifies the macro takes exactly two arguments.  Specifying more arguments, or less, during and invocation, will result in an error.  The %1 and %2 are substituted with the first and second arguments.  For example:
 
>     MY_MOV eax,ebx
 
 gets translated to:
 
>     mov eax,ebx
 
 
 Macros can have a variable number of parameters.  In:
 
>     %macro MY_PUSH 1-4
>     %rep %0
>         push %1
>         %rotate 1
>     %endrep
>     %endmacro
 
 the header specifies the macro can have between one and four arguments.  In the body, the %0 gives the number of arguments that were actually specified.  The %rotate rotates the arguments left by one, so that the contents of %2 moves to %1, the contents of %3 moves to %2, and so forth.  The contents of %1 moves to the end of the list.
 
 Invoking this macro as follows:
 
>     MY_PUSH eax,ebx,ecx
 
 results in the preprocessor generating the following instructions:
 
>     push eax
>     push ebx
>     push ecx
 
 This can be made just a little bit better through the use of the infinite argument list specifier:
 
>     %macro MY_PUSH 1-*
>     %rep %0
>         push %1
>         %rotate 1
>     %endrep
>     %endmacro
 
 where the \* in the header means there is no practical limit to the number of arguments (there is a limit, but it wouldn't be realistic to type that many arguments even with a code-generating program).  Now the macro isn't limited to four push statements; it can push as many things as are listed in the macro invocation.
 
 A corresponding MY_POP can be created with minor changes:
 
>     %macro MY_POP 1-*
>     %rep %0
>         %rotate -1
>         pop %1
>     %endrep
>     %endmacro
 
 where the rotate statement now shifts right instead of left, with the rightmost argument appearing in %1.
 
 Occasionaly it is beneficial to specify that you need some arguments, then you want the rest of the command line:
 
>     %define STRINGIZE(s) \s
>     %macro MY_MSG 1+
>     db %1,STRINGIZE(%2)
>     %endmacro
 
 Where the + symbol means anything beyond the first argument should be gathered together and make another argument.  This does include comma characters; after the first argument's separating comma commas will no longer be processed with this syntax.  As an example invocation:
 
>     MY_MSG 44, hello there, world
 
 would translate to:
 
>     db   44,"hello there, world"
 
 Of course the + symbol may be combined with specifying variable length argument lists as shown in the following header:
 
>     %macro do_something 1-4+
 
 Another use for the + symbol is to get the entire argument list of a macro invocation, unparsed, as shown in the following header:
 
>     %macro do_something 0+
 
 Sometimes it is useful to include the comma character in the argument for a macro invocation:
 
>     %macro define_numbers 3
>     db   %1,%2,%3
>     %endmacro
 
 can be invoked with:
 
>     define_numbers {1,2},3,4
 
 to result in:
 
>     db   1,2,3,4
 
 When variable length argument lists are used, everything starting with the first variable argument can have a default value.  For example with the macro header:
 
>     %macro define_strings 1-4 "hello", "there"
>     %rep %0
>         db   %1
>         %rotate 1
>     %endrep
>     %endmacro
 
 defaults the second and third arguments to "hello" and "there" respectively, if they are not specified in the invocation.  For example:
 
>     define_strings "one"
 
 results in:
 
>     db "one"
>     db "hello"
>     db "there"
 
 whereas define_strings "one", "two"
 
 results in:
 
>     db "one"
>     db "two"
>     db "there"
 
 Many of the above macros are unexciting, and perform functions that could be done other ways e.g. with **%define**.  A more interesting example of a multiline macro is as follows:
 
>     %macro power 2
>         mov ecx,%1
>         mov eax,1
>         jecxz %noop
>         mov eax,%2
>         cmp ecx,1
>         jz   %noop
>     %local:
>         imul eax,%2
>         loop %local
>     %noop:
>     %endmacro
 
 which creates code to raise the second argument to the power of the first argument, and leaves the result in eax.  An example invocation:
 
>     power 2,3
 
 which generates code to return 3 squared in eax.  Here we have introduced local labels in macros, which are similar in form to local labels in contexts, except that the macro version does not have a dollars symbol.  Such local labels are in scope for a single invocation of the macro;  each time the macro is invoked the label will have a different context.  
 
 As with context-specific labels, the assembler does not implement multiple symbol tables but instead uses a non-local label name.  The non-local label name consists of ..@ followed by a context id followed by a period followed by the label name.  For example, the labels in the above example would be translated to:
 
 ..@54.local
 ..@54.noop
 
 if the context identifier for the current macro invocation is 54.  non-local labels fitting this general format should not appear in the source code, as there is a chance they will conflict with label names chosen by the preprocessor.


#### %macro

 **%macro** starts a macro definition.  The name of the macro is case-sensitive.


#### %imacro

 **%imacro** starts a macro definition.  The name of the macro is not case-sensitive.


#### %endmacro

 **%endmacro** ends a macro definition.


#### %rotate

 **%rotate** rotates the macro argument list for the current invocation a number of times specified in the argument.  If the number of times is positive, the arguments are rotated left, with the leftmost arguments going to the end of the list.  If the number of times is negative, the arguments are rotated right, with the rightmost arguments going to the beginning of the list.
 
   