### Multiline Macro Extensions

 
 Multiline macro extensions allow definition of types of macros that are more familiar to assembly language programmers.  Such macros may contain an arbitrary number of assembly language statements and preprocessor directives.  These macros have three parts:  the macro header, the macro body, and the macro invocation.
 
 The macro header and macro body are used when defining the macro.  For example a simple multiline macro that gives a new name to NOP might look as follows:
 
>     ace;">%macro MY\_NOP 0
>     ace;">    nop
>     ace;">%endmacro
 
 here MY\_NOP is the name of the macro, which is case-sensitive since %macro was used rather than %imacro.  The zero in the header following MY\_NOP indicates this macro has no parameters.  The body of the macro is the 'nop' statement, and the macro definition ends with %endmacro.
 
 After the macro is defined, it can be invoked as many times as necessary.  For example:
 
>     ace;">MY\_NOP
>     ace;">MY\_NOP
 
 causes the assembler to assemble two 'nop' statements based on the above definition.
 
 A macro can have parameters:
 
>     ace;">%macro MY\_MOV 2
>     ace;">    mov   %1, %2
>     ace;">%endmacro
 
 In this macro, the 2 in the header signifies the macro takes exactly two arguments.  Specifying more arguments, or less, during and invocation, will result in an error.  The %1 and %2 are substituted with the first and second arguments.  For example:
 
>     ace;">MY\_MOV eax,ebx
 
 gets translated to:
 
>     ace;">mov eax,ebx
 
 
 Macros can have a variable number of parameters.  In:
 
>     ace;">%macro MY\_PUSH 1-4
>     ace;">%rep %0
>     ace;">    push %1
>     ace;">    %rotate 1
>     ace;">%endrep
>     ace;">%endmacro
 
 the header specifies the macro can have between one and four arguments.  In the body, the %0 gives the number of arguments that were actually specified.  The %rotate rotates the arguments left by one, so that the contents of %2 moves to %1, the contents of %3 moves to %2, and so forth.  The contents of %1 moves to the end of the list.
 
 Invoking this macro as follows:
 
>     ace;">MY\_PUSH eax,ebx,ecx
 
 results in the preprocessor generating the following instructions:
 
>     ace;">push eax
>     ace;">push ebx
>     ace;">push ecx
 
 This can be made just a little bit better through the use of the infinite argument list specifier:
 
>     ace;">%macro MY\_PUSH 1-\*
>     ace;">%rep %0
>     ace;">    push %1
>     ace;">    %rotate 1
>     ace;">%endrep
>     ace;">%endmacro
 
 where the \* in the header means there is no practical limit to the number of arguments (there is a limit, but it wouldn't be realistic to type that many arguments even with a code-generating program).  Now the macro isn't limited to four push statements; it can push as many things as are listed in the macro invocation.
 
 A corresponding MY\_POP can be created with minor changes:
 
>     ace;">%macro MY\_POP 1-\*
>     ace;">%rep %0
>     ace;">    %rotate -1
>     ace;">    pop %1
>     ace;">%endrep
>     ace;">%endmacro
 
 where the rotate statement now shifts right instead of left, with the rightmost argument appearing in %1.
 
 Occasionaly it is beneficial to specify that you need some arguments, then you want the rest of the command line:
 
>     ace;">%define STRINGIZE(s) \#s
>     ace;">%macro MY\_MSG 1+
>     ace;">db %1,STRINGIZE(%2)
>     ace;">%endmacro
 
 Where the + symbol means anything beyond the first argument should be gathered together and make another argument.  This does include comma characters; after the first argument's separating comma commas will no longer be processed with this syntax.  As an example invocation:
 
>     ace;">MY\_MSG 44, hello there, world
 
 would translate to:
 
>     ace;">db   44,"hello there, world"
 
 Of course the + symbol may be combined with specifying variable length argument lists as shown in the following header:
 
>     ace;">%macro do\_something 1-4+
 
 Another use for the + symbol is to get the entire argument list of a macro invocation, unparsed, as shown in the following header:
 
>     ace;">%macro do\_something 0+
 
 Sometimes it is useful to include the comma character in the argument for a macro invocation:
 
>     ace;">%macro define\_numbers 3
>     ace;">db   %1,%2,%3
>     ace;">%endmacro
 
 can be invoked with:
 
>     ace;">define\_numbers {1,2},3,4
 
 to result in:
 
>     ace;">db   1,2,3,4
 
 When variable length argument lists are used, everything starting with the first variable argument can have a default value.  For example with the macro header:
 
>     ace;">%macro define\_strings 1-4 "hello", "there"
>     ace;">%rep %0
>     ace;">    db   %1
>     ace;">    %rotate 1
>     ace;">%endrep
>     ace;">%endmacro
 
 defaults the second and third arguments to "hello" and "there" respectively, if they are not specified in the invocation.  For example:
 
>     ace;">define\_strings "one"
 
 results in:
 
>     ace;">db "one"
>     ace;">db "hello"
>     ace;">db "there"
 
 whereas define\_strings "one", "two"
 
 results in:
 
>     ace;">db "one"
>     ace;">db "two"
>     ace;">db "there"
 
 Many of the above macros are unexciting, and perform functions that could be done other ways e.g. with **%define**.  A more interesting example of a multiline macro is as follows:
 
>     ace;">%macro power 2
>     ace;">    mov ecx,%1
>     ace;">    mov eax,1
>     ace;">    jecxz %noop
>     ace;">    mov eax,%2
>     ace;">    cmp ecx,1
>     ace;">    jz   %noop
>     ace;">%local:
>     ace;">    imul eax,%2
>     ace;">    loop %local
>     ace;">%noop:
>     ace;">%endmacro
 
 which creates code to raise the second argument to the power of the first argument, and leaves the result in eax.  An example invocation:
 
>     ace;">power 2,3
 
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

>     ace;">**%rotate** rotates the macro argument list for the current invocation a number of times specified in the argument.  If the number of times is positive, the arguments are rotated left, with the leftmost arguments going to the end of the list.  If the number of times is negative, the arguments are rotated right, with the rightmost arguments going to the beginning of the list.
 
   