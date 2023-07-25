# DLPE

 
 **DLPE** is the postprocessor used to create Win32 executables.  There is quite a bit of linker defaults built around it; normally it will be called by the linker in response to use of a linker **/T** switch that specifies a WIN32 output format..  It would be rare for a user to need to call it directly.


## Command Line Options
 

 The general form of a **DLPE** command line is:
 
>     dlpe [options] relfile [resourcefile]
 
 Here the  _relfile_ is the linker generated file that  holds the linked code, and the optional _resourcefile_ can be used to add resources to an executable.
 
 Resource files can be specified on the command line and DLPE will build a resource section for the exectuble.  For example:
 
>     dlpe test.rel test.res
 
 makes a console application called** test.exe **from the linked code in** test.rel**, using resources as specified in **test.res**.
 
 There are several command line options that control the output.  These include options for specifying the output format, the output file name, and optionally a stub file to use.


### Specifying the output file name
 

 
By default, **DLPE** will take the input file name, and replace the extension with the extension .EXE to form an output file 
name.
 
However in some cases it is useful to be able to specify the output file name.  The specified name can have its extension 
specified, or it can be typed without an extension to allow dlhex to add one of the default extensions.  The output file name 
is specified with the **/o** switch.  For example:     

>     dlpe /mGUI /omyfile test.rel
 
 makes a windowing executable called>      **myfile.exe**.

### Specifying the output file format

**DLPE** supports several types of WIN32 Executables with the **/m** switch.  the switch is followed by one of the folowing:
 
* CON - a console application
* GUI - a windowing application
* DLL - a DLL
   
  The default output format if no **/m** switch is specified is the console format.

### Creating a .def file with exports

When creating a DLL, the switch --output-def instructs dlpe to use oimplib to create a .def file with the exports.

>     dlpe /mDLL /otest --output-def test.def test.rel

creates a DLL and outputs the exports file test.def from test.rel.

### Specifying a stub file

By default **DLPE** will add an MSDOS stub file which simply says that the program requires WIN32, if someone happens to run it 
on MSDOS.  However, with the **/s** switch a specific stub can be specified.  This might be useful for example with certain 
MSDOS DPMI extenders that mimic the WIN32 API for console mode programs.  The **/s** switch is used to change the stub file 
name.  For example:
 
>     dlpe /smystub.exe test.rel
 
 makes an output file **test.exe** which has the 16-bit program **mystub.exe** as its MSDOS stub.

### Alternative display options

 The **/V** switch shows version information, and the compile date

 The **/!** or **--nologo** switch is 'nologo'

### Delay loading of DLLs

There are three switches to support WIN32 delay loading of DLLs.

>     dlpe /dln:USER32.dll test.rel

adds user32.dll to the delay load table.   It does not create a binding table.

To have it create a binding table use -dlb:

>     dlpe /dln:USER32.dll /dlb test.rel

To add a table for manually unloading the dll use -dlu

>     dlpe /dln:USER32.dll /dlu test.rel

### Environment Variables consumed by DLPE

The environment variable SOURCE_DATE_EPOCH will be checked when DLPE runs.   If it exists, it is expected to hold a 32-bit time_t value.
Timestamps in the executable which are normally set to the current time will use this value instead.