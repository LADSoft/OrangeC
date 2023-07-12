# linkocc

 ** linkocc** is a wrapper around the msvc link.exe command line.  It translates the command line to a format suitable for olink then calls olink.


## Command Line Options

 The general format of an **linkocc** command line is:
 
>     linkocc [options] [list of files]
 
where options are various command line options that can be used to customize the behavior, and the list of files that olib should operate on.  If there is no output file option specified the output file name will be derived from the first input file.


### Linker commands

To create an executable

note that linkocc fills in the compiler-specific modules needed to build the executable

>     linkocc file.o

to create a file, linking to a library

>     linkocc file.o library.lib

to create an executeable named myfile.exe

>     linkocc /OUT:myfile file.o

to set a default image base (noting that the size option is ignored if specified):

>     linkocc /BASE:10000000 file.o

to enable the generation of debug symbols:

>     linkocc /DEBUG:FULL file.o

to build a dll:

>     linkocc /DLL file.o

to create an import library when building a dll:
(this generates both the dll and the library

>     linkocc /DLL /IMPLIB:mylib file.o

to set the alignment within the executable file:

>     linkocc /FILEALIGN:1024 file.o

to set a path to search for libraries on

>     linkocc /LIBPATH:..\greetings file.o hello.lib goodbye.lib

to create a map file (note it takes on a default name)

>     linkocc /MAP file.o

to not use the builtin libraries:
(xxx is ignored)

>     linkocc /NODEFAULTLIB:xxx file.o

to set the program stack:
(1MB)
>     linkocc /STACK:1000000 file.o
(1MB with commit of 4095)
>     linkocc /STACK:1000000, 4096 file.o

make a console program

>     linkocc /SUBSYSTEM:CONSOLE file.o

make a windowing program

>     linkocc /SUBSYSTEM:WINDOWS file.o

### Compatibility commands

Several commands are parsed, but don't do anything.   These include

>     linkocc /DEF:xxx  file.o
>     linkocc /ENTRY:xxx  file.o
>     linkocc /EXPORT:xxx  file.o
>     linkocc /MACHINE:xxx  file.o
>     linkocc /MANIFESTFILE:xxx  file.o
>     linkocc /OPT:xxx  file.o
>     linkocc /PROFILE  file.o
>     linkocc /RELEASE  file.o
>     linkocc /SAFESEH:xxx  file.o
>     linkocc /VERBOSE:xxx  file.o
>     linkocc /STUB:xxx  file.o
>     linkocc /WX:xxx  file.o
