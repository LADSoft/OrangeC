### C-Style Preprocessor Directives

 
 The C-Style preprocessor directives are compatible with similar directives that are existant in preprocessors for C compilers.  OAsm does not change the behavior of these directives other than to change the initial character which introduces the directive from '\#' to '%'.


#### %define

 **%define** introduces a method to perform textual substitutions.  In its simplest form it will just replace an identifier with some text:
 
>     %define HELLO\_WORLD "Hello World"
 
 replaces all instances of the identifier HELLO\_WORLD with the indicated string.  For example after this definition the following statement:
 
>     db HELLO\_WORLD
 
 will result in the string "Hello World" being compiled into the program.
 
 **%define** is not limited to giving names to strings, it will do any sort of text substitution you want.  That could include defining names for constants, or giving a name to an often-used instruction, for example.
 
 In the below:
 
>     %define ARRAY\_MAX 4
>     mov eax,ARRAY\_MAX
 
 the text "4" gets substituted for ARRAY\_MAX prior to assembling the mov instruction, so what the assembler sees is:
 
>     mov eax,4
 
 Note that definitions are also processed for substitution:
 
>     %define ONE 1
>     %define TWO (ONE + 1)
>     %define THREE (TWO + 1)
>     mov eax,THREE
 
 is substituted multiple times during processing, with the final result being:
 
>     mov eax,((1 + 1) + 1)
 
 OAsm will detect recursive substitutions and halt the substitution process, so things like:
 
>     %define ONE TWO
>     %define TWO ONE
 
 will halt after detecting the recursion.
 
 Also, the substitution text can be empty:
 
>     %define EMPTY
>     mov eax, EMPTY
 
 results in the translated text:
 
>     mov eax,
 
 which cannot be assembled and will result in a syntax error during assembly.
 
 
 **%define** can also be used in its functional form for more advanced text replacement activities.  In this form, the identifier is parameterized.  During substitutions, arguments are also specified; and the original parameters are replaced with the arguments while substitution is occurring.  For example:
 
>     %define mul(a,b)  a \* b
>     mov eax,mul(4,7)
 
 is changed to:
 
>     mov eax,4 \* 7
 
 prior to assembly.  It is usually not a good idea to write this quite the way it was done however.  The user may elect to put any text they want in the invocation, so one thing that can happen is they write:
 
>     mov eax, mul(4+3, 7+2)
 
 This gets translated to:
 
>     mov eax, 4 + 3 \* 7 + 2
 
 which was probably not the intent.  Below is what was probably desired:
 
>     mov eax, (4+3) \* (7+2)
 
 For this reason it is a good idea to fully parenthesize the parameters used in the original definition:
 
>     %define mul(a,b) ((a) \* (b))
 
 so that the mov with mul gets translated to:
 
 mov eax, ((4 + 3) \* (7 + 2))
 
 The extra set of parenthesis is used to prevent similar situations from happening when 'mul' is used as a subexpression of another expression.
 
 
 Note that when using **%define**, substituted text is not evaluated in any way, other than to process substitutions on the identifier and any specified parameters.    So the move statement in the last example would be visible to the assembler exactly as the substitutions dictate, and the assembler has to do further evaluation of the expression if it wants a constant value.
 
 Within a definition, there are a couple of special-case substitutions that can occur with function-style definitions.  In Stringizing, a parameter can be turned into a string.  For example if you write:
 
>     %define STRINGIZE(str) \#str
>     db   STRINGIZE(Hello World)
 
 quotes will be placed around the substituted parameter.  So this translates to:
 
>     db   "Hello World"
 
 prior to assembly.
 
 In Tokenizing, new identifiers may be produced.  For example:
 
>     %define Tokenizing(prefix, postfix) (prefix \#\# postfix + 4)
>     mov eax,Tokenizing(Hello,World)
 
 would be translated to:
 
>     mov eax,HelloWorld + 4
 
 prior to assembly.
 
 Note that even though the hash character used to start a preprocessor statement has been changed to '%', hash is still used in stringizing and tokenizing.
 
 
 Finally, **OAsm** supports the C99 extension to function-style definitions, which allows variable-length argument lists.  For example:
 
>     %define mylist(first, ...)  dw first, \_\_VA\_ARGS\_\_
 
 where \_\_VA\_ARGS\_\_ means append all remaining arguments that are specified, could be used like this:
 
>     mylist(1)
>     mylist(1,2)
>     mylist(1,2,3,4,5)
 
 and so on.  These would expand to:
 
 dw    1
 dw    1,2
 dw    1,2,3,4,5
 
 Note that the name of the identifier that is replaced is case-sensitive with **%define**, for example HELLO\_WORLD is not the same as Hello\_World.  There is a case-insensitive form of this directive **%idefine** which can be used to make these and other related identifiers the same.
 
 Note: OAsm does not support overloading function-style macros.  
 
 For convenience OAsm allows **%define** definitions on the command line, which are useful for tailoring build behavior either directly or through the conditional processing facility.


#### %undef

>     **%undef** undoes a previous definition, so that it will not be considered for further substitutions (unless defined again).  For example:
 
>     %define REG\_EBX 3
>     %undef REG\_EBX
>     mov eax, REG\_EBX
 
 results in no substitution occurring for the use of REG\_EBX.


#### %error

 **%error** displays an error, causing the assembler to not generate code.  For example:
 
>     %error my new error
 
 might display something like:
 
>     Error errdemo.asm(1): Error Directive: my new error
 
 When the file is assembled.


#### %line

 **%line** is used to change the file and line number listed in the error reporting.  By default the error reporting indicates the file and line an error occur on.  Sometimes in generated source code files, it is useful to refer to the line number in the original source code rather than in the file that is currently being assembled.  **%line** accomplishes this by updating internal tables to indicate to the preprocessor that it should use alternate information when reporting an error.  For example inserting the following at line 443 of test.asm:
 
>         mov eax,^4
 
 produces a syntax error when the code is assembled:
 
>     Error test.asm(443): Syntax Error.
 
 If an additional %line directive is inserted:
 
>         %line 10, "demo.c"
>         mov eax,^4
 
 the error changes to:
 
>     Error demo.c(10): Syntax Error
 
 Note that once **%line** is used to change the line number and file name, **OAsm** remembers the new information and continues to increment the new line number each time it processes a line of source code.


#### %include

 **%include** is used to start the interpretation of another source file.  The current source file is suspended, and the new source file is loaded and assembled.  Once the assembler is done with the new source file (and anything it also **%includes**) assembly resumes beginning where it left off in the current source file.
 
 This facility is useful for example to hold preprocessor constants and structures that are shared between multiple source files.  But the included file can include any valid assembler statement, including GLOBAL and EXTERN definitions.  
 
 For example if **test.asm** is being assembled and the statement:
 
>     %include "test1.asm"
 
 is encountered, the assembly of **test.asm** will temporarily be suspended while **OAsm** goes off to assemble **test1.asm**.  After it is done with **test1.asm,** **OAsm** remembers that it was previously assembling** test.asm** and picks up in that file where it left off (e.g. at the line after the **%include** statement).
 
 This is not quite the same as specifying both **test.asm** and **test1.asm** on the command line.  In the current example there is only one output file which contains the contents of both **test.asm** and **test1.asm**, where as if both were specified on the command line they would result in separate output files.
 
 For convenience, an include path may be specified on the command line, and **OAsm** will search for included files both in the current directory, and in directories specified on that path.


#### %if

 **%if** is a _%if-style conditional _that takes a numeric expression as an argument.  If the numeric expression evaluates to a non-zero value, the result of the evaluation will be true, otherwise it will be false.   Note that for purposes of this conditional, expressions are always evaluated; if an undefined identifier is used in a **%if** expression it is replaced with '0' and evaluation continues.   Subsequent blocks of code will either be assembled if the result of the evaluation is non-zero, or ignored if the result of the evaluation is zero.
 
 For example:
 
>     %define COLOR 3
>     %if  COLOR == 3
>         mov eax,4
>     %endif
 
 will result in the mov statement being assembled because the result of the argument evaluation is a nonzero value.
 
 On the other hand:
 
>>     %if ZERO
>         mov eax,4
>     %endif
 
 results in nothing being assembled because the value of 'ZERO' is zero.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %elif

  **%elif **is a _%elif-style conditional_ that takes a numeric expression as an argument.  If the numeric expression evaluates to a non-zero value, the next block will be assembled, otherwise it will be ignored.  As with _%if_, undefined symbols will be replaced with '0' for purposes of the evaulation.
 
 For example:
 
>     %define COLOR 3
>     %if  COLOR == 2
>         mov eax,4
>     %elif COLOR == 3
>         inc eax
>     %endif
 
 will result in the mov statement being ignored and the inc statement being assembled because the result of the **%if** argument evaluation is zero, and the result of the **%elif **argument evaluation is nonzero.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on %elif-style conditionals.


#### %ifdef

  **%ifdef** is a _%if-style_ conditional that takes an identifer as an argument.  If the identifier has been defined with a previous **%define** or **%assign** statement, the next block will be assembled, otherwise it will be ignored.
 
 For example:
 
>     %define COLOR 3
>     %ifdef COLOR
>         mov eax,4
>     %endif
 
 will result in the mov statement being assembled because COLOR has been defined.
 
 Note that a definition declared with **%define** or **%assign** must match the argument exactly, whereas a definition declared with **%idefine** or **%iassign** can differ in case and still match.  **%ifdef** will not match identifiers declared with **%macro** or **%imacro**.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %ifndef

   **%ifndef** is a _%if-style conditional_ that takes an identifer as an argument.  If the identifier has not been defined with a previous **%define** or **%assign** statement, the next block will be assembled, otherwise it will be ignored.
 
 For example:
 
>     %define COLOR 3
>     %ifndef COLOR
>         mov eax,4
>     %endif
 
 will result in the mov statement being ignored because COLOR has been defined.  Alternatively:
 
>     %undef COLOR
>     %ifndef COLOR
>         mov eax,4
>     %endif
 
 will result in the mov statement being assembled because COLOR is not currently defined.
 
 Note that a definition declared with **%define** or **%assign** can have any difference from the argument and trigger assembly of the block, whereas a definition declared with **%idefine** or **%iassign** must differ in some way other than in case.  **%ifndef** will assemble the following block for identifiers declared with **%macro** or **%imacro**.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %else

 **%else** is used to select a block for assembly, when all previous _%if-style conditionals_ and _%elif-style conditionals_ in the same sequence have had their arguments evaluate to false.  For example:
 
>     %define COLOR = 3
>     %if COLOR ==4
>         mov eax,3
>     %else
>         inc eax
>     %endif
 
 will result in the mov being ignored, but the inc being assembled, because the evaluation of the **%if** argument is false.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%else_.


#### %endif

  
 **%endif**  is used to end a  conditional sequence. Once a conditional sequence is ended, code assembly proceeds as normal, unless the conditional sequence was itself nested within a block of a higher-level conditional sequence that is not being assembled.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%endif_.
  