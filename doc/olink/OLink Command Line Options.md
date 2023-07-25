## OLink Command Line Options

 The general format of an **OLink** command line is:
 
>     OLink [options] file-list
 
 where _file-list_ is an arbitrary list of input files.
 
 For example:
 
>     OLink one.o two.o three.o
 
 links several object files and makes an output file called **one.rel**.  By default **OLink** takes the name of the first file on the command line, and replaces it with the extension **.rel**, to decide on an output file name.
 
 The file list can have wildcards:
 
>     OLink /otest.rel \*.o
 
  links all the files in the curent directory, and the results are put in **test.rel** because the **/o** command line switch is specified.


### Response Files

 Response files can be used as an alternate to specifying input on the command line.  For example:
 
>     OLink @myresp.lst
 
 will take command line options from **myresp.lst**.  This is useful for example when there are many **.o** or **.l** files being merged and it is not convenient to specify them on the command line.


### Case Sensitivity

 By default **OLink** treats symbols as case-insensitive.  If the **-c+** switch is specified on the command line, labels will be treated as case sensitive.  This supports certain language compilers that do allow the user to type the same word in different case, and have it mean different things.


### Specifying the output file name
 

 
 By default, **OLink** will take the first input file name, and replace the extension with the extension **.rel **to create the name of the output file.
 
 However in some cases it is useful to be able to specify the output file name.  The specified name can have its extension specified, or it can be typed without an extension to allow **OLink** to add the **.rel** extension.
>     
>     OLink /r+ /omyfile test1.o test2.o test3.o cl.l
 
 makes an object file called **myfile.rel**>     .


### Specifying Lib File Path
 

 By default, **OLink** will search for library files in the C Compiler library file path, and in the current directory.  The **/L** option may be used to specify additional directories to search for lib files:
 
>     OLink /L..\mylibs test.o floating.l
 
 will find **floating.l **either in the C compiler library directory, the current directory, or the directory ..\\mylibs.


### Defining variables

 **OLink** allows definition of global variables on the command line, and gives them an absolute address.  This facility can be used either to set the location of a function or data, or to provide a constant value to a [specification file](OLink%20Specification%20Files.md) entry or user code.  If it is necessary to define a variable on the command line, use the switch **/D**.  
 
 For example
 
>     OLink /DRAM=0x80000000 myprog.o
 
 Defines a global variable RAM which has the address 80000000 hex.  This variable may be accessed externally from either the program code, or the specification file.  It might be used for example to relocate code or data, to specify the address of some hardware, and so forth.
 
 Decimal values for addresses may be provided by omitting the **0x** prefix.


### Complete Link

 By default, **OLink** performs a partial link.  ****In a partial link, some global definitions may remain unresolved, and libraries are ignored.  The output file may be further linked with more object files or with the result of other partial links.  However, a partial link cannot be used to generate a rom image or executable file, because not all the information required for such binaries has been generated; specifically there may be some addresses that haven't been defined yet.
 
 Usually, a complete link happens automatically when the **/T** switch is used to specify a [target configuration](OLink%20Target%20Configurations.md).  However in some cases it is desirable to indicate to the linker that a complete link is desired without specifying a target configuration.  Use **/r+** for this purpose.

### DLL control

 The switch --output-def is used to ask the downloader (dlpe) to generate a .DEF file with dll exports.   This switch only has meaning
when creating dlls. 

>     OLink /T:DLL32 /c --output-def test.def /o test.exe c0dpe.o test.o clwin.l climp.l

generates a DLL from test.o, and outputs a file test.def with the exports

### Delay loading of DLLs

There are three switches to support WIN32 delay loading of DLLs.

>     OLink /T:DLL32 /c /DELAYLOAD:USER32.DLL  --output-def test.def /o test.exe c0dpe.o test.o clwin.l climp.l

adds USER32.DLL to the delay load table, and creates a binding table for the DLL.

To have it not create a binding table use /DELAY:NOBIND:

>     OLink /T:DLL32 /c /DELAYLOAD:USER32.DLL /DELAY:NOBIND  --output-def test.def /o test.exe c0dpe.o test.o clwin.l climp.l

To add a table for manually unloading the dll use /DELAY:UNLOAD

>     OLink /T:DLL32 /c /DELAYLOAD:USER32.DLL /DELAY:UNLOAD  --output-def test.def /o test.exe c0dpe.o test.o clwin.l climp.l

### Map File

 To generate a map file, use the **/m **switch.  The map file name will be the same as the name of the **.rel** file, with the extension replaced by **.map**.  The standard map file summarizes the partitions the program is contained in, and then lists publics in both alphabetic and numeric order.  
 
 A more detailed map file may be obtained by using **/mx**.  This gives a list of partitions, overlays, regions, and files that went into making up the program, in the order they were included.  It also includes details of the attributes used to place the sections.


### Specification file

 Sometimes a default [specification file](OLink%20Specification%20Files.md) will be selected automatically with the **/T** [target configuration](OLink%20Target%20Configurations.md) switch.  This specification file may be overridden with the **/s** switch; or if the **/T** switch is not specified the **/s** switch may be used to select a specification file.  For example:
 
>     OLink /smyprog.spc myprog.o


###>


### Target Configuration

 The [target configuration](OLink%20Target%20Configurations.md) switch specifies that a specific profile be used to build the target.  The profile includes a [specification file](OLink%20Specification%20Files.md), default definitions, a post-processing program to run, and some other information.  The link will be done with the given parameters, and then the post-processing program will be run to generate the final ROM image or executable file.
 
 For example:
 
>     OLink /T:M3 myprog.o
 
 selects the specification profile that builds a Motorola Hex file with four-byte addresses.


### Link Only

 Sometimes when a [target configuration](OLink%20Target%20Configurations.md) has been specified, it is desirable to stop after generating the **.rel** file instead of going on to build the ROM Image or Executable.  This may be done by specifying the **/l** switch on the command line.
 
   
### Alternative display options

 The **/V** switch shows version information, and the compile date

 The **/!** or **--nologo** switch is 'nologo'
