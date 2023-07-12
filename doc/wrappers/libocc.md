# libocc

 ** libocc** is a wrapper around the msvc lib.exe command line.  It translates the command line to a format suitable for olib then calls olib.


## Command Line Options

 The general format of an **libocc** command line is:
 
>     libocc [options] [list of files]
 
where options are various command line options that can be used to customize the behavior, and the list of files that olib should operate on.  If there is no output file option specified the output file name will be derived from the first input file.


### Librarian maintenance

The librarian understands three commands.   These are replace file, delete file, and extract file.

To add a file to the library 'lib.l'

>     libocc file.o

to add a file to the library 'mylib.l'

>     libocc /OUT:mylib file

To remove a file:

>     libocc /OUT:lib.l /REMOVE:file.o

To extract it:

>     libocc /OUT:lib.l -/EXTRACT:file.o

to create an import library from a dll

>     libocc /NAME:mydll.dll

to create an import library named hi.l from a dll:

>     libocc /OUT:hi.l /NAME:mydll.dll

### Compatibility commands

Several commands are parsed, but don't do anything.   These include

>     libocc /LIBPATH:XXXX file.o
>     libocc /LIST:XXXX file.o
>     libocc /MACHINE:XXXX file.o
>     libocc /NODEFAULTLIB:XXXX file.o
>     libocc /SUBSYSTEM:XXXX file.o
>     libocc /VERBOSE file.o
>     libocc /WX file.o
