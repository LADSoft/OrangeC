# onm

 **onm** is an object dump utility, similar to GNU nm.exe.   It takes the symbolic information from an object file and presents it on the console;

## Command Line Options

 The general format of an **onm** command line is:
 
>     onm [options] [list of files]
 
where options are various command line options that can be used to customize the behavior, and the list of files lists files that should be processed.   Files can either be object files (.o) or library files (.l)

### Display grouping options

By default, **onm** shows non-external symbols.  The '/a' option adds the externals to this list, the '/g' option shows only externals, and the '/u' option shows externals that have not been resolved by other input files.

### Sorting options

By default **onm** sorts first by file, then by symbol name.   To avoid sorting use '/u'.  To sort by address use '/n'.   To sort in reverse (while regarding other sorting options) use '/r'.

### Print formatting options

By default **onm** displays the file offset (in hexadecimal) with a symbol type and with mangled symbol names.  

To cause it to unmangle the symbol names use '/C'.  To change the display radix use '/to' '/td' or '/tx' which support octal, decimal, and hexadecimal respectively.   To show the file name of each symbol on the same line of the symbol (instead of displaying it at the beginning of each file's data) use '/A'.


As an example onm may display something like for a symbol:

>     4582 T _myfunc

here 4582 is the offset in the file, the symbol type 'T' means it is a global in the text segment (normally a function) and _myfunc is the (mangled) name of the symbol.

Valid symbol types are as follows:

A - Absolute
B - BSS, global
b - BSS, local
C - Common
D - Initialized Data, Global
d - Initialized Data, Local
i - import or other DLL related
N - debugging symbol (usually autos)
R - const or string data
r - const or string data
S - section symbol
T - Text (functions), global
t - Text (functions), local
U - Unknown

### Alternative display options

 The **/V** switch shows version information, and the compile date

 The **/!** or **--nologo** switch is 'nologo'