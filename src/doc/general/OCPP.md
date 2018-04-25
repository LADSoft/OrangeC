# OCPP

 
 **OCPP** is an extended version of the traditional C language preprocessor.  The extensions include support for C99, and support for various extended constructs present in the assembler.  It is beyond the scope of this document to discuss the format of input files used with the preprocessor.  See a discussion of the C language for further details of use with the C language, and the assembler documentation for discussion of extensions used with the assembler.
 
 Note that **OCPP** is not quite the same as the preprocessor built into a C compiler.  The C compiler is able to maintain a slightly more detailed context about the preprocessed text.  In rare cases loss of this information will cause a file preprocessed with OCPP to not be compilable with any C compiler.


## Output file

 **OCPP** has no mechanism for specifying the output file name; instead it takes the input file, strips the extension, and writes a file with a '.i' extension to indicate preprocessor output.


## Command Line Options

 The general form of an OCPP command line is:
 
>     OCPP [options] file
 
 Here the_ file_ is the file to preprocess.  (multiple files may be specified on the command line if you choose).
 
 There are several command line options that control how the preprocessing is done.  These include the ability to enable extensions, the ability to set a path for include files, and options to define and undefine preprocessor variables.


### Specifying use of extensions

 By default, **OCPP** will perform as a C89 version preprocessor, which is slightly looser than the standard.  It can be tightened to meet the standard with the /**A** switch.
 
 However, it also has full support for the C99 concept of variable length argument lists in macros.  This can be enabled with either the **/9** switch (For C99 support only) or with the **/a **switch (For assembly language support).  Additionally, the **/a** switch will enable various nasm-like preprocessor extensions such as a whole family of additional \#IF conditional statements and additional types of macros that are more consistent with the needs of assembly language.
 
 Note that when **/a** is specified, the character identifying preprocessor directions changes from \# to %.  For example when assembly language extensions are defined,
 
>     %define MYINT 4
 
 is used instead of the normal
 
>     #define MYINT 4
 
 used by C compilers.


### Specifying include file path

 By default, **OCPP** will use the C language system include path to search for include files specified in the source file.  If there are other include paths **OCPP** should search, the **/I** switch can be specified to have it search them.  For example by default the statement:
 
>     #include <windows.h>
 
>     (or %include <windows.h>)
 
 will search in the C language system include directory to find windows.h.  Whereas:
 
>     OCPP /I.\\include test.c
 
 will create a file test.i, which will additionally search the path **.\\include** for any include files specified in preprocessor directives.


### Defining variables

 **OCPP** has two switches useful for defining variables.  The first switch **/D** defines a variable.  The second switch **/U** causes OCPP to never allow the specified variable to be defined.
 
 For example
 
>     OCPP /DMYINT=4 test.c
 
 defines the variable MYINT and gives it a value of 4.  Whereas
 
>     OCPP /UMYINT test.c
 
 globally undefines MYINT in such a way that it cannot be defined while preprocessing the file.
 
 A variable doesn't have to be defined with a value:
 
>     OCPP /DWIN32 test.c
 
 might be used to specify preprocessing based on the program looking for the word WIN32 in **\#ifdef** statements.


### Setting number of errors
 

 By default **OCPP** will display up to 25 errors before it exits with a fatal error.  This can be changed with the **/E** switch.  There are two functions of the **/E** switch.  The first function is to enable some trivial warnings that are normally suppressed, such as the warning about the file not ending in a CRLF.  The second is to change the number of errors that OCPP will display before exiting.
 
 To enable trivial errors, use the **/E+** form of the switch:
 
>     OCPP /E+ test.c
 
 To enable more (or less) errors, use the **/Enn** form of the switch:
 
>     OCPP /E255 test.c
 
 stops after 255 errors.
 
 The two forms may be comined:
 
>     OCPP /E+3 test.c
 
 enables trivial errors but stops after three errors.
 
 
 
