# OCIDE

 ** OCIDE**  is an integrated development environment (IDE).  Most IDE features are best documented in the IDE help files,
however there are a few command line options that may be useful, which are documented here.

## Command Line Options

 The general format of an **OCIDE** command line is:
 
>     OCIDE [options] [list of files]
 
where options are various command line options that can be used to customize the behavior, and the list of files lists files that the IDE should open for editing.


### Help

To view a reminder of what the command line options are, type:

>     OCIDE -h

or

>     OCIDE -?


### Clearing files on startup

To clear files on startup use the **-c** switch

There are two options, which can be combined:

>     OCIDE -cf

closes all edit windows

whereas

>     OCIDE -cw

closes all projects in the selected workspace.

One can also do:

>     OCIDE -cfw

to clear both or

>     OCIDE -c

which is shorthand for the -cf switch

### Loading a workarea

>     OCIDE -wmyworkarea.cwa

loads the workarea myworkarea

### Loading a project

>     OCIDE -pmyproject.exe.cpj

loads the project myproject.exe


One can cascade a workarea and project:

>     OCIDE -wmyworkarea.cwa -pmyproject.exe.cpj

opens myworkarea then loads myproject.exe

At present only one project can be selected on the command line