### Basic Extensions to C Preprocessor

 
 
 The basic extensions to the C Preprocessor generally have functionality that is very similar to the functionality already found in the C Preprocessor, but extends it in some way.


#### %assign

 **%assign** is similar to the non-functional form of **%define**, in that it defines text to be substituted for an identifier.  Where **%assign** differs is that the text to be substituted is evaluated as if it were an expression at the time the **%assign** is encountered.  This is useful for example in **%rep** loops.  For example the following code makes a data structure that consists of the integers from 1 to 100, in ascending order:
 
> %assign WORKING 1
> %rep 100
>     db WORKING
> %assign WORKING WORKING + 1
> %endrep
 
 But there is another difference with **%assign**.  It is the only preprocessor directive that interacts with data structures in the assembler; so for example the **%assign** expression can contain expressions involving labels and the program counter.  Thus:
 
> helloWorld db "hello world"
> %assign COUNTER 64 - ($-helloWorld)
> %assign PADDING ($-helloWorld)
> %rep COUNTER
>     db PADDING
>     %assign PADDING PADDING + 1
> %endrep
 
 puts the string "hello world" in the text, followed by the byte for 11, the byte for 12, etc... up to the byte for 63.
 
 Note that this latter behavior of interacting with the assembler is only valid if code is being assembled.  If the preprocess-only switch is specified on the command line, assembler symbols will not be available, and the latter example will result in errors.


#### %iassign

 **%iassign** is a form of **%assign** where the identifier is considered to be case insensitive.  So for example:
 
> %iassign COUNTER 63
> %rep Counter
>     ...
> %endrep
 
 and similar case variants on the word counter would still result in a loop that executes 63 times.


#### %idefine

 **%idefine** is a form of **%define** where the identifier is assumed to be case insensitive.  So for example:
 
> %idefine COUNTER 4
> %idefine counter 4
> %idefine Counter 4
 
 are equivalent statements, and any case variant of the word COUNTER will match for substitution.  Note that this case sensitivity only extends to the identifier; any parameters specified in a function-style **%idefine** are still case-sensitive for purposes of substitution.


#### %elifdef

   **%elifdef** is a _%elif-style conditional_ that takes an identifer as an argument.  If the identifier has been defined with a previous **%define** or **%assign** statement, the next block will be assembled, otherwise the next block will be ignored.
 
 For example:
 
> %define COLOR 3
> %if COLOR == 2
>     mov eax,4
 %elifdef COLOR
     inc eax
> %endif
 
 will result in the mov statement being ignored and the inc statement being assembled because COLOR has been defined but is not 2.
 
 Note that a definition declared with **%define** or **%assign** must match the argument exactly, whereas a definition declared with **%idefine** or **%iassign** can differ in case and still match.  **%elifdef **will not match identifiers declared with **%macro** or **%imacro**.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %elifndef

 
   **%elifndef** is a _%elif-style conditional_ that takes an identifer as an argument.  If the identifier has not been defined with a previous **%define** or **%assign** statement, the next block will be assembled, otherwise the next block will be ignored.
 
 For example:
 
> %define COLOR 3
> %if COLOR == 2
>     mov eax,4
 %elifndef COLOR
     inc eax
>  %endif
 
 will result in nothing being assembled because COLOR is defined but not equal to 2.
 
 Note that a definition declared with **%define** or **%assign** can have any difference from the argument and trigger assembly of the block, whereas a definition declared with **%idefine** or **%iassign** must differ in some way other than in case.  **%elifndef** will assemble the following block for identifiers declared with **%macro** or **%imacro**.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.
     
 