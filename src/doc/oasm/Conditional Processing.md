### Conditional Processing

 
 Conditional processing is a way to tell the assembler that some lines of code should be assembled into the program, and other lines may be ignored.  There are a variety of conditional processing directives, which use conditions ranging from evaluation of an expression, to string comparison, to type or state of a previous symbol definition.  It is useful particularly in configuration management, to allow different configurations of the program to be built for example by changing the command line.  It is also useful in conjunction with [multiline macros](Multiline%20Macro%20Extensions.md), where it can be used to evaluate some characteristic of an argument to a macro.
 
 The conditional processing statements can be broken into four basic types:
 
* %if-style conditional
* %elif-style conditional
* %else
* %endif
  Conditional processing always starts with a _%if-style conditional_, and ends with a _%endif conditional_.  Between them them, there can be any number of _%elif-style conditionals_ (including none at all), followed by zero or one _%else conditional_.  A sequence of these conditionals breaks the enclosed code up into multiple blocks.
 
 Conditionals may be nested; in other words each block can be further broken up into smaller blocks with more conditionals that are placed inside the initial conditional.  Two conditional statements are considered to be at the same level if all sets of conditionals within the blocks specified by the conditionals have both a _%if-style conditional_ and a _%endif conditional_.
 
 Processing starts when the _%if-style conditional_.  Its arguments are evaluated according to the rules for that conditional.  If the evaluation returns a value of true, the following block of code is assembled, and any other blocks up to the _%endif conditional_ are ignored
 
 If however the evaluation of the _%if-style conditional _returns false, the following block of code is ignored.  Then processing begins with any _%elif-style conditionals_ at the same level, in the order they appear in the code.  Each _%elif-style conditional_ is successively evaluated, until the evaluation of one of the arguments returns 'true'.  For each _%elif-style conditional _that evaluates to false, the corresponding block is skipped; if one returns true its code block is assembled and all remaining blocks of code up to the _%endif conditional_ are ignored.
 
 If the evaluation of the _%if-style conditional_ returns false, and a corresponding _%elif-style conditional_ that returns true cannot be found at the same level (e.g. they all return false or there aren't any), the _%else conditional_ block is assembled.
 
 If all the evaluations return false, and there is no _%else conditional_, none of the blocks are assembled.
 
 Various examples follow, for the _%if-style conditional_ that evaluates expressions:
 
>     ace;">%if COLOR == 4
>     ace;">    mov eax,4
>     ace;">%endif
 
>     ace;">%if COLOR == 4
>     ace;">    mov eax,4
>     ace;">%else
>     ace;">    mov eax,1
>     ace;">%endif
 
>     ace;">%if COLOR == 4
>     ace;">    mov eax,4
>     ace;">%elif COLOR == 3
>     ace;">    mov eax,3
>     ace;">%elif COLOR == 2
>     ace;">    mov eax,2
>     ace;">%endif
 
>     ace;">%if COLOR == 4
>     ace;">    mov eax,4
>     ace;">%elif COLOR == 3
>     ace;">    mov eax,3
>     ace;">%elif COLOR == 2
>     ace;">    mov eax,2
>     ace;">%else
>     ace;">    mov eax,1
>     ace;">%endif
 
 
 Note that when a conditional block is not being assembled, no preprocessor directives within that block will be evaluated either (other than to allow **OAsm's** preprocessor to reevaluate the state of conditional processing).
 
 