## OCC Defining Macros

### /Dxxx    define a macro
 
This switch defines a macro as if a \#define statement had been used somewhere in the source.  It is useful for building different versions of a program without modifying the source files between compiles.  Note that you may not give macros defined this way a value.  For example:
 
     OCC /DORANGE myfunc.c
 
is equivalent to placing the following statement in the file and compiling it.
 
     #define ORANGE 

## Standard Predefined Macros

The following standard macros are predefined by the compiler:

|Macro |Usage |
|--- |--- |
|\_\_FILE\_\_| name of source file|
|\_\_DATE\_\_| the current date|
|\_\_DATEISO\_\_| the current date (ISO format)| 
|\_\_TIME\_\_| the current time|
|\_\_LINE\_\_| the current line number|



## Non-standard predefined macros

The following non-standard macros are predefined by the compiler:

|Macro |Usage |
|--- |--- |
|\_\_ORANGEC\_\_|always defined, value is "major version * 100 + minor version"|
|\_\_ORANGEC\_MAJOR\_\_|always defined, value is "major version"|
|\_\_ORANGEC\_MINOR\_\_|always defined, value is "minor version"|
|\_\_ORANGEC\_PATCHLEVEL\_\_|always defined, value is "build number"|
|\_\_VERSION\_\_|always defined, rendition of the major/revision/minor/build as a string|
|\_\_RTTI\_\_|defined when C++ rtti/exception handling info is present|
|\_\_386\_\_|always defined|
|\_\_i386\_\_|always defined|
|\_i386\_|always defined|
|\_\_i386|always defined|
|\_\_WIN32\_\_|defined for WIN32|
|\_WIN32|defined for WIN32|
|\_\_DOS\_\_|defined for MSDOS|
|\_\_RAW\_IMAGE\_\_|defined if building a raw image|
|\_\_LSCRTL\_DLL|defined when LSCRTL.dll is in use|
|\_\_MSVCRT\_DLL|defined when MSVCRT.dll is in use|
|\_\_CRTDLL\_DLL|defined when CRTDLL.dll is in use|



  
