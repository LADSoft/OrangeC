# arocc

 ** arocc** is a wrapper around the ar command line.  It translates the command line to a format suitable for olib then calls olib.


## Command Line Options

 The general format of an **arocc** command line is:
 
>     arocc [options] [archive name] [list of files]
 
where options are various command line options that can be used to customize the behavior, and the list of files that olib should operate on


### Librarian maintenance

The librarian understands three commands.   These are replace file, delete file, and extract file.

To add a file to the library 'lib.l'

>     arocc -r lib.l file.o


To remove it again:

>     arocc -d lib.l file.o

To extract it:

>     arocc -x lib.l file.o     


### reserved commands

**arocc** understands various other commands for compatibility, although it doesn't do anything with them.   For example the command to create an archive and add files to it:

>     arocc -cr lib.l file.o

olib automatically creates files if they don't exist, so the 'c' is unnecessary

The commands that are ignored for compatibility reasons are:

'c' 's' 'u' 'v'

### Miscellaneous commands

To print version information:

>    arocc -V   prints version information

To not show the logo:

>    arocc --nologo  -r lib.l file.o
