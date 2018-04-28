# OImpLib

 
 **OImpLib** is a WIN32 import librarian, suitable for various operations regarding the import sections of DLLs.  It can take input from one of several sources, and place output in one of several destinations.  In its most basic format one could use it to take a .DEF file or .DLL file and construct an import library for use with the toolchain, but it can also be used to create a .DEF file or extract things from a library.


## Command Line Options

 
 The general format of an **OImpLib** command line is:
 
>     OImpLib [options] source dest
 
 where source and dest specify files to use, and further, by parsing the extensions of source and dest **OImpLib** is able to act in one of several modes


### Response Files

 Response files can be used as an alternate to specifying input on the command line.  For example:
 
>     OImpLib test.l @myresp.lst
 
 will take command line options from **myresp.lst**.  In general it isn't necessary to use response files with **OImpLib** as the amount of input required is minimal.


### Case Insensitivity
 

 
 **OImpLib** will allow the creation of case insensitive libraries with the **/c-** switch, however, in general it isn't a good idea to make a case-insensitive import library, as WIN32 export records  found in DLLs are case-sensitive.


###


### Operation modes
 

 **OImpLib** will perform different operations depending on what the file extensions of the input files are.  The output file is specified first, followed by one or more input files.  The output file may be one of the following:
 
* a library file
* an object file
* a .DEF file
  When the output file is a library file, the input file can be a list of object files, .DEF files, and .DLL files.  The object files will be placed in the library, whereas the export sections of .DEF and .DLL files will be converted to object files that hold import records, and then placed in the library.
 
 When the output file is an object file, a single input file can be either a .DEF or .DLL file.  The exports from the input file will be placed in the output file.
 
 When the output file is a .DEF file, the input file can be either a .DLL file or an object file.  The exports in the .DLL file will be written to the .DEF file, or the import records in the object file will be converted to export records and written to the .DEF file.  For example:
 
>     OImpLib test.l kernel32.dll
 
 will make an import library holding the export definitions from **kernel32.dll** and
 
>     OImpLib test.ld mydll.def
 
 will make an import library containing the export definitions from **mydll.def**.
 
 On the other hand:
 
>     OImpLib user32.def user32.dll
 
 will create a definition file from the export records in **user32.dll**.
 
 
### Alternative display options

 The **/V** switch shows version information, and the compile date

 The **/!** switch is 'nologo'

 
 
