## OAsm Command Line

 The general format of an **OAsm** command line is:
 
>     OAsm \[options\] file-list
 
 where the _file-list_ is an arbitrary list of input files.
 
 For example:
 
>     OAsm one.asm two.asm three.asm
 
 assembles several files, and makes output files called one.o, two.o and three.o.
 
 The file list can have wildcards:
 
>     OAsm \*.asm
 
 assembles all the files in the curent directory.


### Response Files

 Response files can be used as an alternate to specifying input on the command line.  For example:
 
>     OAsm @myresp.lst
 
 will take command line options from **myresp.lst**>       Response files aren't particularly useful with the assembler, but they are supported.


### Case Insensitivity

 if the **-i **switch is specified on the command line, [labels](OAsm%20Labels.md) will be treated as case insensitive.  This can allow easier linkage between modules which use different case.  Note that case insensitivity only extends to labels; preprocessor symbols are always case-sensitive in OAsm.


### Using the assembler as a preprocessor


### 
 

 If the **-e** switch is specified on the assembler command line, **OAsm **will act as a preprocessor and not perform the actual assembly phase.  This operation is equivalent to using **OCPP** in assembly-language mode.  In preprocessor mode, the full functionality of the preprocessor is available, when it does not rely on information that would only be available while assemblying the file.  Specifically, the preprocessor is fully functional, except that expressions that refer to labels or program counter locations will result in an error when used for example with the **%assign** preprocessor statement.


### Specifying the output file name
 

 
 By default, **OAsm** will take the input file name, and replace the extension with the extension **.o**.
 
 However in some cases it is useful to be able to specify the output file name.  The specified name can have its extension specified, or it can be typed without an extension to allow **OAsm** to add the **.o** extension.  **OAsm** is only capable of producing object files.
>     
>     OAsm /omyfile test.asm
 
 makes an object file called **myfile.o**.>


### Specifying include file path

 By default, **OAsm** will search for include paths in the current directory.  If there are other paths **OAsm** should search, the **/I** switch can be specified to have it search them.


### Defining variables

 If it is necessary to define a variable on the command line, use the switch **/D**.
 
 For example:
 
>     OAsm /DMYINT=4 test.c
 
 defines the variable MYINT and gives it a value of 4. 
 
 A variable doesn't have to be defined with a value:
 
>     OAsm /DMSDOS test.c
 
 might be used to specify preprocessing based on the program looking for the word MSDOS in \#ifdef statements.


### Listing File

 When the command line switch **-l** is specified, **OAsm** will product a listing file correlating the output byte stream with input lines of the file.  Note the listing file is not available in preprocessor mode as no assembly is done.  
 
 By default the listing file will not show macro expansions.  To get a listing file where macros are shown in expanded form, use **-ml**.  This will also expand preprocessor repeat statements.  Note that there is a special qualifier used in a macro definition, **.nolist**, which can be used on a macro-by-macro basis.  When used, it prevents macros from being expanded in the listing file even when **-ml** is used.  This is useful for example to prevent cluttering the the listing file with expansions of often-used macros or macros that are composed largely of preprocessor statements.  In fact the builtin macros that map preprocessor directives to native form all use this qualifier.
 
 
 