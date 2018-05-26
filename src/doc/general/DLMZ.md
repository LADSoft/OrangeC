# DLMZ

 
 **DLMZ** is the postprocessor used to create DOS 16-bit executables.  There is quite a bit of linker defaults built around it; normally it will be called by the linker in response to use of a linker **/T** switch that specifies an MSDOS output format..  It would be rare for a user to need to call it directly.


## Command Line Options
 

 The general form of a **DLMZ** command line is:
 
>     dlmz [options] relffile
 
 Here the  _relfile_ is the linker generated file that  holds the linked code.  For example:
 
>     dlmz test.rel
 
 makes a console application called** test.exe **from the linked code in **test.rel**.>     
 
 There are several command line options that control the output.  These include options for specifying the output format, the output file name, and optionally a stub file to use.


### Specifying the output file name
 

 
 By default, **DLMZ** will take the input file name, and replace the extension with the extension .EXE.
 
 However in some cases it is useful to be able to specify the output file name.  The specified name can have its extension specified, or it can be typed without an extension to allow dlhex to add one of the default extensions.  The output file name is specified with the **/o** switch. For example:
>     
>     dlmz /mREAL /omyfile test.rel
 
 makes a segmented executable called **myfile.exe**.>


### Specifying the output file format

 DLMZ supports two types of MSDOS executables with the /m switch, as follows:
 
* TINY - a tiny-mode program
   
* REAL - a segmented program
   
  The default output format if no **/m** switch is specified is REAL.

### Alternative display options

 The **/V** switch shows version information, and the compile date
 The **/!** or **--nologo** switch is 'nologo'

## General Considerations

 This toolchain isn't compatible with the normal MSDOS build process.  Normally, MSDOS programs would have sections that also included something called a class name;  the linker would take both the class name and the section name into account when determining how to create an output file.  Class names aren't supported by this toolchain, instead it is preferred to write a linker specification file to specify how the sections should be combined.  A generic linker specification file exists for each of the supported modes, however, since sections can be named and placed arbitrarily, this specification file would not work for all programs.  It may be necessary to augment an MSDOS program with its own specific linker specification file.  
 
 In tiny mode, it is customary to instruct most MSDOS assemblers to set a code origin of 100h.  However, the linker specification file for tiny mode automatically sets this origin.  Therefore it does not have to be present for this toolchain to generate tiny mode files.  Such files still must start with a code sequence, however.
 
 
