# gccocc

 **gccocc** is a wrapper around the gcc command line.   It reinterprets the command line arguments and issues them to occ

## Command Line Options

 The general format of an **gcc** command line is:
 
>     gccocc [options] [list of files]
 
where options are various command line options that can be used to customize the behavior, and the list of files lists files to be compiled/assembled/linked

### Compiling options

Normally the compiler will compile a C or C++ program, and link it.   There are some options to control that behavior, however.

>     gccocc -c file.c

compiles a C file without linking

>     gccocc file.cpp

compiles a C++ file, linking it

>     gccocc file.s

assemble a file and link it

>     gccocc -S file.c

creates an assembly file output, without linking

>     gccocc file.s

assemble a file and link it

>     gccocc file.o file1.o

link two object files

>     gccocc -E file.c

### Generating debug information

>     gccocc -g file.c

compiles without optimizing, and generates debug information

>     gccocc -Ox file.c

sets the optimization level:

level (x) | result | translated to 
----|--------------|--------
0   | means no optimization   | `occ -O-` 
s   | means size optimization | `occ -O1`
1-3, no level specified | means time optimization | `occ -O2`


### Linker commands

>     gccocc -l mylib.l file.c

compiles and links agains mylib.l

>     gccocc -L\temp -l myulib.l file.c

compiles and links against mylib.l, searching for mylib.l in the current directory and in \temp

>     gccocc -dll r.c
>     gccocc -shared r.c

either will compile r.c into a dll

>     gccocc -link r.c

links r.c after compiling.   This is the default behavior so the option doesn't do anything
 
### Preprocessor control

>     gccocc -DHI=7 -DBYE file.c

makes two preprocessor definitions, HI with a value of 7 and BYE, then compiles and links file.c

>     gccoc -UHI file.c

undefines HI, then compiles and links file.c

>     gccocc -I \mydir\home file.c

compiles and links file.c.   additionally, \mydir\home will be searched for include files.

>     gccocc -nostdinc file.c

don't search the system C language include directory

>     gccocc -nostdinc++ file.c

don't search the system C++ language include directory

### Warning control

>     gccocc -W file.c

shows additional warnings that wouldn't normally be shown

>     gccocc -Wno file.c

doesn't show any warnings

### Miscellaneous commands

>     gccocc -v file.c

prints various information while processing the input file

>     gccocc -V

shows version information

>     gccocc --nologo file.c

doesn't show the gccocc logo

>     gccocc -dumpmachine

dumps the machine string

>     gccocc -dumpversion

dumps the version string

>     gccocc -print-file-name=xxx

searches along the library path for the specified library file, and displays the full path of the file

>     gccocc -print-prog-name=xxx

searches for the program name within the occ distributions, and displays the full path of the file

>     gccocc -funsigned-char file.c

compiles and links file.c, treating the 'char' type as if it were unsigned.

### Compatibility commands

Several commands are parsed, but don't do anything.   These include

>     gcc -output-def deffile.def file.c
>     gcc -ixxx file.c
>     gcc -export-all-symbols file.c
>     gcc -static file.c
>     gcc -fsyntax-only file.c
>     gcc -march=native -mtune=native file.c
