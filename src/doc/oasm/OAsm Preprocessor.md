## OAsm Preprocessor
 

 The OAsm preprocessor is a C preprocessor, with extensions.  The preprocessor can be considered to be a set of routines which go through the code before it is assembled, making certain types of textual transformations to the source code.  The transformations range from simple substitution of text or lines of text when a keyword is encountered, to inclusion of text from another file, to selectively ignoring some lines based on compile-time settings.  The main difference from a C preprocessor, other than the extensions, is that instead of starting a preprocessor directive with a hash ('\#') preprocessor directives are started with a percent ('%').
 
 Many of these directives involve [Conditional Processing](Conditional%20Processing.md), which is a way to selectively choose what lines of code to assemble and what lines to ignore based on previous declarations.
 
 Table 1 shows the [C-style preprocessor directives](C-Style%20Preprocessor%20Directives.md).  These directives are compatible with similar directives in a C Compiler's preprocessor.
 
     

|colheader |colheader |
|--- |--- |
|%define|define a constant or function-style macro|
|%undef|undefine a macro|
|%error|display an error|
|%line|set the file and line displayed in error messages|
|%include|include another file|
|%if|conditional which tests for nonzero expression|
|%elif|else-style conditional which tests for nonzero-expression|
|%ifdef|conditional which tests to see if a macro has been %defined|
|%ifndef|conditional which tests to see if a macro has not been %defined|
|%else|else clause for conditionals|
|%endif|end of conditional|   

  Table 1 - C lanuage style preprocessor directives
 
 Table 2 shows [basic extensions to the C preprocessor](Basic%20Extensions%20to%20C%20Preprocessor.md) that are similar to directives already found in the preprocessor.  This includes a new directive **%assign** which is like **%define** except it evaluates the macro value on the assumption it is a numeric expression.  It also includes case insensitive macro definition directives, and the beginning of an extensive set of extensions to condtionals that are similar to the %elif mechanism.
 
      

|colheader |colheader |
|--- |--- |
|%assign|Like %define, but evaluates an expression and sets the value to the result|
|%iassign|%assign with a case-insensitive name|
|%idefine|%define with a case-insensitive name|
|%elifdef|else-style conditional which tests to see if a macro has been %defined|
|%elifndef|else-style conditional which tests to see if a macro has not been %defined|   

  Table 2 - Basic extensions to C style preprocessor
   
 Table 3 shows extensions to the conditional mechanism that allow [text comparisons](Text%20Comparison%20Conditionals.md).  There are both case sensitive and case insensitive forms of these directives.
 
      

|colheader |colheader |
|--- |--- |
|%ifidn|Case sensitive test for string matching|
|%ifnidn|Case sensitive test for string not matching|
|%elifidn|else-style case sensitive test for string matching|
|%elifnidn|else-style case sensitive test for string not matching|
|%ifidni|Case insensitive test for string matching|
|%ifnidni|Case insensitive test for string not matching|
|%elifndi|else-style case insensitive test for string matching|
|%elifnidni|else-style case insensitive test for string not matching|   

  Table 3 - Text Comparison Conditionals
 
 Table 4 shows various extensions to the conditional mechanism that allow [classification of a token's type](Token%20Type%20Classification%20Conditionals.md).  They can be used for example in [multiline macros](Multiline%20Macro%20Extensions.md), to allow a single macro to have different behaviors based on the type of a macro argument.
 
      

|colheader |colheader |
|--- |--- |
|%ifid|test to see if argument is an identifier|
|%ifnid|test to see if argument is not an identifier|
|%elifid|else-style test to see if argument is an identifier|
|%elifnid|else-style test to see if argument is not an identifier|
|%ifnum|test to see if argument is a number|
|%ifnnum|test to see if argument is not a number|
|%elifnum|else-style test to see if argument is a number|
|%elifnnum|else-style test to see if argument is not a number|
|%ifstr|test to see if argument is a string|
|%ifnstr|test to see if argument is not a string|
|%elifstr|else-style test to see if argument is a string|
|%elifnstr|else-style test to see if argument is not a string|   

  Table 4 - Token Type Classification Conditionals
 
 Table 5 shows the [Multiline Macro Extensions](Multiline%20Macro%20Extensions.md). and the [Repeat Block Extensions](Repeat%20Block%20Extensions.md).  These extentions include multiline macros, as well as a powerful facility for using the preprocessor to repeat sections of code or data.
 
      

|colheader |colheader |
|--- |--- |
|%macro|start a multiline macro|
|%imacro|start a multiline macro, case insensitive name|
|%endmacro|end a multiline macro|
|%rotate|rotate arguments in a multiline macro|
|%rep|start a repeat block|
|%endrep|end a repeat block|
|%exitrep|exit a repeat block prematurely|   

  Table 5 - Multiline Macro and Repeat Block Extensions
  
 Table 6 shows the [Context-Related Extensions](Context-Related%20Extensions.md).  Preprocessor contexts are a powerful mechanism that can be used to 'remember' data between successive macro invocations, and for example could be used to construct a high-level representation of control constructs in the assembler.
 
      

|colheader |colheader |
|--- |--- |
|%push|start a new context|
|%pop|end a new context|
|%repl|rename the context at the tope of the context stack|
|%ifctx|test to see if a context is in effect|
|%ifnctx|test to see if a context is not in effect|
|%elifctx|else-style test to see if a context is in effect|
|%elifnctx|else-style test to see if a context is not in effect|   

  Table 6 - Context - Related Extensions
  
 