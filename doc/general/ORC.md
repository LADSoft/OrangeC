# ORC

 
 **ORC** is a windows resource compiler.  It handles compilation of a file containing standard windows resources into a .RES file.  The .RES file can be given to the linker or to DLPE for use in adding resources to a windows executable file.  The specification for the format of the input file is mostly beyond the scope of this document other than to say that the **ORC** program has a C language preprocessor.   This makes it possible to define symbolic constants.


## Command Line Options

 The general format of an **ORC** command line is:
 
>     ORC [options] rcfile.rc
 
 where rcfile.rc is the resource file to compile.


### Response Files

 Response files can be used as an alternate to specifying input on the command line.  For example:
 
>     ORC @myresp.lst
 
 will take command line options from **myresp.lst**.  In general it isn't necessary to use response files with **ORC** as the amount of input required is minimal.


### Setting include file path
 

 By default, **ORC** will use the C language header include path as an include path to search for files specified in preprocessor INCLUDE directives.  For example the statement:
 
>     #include <windows.h>
 
 will result in windows.h being found in the compiler include directory, and included in the file.  If there are other paths that should be searched, they may be specified on the command line with the /i switch.  For example:
 
>     ORC /i.\include test.rc
 
 Searches in the directory **.\\include** as well as in the C language include directory.
 
 The **ORC** preprocessor defines the preprocessor symbol>     RC\_INVOKED to allow include files to specify sections that won't be evaluated by **ORC**.  For example the windows headers use this to prevent RC compilers from trying to parse structure definitions written in the C language.  This way instructions to the RC compiler can be intermixed with instructions to the C compiler without causing **ORC** to process things it isn't capable of processing.


### Defining variables

 
 **ORC** has a **/D** switch to define preprocessor variables.
 
 For example
 
>     ORC /DMYINT=4 test.c
 
 defines the variable MYINT and gives it a value of 4.
>     
 A variable doesn't have to be defined with a value:
 
>     ORC /DWIN32 test.c
 
 might be used to specify preprocessing based on the program looking for the word WIN32 in \#ifdef statements.
 

### Alternative display options

 The **/V** switch shows version information, and the compile date

 The **/!** or **--nologo** switch is 'nologo'