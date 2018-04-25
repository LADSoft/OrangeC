# OAsm

 
 **OAsm** is an intel x86 assembler in the spirit of the Netwide Assembler (NASM).  While it shared many features with NASM,  programs written for NASM are not 100% compatible with it.   
 
 **OAsm** converts a textual form of processor-specific instructions into numbers the computer will understand.  However, discussion of these processor-specific instructions is beyond the scope of this documentation; instead the documentation will focus on various other features **OAsm** presents to help easily create code and data.
 
 **OAsm** supports both 16 and 32-bit code.
 
 In comparison to standard Intel assemblers, **OAsm** shares some features.  But unlike most other Intel assemblers, which keep track of the type of variables, **OAsm** is typeless.  Which means any time a variable is used in an ambiguous context, type information has to be provided.  Another major difference is the interpretation of brackets...  in an Intel assembler brackets around simple variable names are often optional, with the assembler interpreting:
 
>     ace;">    mov ax,\[myvar\]
 
 and
 
>     ace;">    mov ax,myvar
 
 in the same way.  In these assemblers, an additional keyword **offset **>     ace;">is used to refer to a variable's address:
 
>     ace;">    mov ax,offset myvar
 
 However in OAsm,  the **offset **keyword is done away with.  Instead, the brackets are given a more concrete meaning.   When they exist, they indicate the value of a variable; absence of the brackets denotes the address of the variable.


## Comments

 **OAsm** understands the usual comment syntax starting with a ';' and extending to the end of the line.  For example in:
 
>     ace;">    sub   eax,edx   ; normalize
 
 everything after the semicolon is a comment and is ignored.
 
 Additionally OAsm understands C and C++ style comments since it uses an extended C language preprocessor.  For example to write a block of comments use /\* and \*/:
 
>     ace;">/\*
>     ace;">    Everything between the /\* and the \*/ is a comment
     Multiple lines may be typed.
>     ace;">\*/
 
 The final commenting style is the C++ double slash style, which is similar to ';' except uses a '//' sequence to delimit the beginning of the comment:
 
>     ace;">    sub eax,edx   // normalize


## Command Line Options

 The general form of an **OAsm** [Command Line](OAsm%20Command%20Line.md) is:
 
>     ace;">OAsm \[options\] filename-list
 
 Where _filename-list_ gives a list of files to assemble.


## Numbers and Expressions
 

 **OAsm** understands several integer number formats, as well as floating point.  It is also capable of evaluating complex [expressions](OAsm%20Numbers%20and%20Expressions.md) involving numbers, labels, and special symbols for things like the current program counter.  Some of these expressions can be evaluated immediately; others are pushed off to the linker.


## Labels

 There are three kinds of [labels](OAsm%20Labels.md).  Each Standard label may be defined at most once in a source file.  Such labels have a global context and are accessible from anywhere within the current source modules, and sometimes from other modules as well. 
 
 On the other hand Local labels inherit a context from standard labels,  and may be defined multiple times provided they are defined at most once between the occurrance of any two standard labels.  
 
 Non-local labels are sometimes useful - they share the idea of having a global context with standard labels, but don't start a new context for local labels.


## Directives

 [Directives](OAsm%20Directives.md) are psuedo-instructions to the assembler which guide how the assembly is done.  In the most rudimentary form, they can provide a mechanism for defining numeric or textual values, or reserving space.  Other directives allow breaking the code and data up into distinct sections such as CODE and DATA that can be linked against other files.
 
 Natively, directives are always enclosed in brackets, for example to define a byte of data:
 
>     ace;">myvar \[db   44\]
 
 However, the directives are redefined with default macros, so that the brackets are not necessary when typing code:
 
>     ace;">myvar db 44
 
 Macros are described further in the section on the [preprocessor](OAsm%20Preprocessor.md).
 
 Some of the macro redefinitions of the directives are simply a mapping from the non-bracket version to the bracketized version.  Other macros are more complex, adding behavior to the assembler.  For example the macros for psuedo-structures also define the structure size, and keep track of the current section and switch back to it when done.


## Preprocessor

 **OAsm** uses a [C99](C-Style%20Preprocessor%20Directives.md)-style [preprocessor](OAsm%20Preprocessor.md), which has been enhanced in various ways.  One difference is that instead of using '\#' to start a preprocessor statement, **OAsm** uses '%'.  This does not apply to the stringizing and tokenizing sequences however; those still use '\#'.
 
 [Basic extensions](Basic%20Extensions%20to%20C%20Preprocessor.md) to the preprocessor include **%assign**, which is similar to **%define** except the preprocessor will evaluate an arithmetic expression as part of the assignment.  Other extensions include [repeat blocks](Repeat%20Block%20Extensions.md) and [multiline macros](Multiline%20Macro%20Extensions.md).  There are also various types of %if and %elif statements which have been added to do [textual comparisons](Text%20Comparison%20Conditionals.md) and find out a [token type](Token%20Type%20Classification%20Conditionals.md), and a [context](Context-Related%20Extensions.md) mechanism useful for keeping track of assembler states.  For example the context mechanism might be used with multiline macros to create a set of high level constructs such as 'if-else-endif', 'do-while' and so forth.
 
 