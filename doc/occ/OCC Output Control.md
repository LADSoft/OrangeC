## OCC Output Control
 

 
 This section deals with compiler parameters related to controlling the type of output.
 
### /c    generate object file only
 
  Causes OCC to not generate an EXE file automatically.  Useful when compiling many files prior to a later link stage.
 
     OCC /c hello.c
 
  generates a file hello.obj instead of generating hello.exe
 
### /o      set output file name
 
  Causes OCC to rename the output file. If generating an EXE output, OCC will rename the exe file.  If generating an object (OBJ) file, OCC will rename the obj file.  Note that you cannot set the output file name for a group of files unless you are generating an EXE file.
 
     OCC /ohi hello.c
 
  generates an EXE file called HI.EXE.

 You can also set the output file to be a directory, in which case object and EXE files will be placed in that directory.  To indicate a directory is to be used, the output file name must have a trailing '\\'.   For example:

    OCC /o..\\output\\ /I..\\include \*.c

 compiles all C language files in the current directory and places the output files in a directory 'output' at the same level.
 
### /S    generate assembly language file only
 
  Causes OCC to generate an assembly language file in NASM format, but no object or EXE files
 
     OCC /c hello.c
 
  generates a file hello.asm
 
### /s    generate intermediate assembly language file
 
  OCC will generate an executable file by compiling via assembly.  The intermediate assembly language file will remain after compilation.
 
     OCC /c hello.c
     
  generates the files hello.asm and hello.exe.
 
### /W     set exe file type
 
  When OCC is generating an EXE file, several formats are possible.  These are as follows:
 
             /Wd     - generate a WIN32 DLL program
             /Wdc    - generate a WIN32 DLL program, use CRTDLL run time library
             /Wdl    - generate a WIN32 DLL program, use LSCRTL run time library
             /Wdm    - generate a WIN32 DLL program, use MSVCRT run time library
             /Wc     - generate a WIN32 console program
             /Wcc    - generate a WIN32 console program, use CRTDLL run time library
             /Wcl    - generate a WIN32 console program, use LSCRTL run time library
             /Wcm    - generate a WIN32 console program, use MSVCRT run time library
             /Wg     - generate a WIN32 gui program
             /Wgc    - generate a WIN32 gui program, use CRTDLL run time library
             /Wgl    - generate a WIN32 gui program, use LSCRTL run time library
             /Wgm    - generate a WIN32 gui program, use MSVCRT run time library
             /We     - generate an MSDOS program (using Tran's PMODE)
             /Wa     - generate an MSDOS program (using DOS32A)
             /Wh     - generate an MSDOS program (using HXDOS/WIN32 runtime)
             /Wr     - generete a RAW program
             /Wx     - generate an MSDOS program (using HXDOS/DOS runtime)
 
     OCC /Wcl hello.c
 
 generates a win32 console program hello.exe, which will use the LSCRTL.DLL run time library.
 
 Note: when compiling files for use with the LSCRTL.DLL, one of the switches /Wdl /Wcl or /Wgl must be present to indicate to the compiler that linkage will be against that library.  Failing to use one of these switches can result in errant run-time behavior. 
