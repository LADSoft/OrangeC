HelpScribble project file.
16
YNQ`bsg-113363
0
2
Orange Command Line Tools Help



FALSE


1
BrowseButtons()
0
FALSE

FALSE
TRUE
16777215
0
16711680
8388736
255
FALSE
FALSE
FALSE
FALSE
150
50
600
500
TRUE
FALSE
1
FALSE
FALSE
Contents
%s Contents
Index
%s Index
Previous
Next
FALSE

55
10
Scribble10
Welcome to Command Line Tools Help




Writing



FALSE
39
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Welcome to Command Line Tools Help\f1 
\par \cf0\fs24 
\par \b0\f0\fs20 In the background, OCIDE uses several command line tools known as the Orange Toolchain to perform various functions such as compiling and linking.
\par 
\par This document describes the Orange Toolchain in some detail.
\par 
\par This is somewhat of a classic toolchain, except there is an addition of a linker post-processing or download stage to manage turning the linker output file into an executable program.   There are several linker post-processing stages to target different platforms, including a post-processing stage that will generate various types of HEX file formats.
\par 
\par General tools compile or assemble code, and manage the resulting object files
\par 
\par \tab\cf2\strike OCC\cf3\strike0\{linkID=30\}\cf0  is an optimizing C compiler which handles the various flavors of C from C89 through C11.
\par \tab\cf2\strike OAsm\cf3\strike0\{linkID=170\}\cf0  is an x86 assembler.  It uses a syntax that is very similar to the Netwide Assembler (NASM)
\par \tab\cf2\strike OLib\cf3\strike0\{linkID=490\}\cf0  is an object file librarian.
\par \tab\cf2\strike OLink\cf3\strike0\{linkID=420\}\cf0  is an object file linker object file linker.
\par 
\par Linker postprocessing tools take the linker output, and make some sort of device or OS-specific binary image that serves as the final executable\\ image.
\par \tab\cf2\strike DLHex\cf3\strike0\{linkID=510\}\cf0  is the utility to make hex and binary files, for ROM-based images
\par \tab\cf2\strike DLMZ\cf3\strike0\{linkID=530\}\cf0  is the utility to make 16-bit MSDOS executables.  The compiler will not output 16 bit code but this may be used with the assembler.
\par \tab\cf2\strike DLLE\cf3\strike0\{linkID=520\}\cf0  is the utility to make 32-bit MSDOS executables that aren't windows compatible.
\par \tab\cf2\strike DLPE\cf3\strike0\{linkID=540\}\cf0  is the utility to make Windows 32-bit executables.<
\par 
\par There are several external utilities that the IDE doesn't use directly, but can be useful from the command line
\par \tab\cf2\strike OCPP\cf3\strike0\{linkID=470\}\cf0  is a C and assembly language preprocessor.  It understands C89, C99, C11, and OAsm preprocessor directive syntaxes.
\par \tab\cf2\strike OGrep\cf3\strike0\{linkID=390\}\cf0  looks for regular expressions within source code files.
\par \tab\cf2\strike OMake\cf3\strike0\{linkID=460\}\cf0  is a make utility very similar to GNU make.
\par 
\par 
\par Some utilities are specific to developing WIN32 programs:
\par \tab\cf2\strike OImpLib\cf3\strike0\{linkID=480\}\cf0  is a WIN32 import librarian capable of managing imports from DLL and .DEF files.
\par \tab\cf2\strike ORC\cf3\strike0\{linkID=500\}\cf0  is a WIN32 resource compiler.
\par 
\par Information on setting up the tools for the first time may be found \cf2\strike here\cf3\strike0\{linkID=15\}\cf0 .
\par 
\par Here are the \cf2\strike license terms\cf3\strike0\{linkID=20\}\cf0 
\par \fs24 
\par 
\par }
15
Scribble15
Tools Setup




Writing



FALSE
13
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Tool Setup\cf0\b0\f1\fs20 
\par 
\par \f0 The tools require very little in the way of set up.   Essentially, the install package comes with configuration files for each tool, which the tools read on startup to configure their environment.  The configuration files generally reference an environment variable ORANGEC.   For the the tools to work, this environment variable must be set, and the tools bin directory must be on the path.
\par 
\par For example, if you've installed the tools in c:\\orangec, command line commands to set up the environment for the tools might look as follows:
\par 
\par \f2 set ORANGEC=c:\\orangec
\par PATH=c:\\orangec\\bin;%PATH%
\par 
\par 
\par }
20
Scribble20
Copyright




Writing



FALSE
40
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\fs20 The BSD Licensing terms are as follows:
\par 
\par \tab Software License Agreement (BSD License)
\par \tab 
\par \tab Copyright (c) 1997-2013, David Lindauer, (LADSoft).
\par \tab All rights reserved.
\par \tab 
\par \tab Redistribution and use of this software in source and binary forms, 
\par     with or without modification, are permitted provided that the following 
\par     conditions are met:
\par \tab 
\par \tab * Redistributions of source code must retain the above
\par \tab   copyright notice, this list of conditions and the
\par \tab   following disclaimer.
\par \tab 
\par \tab * Redistributions in binary form must reproduce the above
\par \tab   copyright notice, this list of conditions and the
\par \tab   following disclaimer in the documentation and/or other
\par \tab   materials provided with the distribution.
\par \tab 
\par \tab * Neither the name of LADSoft nor the names of its
\par \tab   contributors may be used to endorse or promote products
\par \tab   derived from this software without specific prior
\par \tab   written permission of LADSoft.
\par \tab 
\par \tab THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
\par     AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
\par     THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
\par     PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
\par     OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
\par     EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
\par     PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
\par     OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
\par     WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
\par     OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
\par \tab ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\par \cf0\f1 
\par }
30
Scribble30
OCC




Writing



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 OCC\cf0\b0\f1\fs20 
\par 
\par OCC is an optimiziing compiler\f0  \f1 capable of compilng C language files written to the C99 \f0 and C11 \f1 standard\f0 s\f1 .\f0   \f1 However in its default mode, it compiles to the older standard for\f0  \f1 which most legacy programs are written.
\par 
\par OCC currently only generates\f0  \f1 code for the x86 series processor. Together with the rest of the\f0  \f1 toolchain and supplied libraries, it can be used to create WIN32\f0  \f1 program files. This toolchain also includes extenders necessary\f0  \f1 for running WIN32 applications on MSDOS, so\f0  \f1 OCC may be run on MSDOS and used to\f0  \f1 generate MSDOS programs as well.
\par 
\par By default OCC will spawn the\f0  \f1 necessary subprograms to generate a completed executable from a source\f0  \f1 file.
\par 
\par A companion program, OCL, may\f0  \f1 be used to generate MSDOS executables which depend on one of a variety\f0  \f1 of MSDOS extenders.
\par 
\par The general\f0  \f1 form of an OCC \cf2\strike Command Line\cf3\strike0\{linkID=40\}\cf0  is:
\par 
\par \f2 OCC [options] filename-list\f1 
\par 
\par Where filename-list gives a\f0  \f1 list of files to \f0 compile\f1 . 
\par 
\par \f0 A special filename '-' allows taking input from the console.\f1 
\par 
\par In addition to support for the C99 standard, OCC supports a variety of the usual\f0  \cf2\strike\f1 compiler extensions\cf3\strike0\{linkID=160\}\cf0\f0  \f1  found\f0  \f1 in MSDOS and WIN32 compilers.
\par 
\par OCC \f0 also \f1 supports a range of\f0  \cf2\strike\f1 #pragma\f0  \f1 preprocessor directives\cf3\strike0\{linkID=150\}\cf0  to allow some level of control over the\f0  \f1 generated code. Such directives include support for structure\f0  \f1 alignment, having the CRTL run routines\f0  \f1 as part of its normal startup and shutdown process, and so forth.
\par 
\par }
40
Scribble40
Command Line




Writing



FALSE
32
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Command Line\cf0\b0\f1\fs20 
\par 
\par OCC has a variety of command line parameters, most of which aren't\f0  \f1 needed to just compile a file. It will also allow you to specify\f0  \f1 multiple input files. The current default for OCC is to generate\f0  \f1 executable files. While processing the command line, OCC may\f0  \f1 encounter a command to process command line arguments from a file.
\par 
\par The general format of the command line is as follows:
\par 
\par \f2 OCC [parameters] list of files\f1 
\par 
\par The list of files can be a list of one or more C language files.\f0  \f1 C++ language files are \f0 also supported\f1 . If you don't specify an extension on the\f0  \f1 command line it will default to .C; it will detect a .CPP extension and\f0  \f1 activate C++ mode as required.
\par 
\par \f0 The list of files can include wild card characters.
\par \f1 
\par OCC will accept response files with a list of command line\f0  \f1 options. To use a response file, prefix its name with '@':
\par \f2 
\par OCC [parameters] @resp.cc\f1 
\par 
\par There are a variety of parameters that can be set. \f0 In the latest version of the compiler it is often possible to place a space between the parameter and any file name associated with it.  \f1 Help is\f0  \f1 available for the following:
\par 
\par \f0\tab\cf2\strike Output Control\cf3\strike0\{linkID=50\}\cf0 
\par \tab\cf2\strike Error Control\cf3\strike0\{linkID=60\}\cf0 
\par \tab\cf2\strike List File Control\cf3\strike0\{linkID=70\}\cf0 
\par \tab\cf2\strike Preprocessor File Contro\cf3\strike0\{linkID=80\}\cf0 l
\par \tab\cf2\strike Compilation Modes\cf3\strike0\{linkID=90\}\cf0 
\par \tab\cf2\strike Defining Macros\cf3\strike0\{linkID=100\}\cf0 
\par \tab\cf2\strike Specifying Include Paths\cf3\strike0\{linkID=110\}\cf0 
\par \tab\cf2\strike Translating Trigraphs\cf3\strike0\{linkID=120\}\cf0 
\par \tab\cf2\strike Code Generation Parameters\cf3\strike0\{linkID=130\}\cf0 
\par \tab\cf2\strike Optimizer Parameters\cf3\strike0\{linkID=140\}\cf0\f1 
\par 
\par }
50
Scribble50
Output Control




Writing



FALSE
83
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset0 Courier;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\fi-700\li700\cf1\b\fs32 Output Control\cf0\b0\f1\fs20 
\par 
\par \f0 The following switches are available for output control:
\par \f1 
\par \b /c\b0  generate object file only
\par 
\par \f0\tab\f1 Causes OCC to not generate an EXE file\f0  \f1 automatically. Useful when compiling many files prior to a later\f0  \f1 link stage.
\par 
\par \f0\tab\f2 OCC /c hello.c
\par \f1 
\par \f0\tab\f1 generates a file hello.o instead of generating\f0  \f1 hello.exe
\par 
\par \b /o\b0   set output file name
\par 
\par \f0\tab\f1 Causes OCC to rename the output file. If generating\f0  \f1 an EXE output, OCC will rename the exe file. If generating an\f0  \f1 object (OBJ) file, OCC will rename the obj file. Note that you\f0  \f1 cannot set the output file name for a group of files unless you are\f0  \f1 generating an EXE file.
\par 
\par \f0\tab\f2 OCC /ohi hello.c\f1 
\par 
\par \f0\tab\f1 generates an EXE file called HI.EXE.
\par 
\par \f0\tab whereas 
\par 
\par \tab\f2 OCC /c /obeep hello.c\f0 
\par 
\par \tab generates an object file called BEEP.O.\f1 
\par 
\par \f0\tab\f1 You can also set the output file to be a directory, in which case object and EXE files will be placed in that directory.  To indicate a directory is to be used, the output file name must have a trailing '\\'.   For example:
\par 
\par     \f0\tab\f2 OCC /o..\\output\\ /I..\\include *.c\f1 
\par 
\par     \f0\tab\f1 compiles all C language files in the current directory and places the output files in a directory 'output' at the same level.
\par 
\par \b /S\b0  generate assembly language file only
\par 
\par \f0\tab\f1 Causes OCC to generate an assembly language file in\f0  \f1 NASM format, but no object or EXE files
\par 
\par \f0\tab\f3 OCC /c hello.c\f1 
\par 
\par \f0\tab\f1 generates a file \f0 HELLO.ASM\f1 
\par 
\par \b /s\b0  generate intermediate assembly language file
\par 
\par \f0\tab\f1 OCC will generate an executable file by compiling\f0  \f1 via assembly. The intermediate assembly language file will remain\f0  \f1 after compilation.
\par 
\par \f0\tab\f2 OCC /s hello.c
\par \f1  
\par \f0\tab\f1 generates the files \f0 HELLO.O, HELLO.ASM, and HELLO.EXE\f1 
\par 
\par \b\f0 /v\b0  generate debug information
\par 
\par \tab OCC will generate debug information for use by the IDE.
\par \f1 
\par /W  set exe file type
\par 
\par \f0\tab\f1 When OCC is generating an EXE file, several formats\f0  \f1 are possible. These are as follows:
\par 
\par \f0\tab\f1 /Wd - generate a WIN32 DLL program 
\par \f0\tab\f1 /Wdc - generate a WIN32 DLL program, use CRTDLL\f0  \f1 run time library
\par \f0\tab\f1 /Wdl - generate a WIN32 DLL program, use LSCRTL\f0  \f1 run time library
\par \f0\tab\f1 /Wdm - generate a WIN32 DLL program, use MSVCRT\f0  \f1 run time library
\par \f0\tab\f1 /Wc - generate a WIN32 console program
\par \f0\tab\f1 /Wcc - generate a WIN32 console program, use\f0  \f1 CRTDLL run time library
\par \f0\tab\f1 /Wcl - generate a WIN32 console program, use\f0  \f1 LSCRTL run time library
\par \f0\tab\f1 /Wcm - generate a WIN32 console program, use\f0  \f1 MSVCRT run time library
\par \f0\tab\f1 /Wg - generate a WIN32 gui program
\par \f0\tab\f1 /Wgc - generate a WIN32 gui program, use CRTDLL\f0  \f1 run time library
\par \f0\tab\f1 /Wgl - generate a WIN32 gui program, use LSCRTL\f0  \f1 run time library
\par \f0\tab\f1 /Wgm - generate a WIN32 gui program, use MSVCRT\f0  \f1 run time library
\par \f0\tab\f1 /We - generate an MSDOS program (using Tran's\f0  \f1 PMODE)
\par \f0\tab\f1 /Wa - generate an MSDOS program (using DOS32A)
\par \f0\tab\f1 /Wh - generate an MSDOS program (using\f0  \f1 HXDOS/WIN32 runtime)
\par \f0\tab\f1 /Wr - generete a RAW program
\par \f0\tab\f1 /Wx - generate an MSDOS program (using\f0  \f1 HXDOS/DOS runtime)
\par \f2 
\par \tab\tab OCC /Wcl hello.c
\par 
\par \f0\tab\tab\f1 generates a win32 console program hello.exe, which\f0  \f1 will use the LSCRTL.DLL run time library.
\par 
\par \f0\tab\f1 Note: when compiling files for use with the\f0  \f1 LSCRTL.DLL, one of the switches /Wdl /Wcl or /Wgl must be present to\f0  \f1 indicate to the compiler that linkage will be against that\f0  \f1 library. Failing to use one of these switches can result in\f0  \f1 errant run-time behavior.
\par \pard 
\par }
60
Scribble60
Error Control




Writing



FALSE
36
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Error Control\cf0\b0\f1\fs20 
\par 
\par \pard\fi-700\li700\f0 The following switches are available for error messages control:\f1 
\par \pard 
\par \pard\fi-720\li720\b +e\b0   put the compiler errors in a file. 
\par 
\par \f0\tab\f1 For each file processed, OCC will create a file with\f0  \f1 the same name as the original source with the extension.'.err'. The contents of this file will be a listing of any errors\f0  \f1 or warnings \f0 w\f1 hich occurred during the compile. For example:
\par 
\par \f2\tab OCC +e myfile.c
\par \f1 
\par \f0\tab\f1 results in myfile.err
\par 
\par \b +Q\b0  Quiet mode
\par 
\par \f0\tab\f1 Don't display errors or warnings on the\f0  \f1 console. Generally this is used in conjunction with the +e\f0  \f1 switch. For example:
\par \f2 
\par \tab OCC +e +Q myfile.c\f1 
\par 
\par \f0\tab\f1 puts the errors in a file, without displaying them\f0  \f1 on the console.
\par 
\par 
\par \b /E[+\f0 -\f1 ]nn\b0  error control
\par 
\par \f0\tab\f1 nn is the maximum number of errors before the\f0   \f1 compile fails; if + is specified extended warnings will be shown that\f0  \f1 are normally disabled by default. \f0 If - is specified warnings will be turned off.  \f1 For example:
\par 
\par \f2\tab OCC /E+44 myfile.c\f1 
\par 
\par \f0\tab\f1 enables extended warnings and limits the number of\f0   \f1 errors to 44. By default only 25 errors will be shown and then\f0  \f1 the compiler will abort\f0  and
\par \f2 
\par \tab OCC /E- myfile.c\f0 
\par 
\par \tab compiles myfile.c without displaying any warnings.\f1 
\par 
\par }
70
Scribble70
List File Control




Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 List File Control\cf0\b0\f1\fs20 
\par 
\par \pard\fi-700\li700\f0 The following switches are available for list file control:
\par \pard\f1 
\par \b +l\b0  create a listing\f0  \f1 file 
\par 
\par \pard\fi-700\li700\f0\tab\f1 For each file processed, OCC will create a file with\f0  \f1 the same name as the original source with the extension.\f0   \f1 '.lst'. The contents of this file will be various information\f0  \f1 gathered about the program which was processed. For example:
\par 
\par \f2\tab OCC +l myfile.c\f1 
\par 
\par \f0\tab\f1 results in myfile.lst
\par 
\par }
80
Scribble80
Preprocessor File Control




Writing



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Preprocessor File Control\cf0\b0\f1\fs20 
\par 
\par \pard\fi-700\li700\f0 The following switches are available for preprocessor control\f1 
\par \pard 
\par \pard\fi-700\li700\b +i\b0  create a file with\f0  \f1 preprocessed text. 
\par 
\par \f0\tab\f1 For each file processed, OCC will create a file with\f0  \f1 the same name as the original source with the extension.\f0   \f1 '.i'. The contents of this file will be the source code, with\f0  \f1 each identifier which corresponded to a macro expanded out to its full\f0  \f1 value. For example:
\par 
\par \f2\tab OCC +i myfile.c\f1 
\par 
\par \f0\tab\f1 results in myfile.i\f0  where each #defined identifier replaced with its value.
\par 
\par \f1 
\par \pard 
\par }
90
Scribble90
Compilation Modes




Writing



FALSE
31
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Compilation Modes
\par \cf0\b0\f1\fs20 
\par \f0 The following switches are available to set the language compatibility mode:
\par \f1 
\par \b +A\b0  disable non-ansi\f0  \f1 extensions
\par 
\par \f0\tab\f1 By default the compiler allows several extensions to\f0  \f1 ansi, to make coding easier. If you want strict adherence to\f0  \f1 ansi, use this switch. For example:
\par 
\par \f2\tab OCC +A myfunc.c
\par 
\par \f0\tab\f1 will enable ANSI mode
\par 
\par \b +9\b0  C99 Compatibility
\par 
\par \f0\tab\f1 By default the compiler compiles for pre-99\f0  \f1 standard. If you want extended features available in the later\f0  \f1 C99 standard, use this switch. For example:
\par 
\par \f2\tab OCC /9 myfunc.c
\par \f1 
\par \f0\tab\f1 Will enable C99 mode.
\par \f0 
\par \b +1\b0  C11 Compatibility\f1 
\par 
\par \f0\tab Enables compatibility with the C11 standard.  For example:
\par 
\par \f2\tab OCC /1 myfunc.c\f1 
\par 
\par \f0\tab\f1 Will enable C\f0 11\f1  mode.
\par 
\par }
100
Scribble100
Defining Macros




Writing



FALSE
33
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Defining Macros\cf0\b0\f1\fs20 
\par 
\par \f0 The following switches are useful for definint macros:
\par \f1 
\par \b /Dxxx\b0  define a macro
\par \pard\fi-700\li700 
\par \f0\tab\f1 This switch defines a macro as if a #define\f0  \f1 statement had been used somewhere in the source. It is useful for\f0  \f1 building different versions of a program without modifying the source\f0  \f1 files between compiles. Note that you may not give macros defined\f0  \f1 this way a value. For example:
\par 
\par \f2\tab OCC /DORANGE myfunc.c\f1 
\par 
\par \f0\tab\f1  is equivalent to placing the following statement\f0  at the beginning of the\f1  \f0 source \f1 file and compiling it.
\par 
\par \f2\tab #define ORANGE \f1 
\par 
\par \f0 In addition to standards compliant macros, the following macros are pre-defined by the compiler:
\par 
\par \pard\fi-320\li320\tx2040 __ORANGEC__\tab always defined
\par __RTTI__\tab defined when C++ rtti/exception handling info is present\f1 
\par \f0 __386__\tab always defined
\par __i386__\tab always defined
\par _i386_\tab always defined
\par __i386\tab always defined
\par __DOS__\tab defined for MSDOS
\par __WIN32__\tab defined for WIN32
\par _WIN32\tab defined for WIN32
\par __RAW_IMAGE__\tab defined if building a raw image
\par __LSCRTL_DLL\tab defined when LSCRTL.dll is in use
\par __MSVCRT_DLL\tab defined when MSVCRT.dll is in use
\par __CRTDLL_DLL\tab defined when CRTDLL.dll is in use
\par 
\par }
110
Scribble110
Specifying Include Paths




Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Specifying Include Paths\cf0\b0\f1\fs20 
\par 
\par \f0 The following switches are useful for specifying where to find #included files.
\par \f1 
\par \b /Ipath\b0  specify include\f0  \f1 path. 
\par 
\par \pard\fi-700\li700\f0\tab\f1 If your file uses headers that aren't in the\f0  \f1 directory you are running \f0 Orange C\f1  from, you will have to tell it what\f0  \f1 directory to look in. You can have multiple search directories by\f0  \f1 using a semicolon between the directory names. If there are\f0  \f1 multiple files with the same name in different directories being\f0  \f1 searched, CC386 will use the first instance of the file it finds by\f0  \f1 searching the path in order. For example:
\par 
\par \f0\tab\f2 OCC /I..\\include;..\\source;c:\\libraries\\include myfile.c
\par 
\par \f0\tab\f1 Will search the paths ..\\include , ..\\source, and\f0  \f1 c:\\libraries\\include in that order. Note that you generally don't\f0  \f1 have to specify a path to the OCC compiler header files such as\f0  \f1 stdio.h, as th\f0 ey will be added to the list of paths automatically\f1 
\par 
\par }
120
Scribble120
Translating Trigraphs




Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Translating Trigraphs\cf0\b0\f1\fs20 
\par 
\par \f0 The following switches are available to aid in translating trigraphs.
\par \f1 
\par \b /T\b0  Translate trigraphs
\par 
\par \f0\tab\f1 use th\f0 is\f1  switch to have OCC translate\f0  \f1 trigraphs. By default OCC will not translate trigraphs for\f0  \f1 compatibility and to compile slightly faster. For example:
\par 
\par \f0\tab\f2 OCC /T myfile.c
\par \f1 
\par \f0\tab\f1 Translates any trigraphs in the text.
\par 
\par }
130
Scribble130
Code Generation Parameters




Writing



FALSE
69
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\fi-700\li700\cf1\b\fs32 Code Generation Parameters\cf0\b0\f1\fs20 
\par 
\par \f0 The following switches guide the code generation process
\par \f1 
\par \b /Cparams\b0  specifies code generation\f0  p\f1 arameters
\par 
\par \f0\tab\f1 Params is a list of parameters, seperated by + and -\f0  \f1 symbols. Use the + symbol to indicate a parameter is to be turned\f0  \f1 on, the minus symbol that the parameter is to be turned off. The\f0  \f1 default states of the various parameters are opposite what is shown\f0  \f1 here; i.e. this lists how to change the default state to something else.
\par 
\par \f0 Where params is one or more of:
\par \f1 
\par  \b +d\b0  display diagnostics
\par 
\par  \f0\tab\f1 displays memory diagnostics, and\f0  \f1 some indication of what internal errors have occurred
\par 
\par  \b -b\b0  merge BSS with initialized data
\par 
\par \f0\tab\f1 Normally there are two segments\f0  \f1 used, one for initialized data and one for uninitialized data.\f0   \f1 This prevents the OS from having to load uninitialized data from a\f0  \f1 file; instead the program just zeroes it during startup. This\f0  \f1 switch merges the two sections into one initialized data section.
\par 
\par  \b -l\b0  don't put C source in the ASM\f0  \f1 file
\par 
\par \f0\tab\f1 When an ASM file output option is\f0  \f1 specified, this will create an ASM file without cross-referencing the\f0  \f1 assembly code to the lines of the source file.
\par 
\par  \b -m\b0  don't use leading underscores
\par 
\par \f0\tab\f1 Normal C language procedure is to\f0  \f1 prepend each identifier with an underscore. If you want to use\f0  \f1 the compiler to generate function and variable names without an\f0  \f1 underscore use this switch. However, doing so will create an\f0  \f1 incompatibility with the run time libraries and you won't be able to\f0  \f1 link.
\par 
\par  \b +r\b0  reverse order of bit operations
\par 
\par \f0\tab\f1 Normally bit fields are allocated\f0  \f1 from least significant bit to most significant bit of whatever size\f0  \f1 variable is being used. Use this switch to reverse the order,\f0  \f1 however this may create incompatibilities with the libraries which\f0  \f1 result in code bugs.
\par 
\par  \b +s\b0  align stack
\par 
\par \f0\tab\f1 This switch causes OCC to emit\f0  \f1 code to align the stack to 16-byte boundaries. This is useful to\f0  \f1 speed up operations that involve loading and storing double-precision\f0  \f1 floating point values to auto variables. By default, the run-time\f0  \f1 libraries cause main() or WinMain() to execute with an aligned stack.
\par 
\par  \b +F\b0  use FLAT model in ASM file
\par 
\par \f0\tab\f1 When using ASM file, select FLAT\f0  \f1 model.
\par 
\par  \b +I\b0  use Microsoft-style imports
\par 
\par \f0\tab\f1 Normally the linker creates a\f0  \f1 thunk table with jump addresses that jump indirectly through the import\f0  \f1 table. This allows basic C code to compile and link.\f0   \f1 However some linkers do not support this and instead need the compiler\f0  \f1 to call indirectly through the import table rather than to a thunk\f0  \f1 table. Use this switch to generate code compatible with these\f0  \f1 linkers.
\par 
\par  \b +M\b0  generate MASM assembler file
\par 
\par  \b +N\b0  generate NASM assembler file\f0  \f1 
\par 
\par  \b +NX\b0  generate generic NASM\f0  \f1 assembler file
\par 
\par  \b +R\b0  use the far keyword to create\f0  \f1 far pointers or far procedure frames
\par 
\par \b  +S\b0  add stack checking code
\par   
\par \f0\tab\f1 This switch adds calls to the\f0  \f1 run-time library's stack checking code. If the stack overruns, an\f0  \f1 error is generated.
\par 
\par  \b +T\b0  generate TASM assembler file
\par 
\par  \b +U\b0  do not assume DS == SS
\par 
\par  \b +Z\b0  add profiler calls
\par 
\par \f0\tab\f1 This switch adds calls to a\f0  \f1 profiler module at the beginning and ending of each compiled\f0  \f1 function. This is DOS compatibility; the WIN32 profiler module\f0  \f1 does not exist at present.\f0  \f1  \f0 F\f1 or example:
\par 
\par \f0\tab\f2 OCC /C+NX+Z myfile.c\f1 
\par 
\par \f0\tab\f1 generates generic NASM assembler module, with\f0  \f1 profiler calls inserted.
\par \pard 
\par }
140
Scribble140
Optimizer Parameters




Writing



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Optimizer Parameters\cf0\b0\f1\fs20 
\par 
\par \f0 The following switches deal with optimization.
\par \f1 
\par \b /O-\b0  turn off optimizer
\par 
\par \f0\tab\f1 This switch disables the OCC optimizer. For\f0  \f1 example:
\par 
\par \f2\tab OCC /O- myfile.c\f1 
\par 
\par \f0\tab\f1 compiles a program without optimization.
\par 
\par \f0\tab Note that specifying the \b /v\b0  switch will also turn off optimization\f1 
\par 
\par }
150
Scribble150
#Pragma statements




Writing



FALSE
73
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 #Pragma statements\cf0\b0\f1\fs20 
\par 
\par #pragma preprocessor directives control\f0  \f1 the interpretation of source code, or extend the functionality of the\f0  \f1 compiler in some way.
\par \f0 
\par 
\par \cf1 #pragma once\cf0 
\par 
\par #pragma once allows automatic include-guarding.  When #pragma once is encountered, a check will be made to determine if the file has been processed before; if it has, processing wil stop immediately.   To determine if a file has been processed before, the file time, the file size, and the file contents are considered; if they match then the file has been processed before even if it is in a different location or has a different name.
\par 
\par 
\par \cf1 #pragma error\f1 
\par 
\par \cf0 #pragma error \f0 <\f1 text\f0 >\f1  allows conditional\f0  \f1 generation of errors. For example:
\par 
\par  \f2 #ifndef WIN32
\par  #pragma error Not a win32 program
\par  #endif
\par 
\par \f1  generates a compile time error if the WIN32 macro is\f0  \f1 not defined.
\par 
\par 
\par \cf1\f0 #pragma warning\cf0 
\par \f1 
\par #pragma warning \f0 <\f1 text\f0 >\f1  allows conditional\f0  \f1 generation of errors. For example:
\par 
\par \f2  #ifndef LONG
\par  #pragma warning long type not defined
\par  #endif
\par 
\par \f1  generates a compile time warning if the LONG macro\f0  \f1 is not defined.
\par 
\par \f0 
\par \cf1 #pragma aux\cf0\f1 
\par 
\par #pragma aux \f0 <\f1 funcname\f0 >\f1  = \f0 <\f1 alias\f0 > \f1  Creates an alias for a function. The alias\f0  \f1 name is substituted for the function name in the OBJ and ASM output\f0  \f1 files. For example:
\par 
\par  \f2 #pragma aux "myfunc"="mynewname"
\par 
\par \f1  causes the linker to see the function \f0 'myfunc' \f1 as being\f0  \f1 called 'mynewname'.\f0   In the source code you would still write 'myfunc' however.\f1 
\par 
\par 
\par \cf1\f0 #pragma pack\cf0\f1 
\par 
\par #pragma pack(n) Sets the alignment for structure\f0  \f1 members and global variables. The default alignment is 1.\f0  \f1 Changing the alignment can increase performance by causing variable and\f0  \f1 structure alignment to optimal sizes, at the expense of using extra\f0   \f1 memory. However, altered alignment can sometimes cause problems\f0  \f1 for example when a structure is used d\f0 i\f1 rectly in a network packet or as\f0  \f1 the contents of a file.
\par 
\par  The actual alignment of any given variable depends\f0  \f1 both on the value of 'n' and on the size of the variable. CC386\f0  \f1 will pick the minimum of the two values for the alignment of any given\f0  \f1 variable; for example if n is 2 characters will be aligned on byte\f0  \f1 boundaries and everything else will be aligned on two byte\f0   \f1 boundaries. If n is 4 characters will be on byte boundaries,\f0  \f1 words (short quantities) on two-byte boundaries, and dwords (ints) on\f0  \f1 four byte boundaries.
\par 
\par #pragma pack() Resets the alignment to the last\f0  \f1 selection, or to the default.
\par 
\par 
\par \cf1 #pragma startup \f0 <\f1 function\f0 >\f1  \f0 <\f1 priority\f0 >\f1 
\par #pragma rundown \f0 <\f1 function\f0 >\f1  \f0 <\f1 priority\f0 >
\par \cf0\f1 
\par  These two directives allow you to specify functions\f0  \f1 that are automatically executed by the\f0  run time library\f1  before and after the main\f0  \f1 program is executed. The priority scheme allows you to order\f0  \f1 functions in a priority order. When the RTL is executing startup\f0  \f1 or rundown functions it executes all functions at priority 1, then all\f0  \f1 functions at priority 2, then all functions at priority 3, and so\f0  \f1 forth. To have a function executed before your program runs, use\f0  \f1 #pragma startup, or to have it execute after the program exits, use\f0  \f1 #pragma rundown. You should use priorities in the range 50-200,\f0  \f1 as priorities outside that range are used by \f0 run time library\f1  functions and their\f0  \f1 execution (or lack thereof) may prevent some functions in the RTL from\f0   \f1 working properly. For example:
\par 
\par \f2  #pragma startup myfunc 100\f1 
\par 
\par  runs the function 'myfunc' after the RTL functions\f0  \f1 have initialized. Myfunc would be defined as follows:
\par 
\par \f2  void myfunc(void) ;
\par 
\par \f1  Note that #pragma rundown is equivalent to\f0  \f1 atexit. 
\par 
\par 
\par 
\par 
\par 
\par >
\par 
\par 
\par }
160
Scribble160
Compiler Extensions




Writing



FALSE
140
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset0 Courier;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Compiler Extensions\cf0\b0\f1\fs20 
\par 
\par Extended keywords extend ANSI C in a variety of ways that are sometimes\f0  \f1 useful for example to add new functionality (such as \cf1 alloca\cf0  or \cf1 typeof\cf0 )\f0  \f1 or to ease integration with operating systems and programming languages\f0  \f1 (for example \cf1 _\f0 _\f1 stdcall\cf0  or \cf1 _\f0 _\f1 pascal\cf0 ).
\par 
\par \cf1 _absolute\cf0 
\par 
\par  create a variable at an absolute address. Forexample:
\par 
\par \f2  _absolute(0x4c21) int a ;\f1 
\par 
\par  places the variable 'a' at address 0x4c21. No\f0  \f1 storage is generated for such variables and no relocation is done.
\par 
\par 
\par \cf1 alloca
\par \cf0 
\par  Allocate memory from automatic storage (the\f0  \f1 processor stack). The primary motivation for using this function\f0  \f1 is that it is much faster than the malloc() function\f0  and the allocation gets freed automatically at the end of the function\f1 .
\par 
\par  alloca is implicitly defined by the compiler as\f0  \f1 follows:
\par 
\par \f2  void *alloca(size_t size);\f1 
\par 
\par  For example:
\par 
\par  \f2 int size = 24;
\par  int *p = alloca(size * sizeof(int));
\par 
\par \f1  will allocate enough space to store an array of 24\f0  \f1 integers.
\par 
\par  alloca allocates space without checking there is\f0  \f1 enough. If the space used by calls to this pseudo-function plus\f0  \f1 the space used by lower level functions and their data exceeds the
\par stack size, the program will probably crash. 
\par 
\par  Memory allocated by alloca is normally freed at the\f0  \f1 end of the function it appears in, which makes it possible to allocate\f0  \f1 a lot of data in a loop. However, if a block has both a
\par call to alloca and uses variable length arrays, at the end of the block\f0  \f1 the variable length arrays will be freed, which will also result in\f0  \f1 freeing the memory allocated by alloca.
\par 
\par 
\par \cf1 _cdecl\cf0 
\par 
\par  use standard C language linking mechanism (here for\f0  \f1 compatibility with other compilers). In this linking mechanism, a\f0  \f1 leading underscore is prepended to the name, which is case\f0  \f1 sensitive. The caller is responsible for cleaning up the\f0  \f1 stack. For example:
\par 
\par \f2  void _cdecl myfunc() ;\f1 
\par 
\par  creates a function myfunc with standard linkage.
\par 
\par \f0 
\par \cf1 _export\cf0\f1 
\par 
\par  make an export record for the linker to\f0  \f1 process. The current record becomes an entry in the EXE files\f0  \f1 export table. For example:
\par 
\par \f2  void _export myfunc() ;\f1 
\par  
\par  will cause myfunc to be an exported function.
\par 
\par 
\par \cf1\f0 _genbyte\cf0\f1 
\par 
\par  Generate a byte of data into the code segment\f0  \f1 associated with the current function. For example:
\par 
\par  \f2 void myfunc()
\par  \{
\par  .
\par  .
\par  .
\par  _genbyte(0x90) ;
\par  .
\par  .
\par  .
\par  \}
\par 
\par \f1  puts a NOP into the code stream.
\par 
\par \f0 
\par \cf1 _import\cf0 
\par \f1 
\par  This can be used for one of two purposes.\f0   \f1 First it can make import record for the linker to process, which will\f0  \f1 result in the appropriate DLL being loaded at run-time. Second,\f0  \f1 it can be used to declare a variable from a DLL so that the compiled\f0  \f1 code can access it. For example:
\par 
\par \f3  int _import myvariable ;
\par 
\par \f1  declares myvariable as being imported. While
\par 
\par \f3  int _import("mylib.dll") myvariable ;\f1 
\par 
\par  declares myvariable as being imported from mylib.dll.
\par 
\par 
\par \cf1 _interrupt\cf0 
\par 
\par  Create an interrupt function. Pushes registers\f0  \f1 and uses an IRET to return from the function. For example:
\par 
\par  \f2 _interrupt void myfunc() 
\par  \{
\par  \}
\par 
\par \f1  Creates a function myfunc which can be used as an\f0  \f1 interrupt handler.
\par 
\par 
\par  _fault is similar to _interrupt, but also pops the\f0  \f1 exception word from the stack. Used when returning from certain\f0  \f1 processor fault vectors
\par 
\par 
\par \cf1\f0 _loadds\cf0\f1 
\par 
\par  For an Interrupt function, force DS to be loaded at\f0  \f1 the beginning of the interrupt. This will be done by adding 0x10 to the\f0  \f1 CS selector to make a new DS selector. For example:
\par 
\par  \f2 _loadds _interrupt void myfunc() 
\par  \{
\par 
\par  \}\f1 
\par 
\par  will create an interrupt function that loads DS
\par 
\par 
\par \cf1\f0 _pascal\cf0\f1 
\par   
\par  use PASCAL linking mechanism. This linking\f0  \f1 mechanism converts the function name to upper case, removes the leading\f0  \f1 underscore, pushes arguments in reverse order from \f0 s\f1 tandard functions,\f0   \f1 and uses callee stack cleanup. For example:
\par 
\par \f2  void _pascal myfunc() ;
\par 
\par \f1  Creates a function myfunc with pascal linkage.
\par 
\par 
\par \cf1\f0 _stdcall\cf0\f1 
\par 
\par  Use STDCALL linking mechanism. This linking\f0  \f1 mechanism removes the leading underscore and uses callee stack\f0  \f1 cleanup. For example:
\par 
\par  \f2 void _stdcall myfunc() ;
\par 
\par \f1  Creates a function myfunc with pascal linkage.\f0   This is the linkage method most windows functions use.\f1 
\par 
\par \cf1 
\par \f0 typeof\f1 
\par \cf0 
\par  the typeof operator may be used anywhere a type\f0  \f1 declaration is used, e.g. as the base type for a variable or in a cast\f0  \f1 expression. It allows you to access the variable's type \f0 w\f1 ithout\f0  \f1 explicitly knowing what that type is. For example:
\par \f2 
\par  long double aa ;
\par  typeof(aa) bb;
\par 
\par \f1  declares bb as long double type.
\par 
\par }
170
Scribble170
OAsm




Writing



FALSE
72
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green255\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 OAsm\cf0\b0\f1\fs20 
\par 
\par OAsm is an intel x86 assembler\f0  \f1 in the spirit of the Netwide Assembler\f0  \f1 (NASM). While it shared many features with NASM, programs\f0  \f1 written for NASM are not 100% compatible with it. 
\par 
\par OAsm converts a textual form of\f0  \f1 processor-specific instructions into numbers the computer will\f0  \f1 understand. However, discussion of these processor-specific\f0  \f1 instructions is beyond the scope of this documentation; instead the\f0  \f1 documentation will focus on various other features\f0  \f1 OAsm presents to\f0  \f1 help easily create code and data.
\par 
\par OAsm supports both 16 and\f0  \f1 32-bit code.
\par 
\par In comparison to standard Intel assemblers, OAsm\f0  \f1 shares some features. But unlike most other Intel assemblers,\f0  \f1 which keep\f0  \f1 track of the type of variables, OAsm\f0  \f1 is typeless. Which means any\f0  \f1 time a variable is used in an ambiguous context, type information has\f0  \f1 to be provided. Another major difference is the interpretation of\f0   \f1 brackets... in an Intel assembler brackets around simple variable\f0  \f1 names are often optional, with the assembler interpreting:
\par 
\par \f2 mov ax,[myvar]
\par \f1 
\par and
\par 
\par \f2 mov ax,myvar\f1 
\par 
\par in the same way. In these assemblers, an additional keyword\f0  \f1 offset is\f0  \f1 used to refer to a variable's address:
\par 
\par \f2 mov ax,offset myvar\f1 
\par 
\par However in OAsm, the\f0  \f1 offset keyword is done away\f0  \f1 with. Instead,\f0  \f1 the brackets are given a more concrete meaning. When they\f0  \f1 exist, they indicate the value of a variable; absence of\f0  \f1 the brackets denotes the address of the variable.
\par 
\par OAsm understands the usual\f0  \f1 comment syntax starting with a ';' and extending\f0  \f1 to the end of the line. For example in:
\par 
\par \f2 sub eax,edx \cf2 ; normalize\cf0 
\par \f1 
\par everything after the semicolon is a comment and is ignored.
\par 
\par Additionally OAsm understands C and C++ style comments since it uses an\f0  \f1 extended C language preprocessor. For example to write a block of\f0  \f1 comments use /* and */:\f0   \f1 Everything between the /* and the */ is a comment\f0 . \f1  Multiple lines may be typed\f0  between these sequences\f1 .\f0   
\par 
\par \f1 The final commenting style is the C++ double slash style, which is\f0  \f1 similar to ';' except uses a '//' sequence to delimit the beginning of\f0  \f1 the comment:
\par 
\par \f2 sub eax,edx \cf2 // normalize\cf0 
\par \f1 
\par The general\f0  \f1 form of an OAsm \cf3\strike Command Line\cf4\strike0\{linkID=180\}\cf0  is:
\par 
\par \f2 OAsm [options] filename-list
\par \f1 
\par Where filename-list gives a\f0  \f1 list of files to assemble. 
\par 
\par 
\par OAsm\f0  \f1 understands several integer number formats, as well as floating\f0  \f1 point. It is also capable of evaluating comp\f0 lex \cf3\strike\f1 expressions\cf4\strike0\{linkID=290\}\cf0\f0  \f1 involving numbers, labels, and special symbols for things like the\f0  \f1 current program counter. Some of these expressions can be\f0  \f1 evaluated immediately; others are pushed off to the linker.
\par 
\par There are three\f0  \f1 kinds of\f0  \cf3\strike\f1 labels\cf4\strike0\{linkID=280\}\cf0 . Each Standard\f0  \f1 label may be defined at most once\f0  \f1 in a source file. Such labels have a global context and are\f0  \f1 accessible\f0  \f1 from anywhere within the current source modules, and sometimes from\f0  \f1 other modules as well. 
\par 
\par On the other hand Local labels inherit a context from standard\f0  \f1 labels, and may be defined multiple times provided they are\f0  \f1 defined at most once between the occurrance of any two\f0  \f1 standard\f0  \f1 labels. 
\par 
\par Non-local labels\f0  \f1 are sometimes useful - they share the idea of having a global context\f0  \f1 with standard labels, but don't start a new context for local labels.
\par 
\par Directives are\f0  \f1 psuedo-instructions to the assembler which guide how the assembly is\f0  \f1 done. In the most rudimentary form, they can provide a mechanism\f0  \f1 for defining numeric or\f0  t\f1 extual values, or reserving space. Other\f0  \f1 directives allow breaking the code and data up into distinct sections\f0  \f1 such as CODE and DATA that can be linked against other files.
\par 
\par Natively, directives are always enclosed in brackets, for example to\f0  \f1 define a byte of data:
\par 
\par \f2 myvar [db 44]
\par \f1 
\par However, the directives are redefined with default macros, so that the\f0  \f1 brackets are not necessary when typing code:
\par 
\par \f2 myvar db 44\f1 
\par 
\par Macros are described further in the section on the \cf3\strike preprocessor\cf4\strike0\{linkID=300\}\cf0 .
\par 
\par Some of the macro redefinitions of the directives are simply a mapping\f0  \f1 from\f0  \f1 the non-bracket version to the bracketized version. Other macros\f0  \f1 are more complex, adding behavior to\f0  \f1 the assembler. For example\f0  \f1 the macros for\f0  \f1 psuedo-structures also define the structure size, and keep track of the\f0  \f1 current section and switch back to it when done.
\par 
\par OAsm uses a \cf3\strike\f0 C99-style\cf4\strike0\{linkID=310\}\cf0  \cf3\strike\f1 preprocessor\cf4\strike0\{linkID=300\}\cf0 , which has been\f0  \f1 enhanced in various\f0  \f1 ways. One difference is that instead of using '#' to start a\f0  \f1 preprocessor statement, OAsm\f0  \f1 uses '%'. This does not apply to the\f0  \f1 stringizing and tokenizing sequences however; those still use '#'.
\par 
\par \cf3\strike Basic\f0  \f1 extensions to the preprocessor\cf4\strike0\{linkID=320\}\cf0  include %assign, which is similar to\f0  \f1 %define except the preprocessor\f0  \f1 will evaluate an arithmetic expression as part of the\f0  \f1 assignment. Other extensions include \cf3\strike repeat blocks\cf4\strike0\{linkID=380\}\cf0  and\f0  \cf3\strike\f1 multiline macros\cf4\strike0\{linkID=370\}\cf0 .
\par 
\par \f0 The basic \cf3\strike conditional\cf4\strike0\{linkID=330\}\cf0  statements allow for a variety of ways to conditionally compile code.  \f1 There are also various types of\f0  conditional \f1 statements which have been added to do \cf3\strike textual comparisons\cf4\strike0\{linkID=340\}\cf0\f0  \f1 and find out a \cf3\strike token type\cf4\strike0\{linkID=350\}\cf0\f0 . A\f1  \cf3\strike context\f0  \f1 mechanism\cf4\strike0\{linkID=360\}\cf0  useful for keeping track of assembler\f0  \f1 states.For example the context mechanism might be used with\f0  \f1 multiline macros to create a set of high level constructs such as\f0  \f1 'if-else-endif', 'do-while' and so forth.
\par 
\par }
180
Scribble180
Command Line




Writing



FALSE
73
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\fi-700\li700\cf1\b\fs32 Command Line\cf0\b0\f1\fs20 
\par 
\par The general format of an OAsm
\par command line is:
\par 
\par \f2 OAsm [options] file-list\f1 
\par 
\par where the\f0  \f1 file-list is an arbitrary list\f0  \f1 of input files.
\par 
\par For example:
\par 
\par \f2 OAsm one.asm two.asm three.asm\f1 
\par 
\par assembles several\f0  \f1 files, and makes output files called \f0 ONE.O\f1 , \f0 TWO.O\f1  and\f0  THREE.O\f1 .
\par 
\par The file list can have wildcards:
\par \f2 
\par OAsm *.asm\f1 
\par 
\par assembles all the files in the curent directory.
\par 
\par 
\par \f0 Res\f1 ponse files\f0  \f1 can be used as an alternate to specifying input on the command\f0  \f1 line. For example:
\par 
\par \f2 OAsm @myresp.lst\f1 
\par 
\par will take command line options from myresp.lst\f0 .  \f1 Response files aren't particularly useful with the assembler, but they\f0  \f1 are supported.
\par 
\par \f0 Following is a list of the command line switches OAsm supports.\f1 
\par 
\par \b\f0 -i\b0  case insensity\f1 
\par 
\par \f0\tab\f1 if \f0 this \f1 switch is specified on the command line, \cf2\strike labels\cf3\strike0\{linkID=280\}\cf0  will be treated as case\f0  \f1 insensitive. This can allow easier linkage between modules which\f0  \f1 use different case. Note that case insensitivity only extends to\f0  \f1 labels; preprocessor symbols are always case-sensitive in OAsm.
\par 
\par \b\f0 -e\b0  preprocess only
\par 
\par \tab if this \f1 switch is specified on the assembler command line,\f0  \f1 OAsm will act as a\f0  \f1 preprocessor and not perform the actual assembly phase.\f0   \f1 In preprocessor mode, the full functionality of the preprocessor is\f0  \f1 available, when it does not rely on information that would only be\f0   \f1 available while assemblying the file. Specifically, the\f0  \f1 preprocessor is fully functional, except that expressions that refer to\f0  \f1 labels or program counter locations will result in an error when\f0  \f1 used for example with the %assign\f0  \f1 preprocessor statement.
\par 
\par \b\f0 -oname\b0  specifying output file name\f1 
\par 
\par \f0\tab\f1 By default, OAsm will take the input file name,\f0  \f1 and replace the extension with the\f0  \f1 extension .o.\f0   \f1 However in some cases it is useful to be able to specify\f0  \f1 the output\f0  \f1 file name\f0  with this switch\f1 . The specified name can have its extension specified,\f0  \f1 or it\f0  \f1 can be typed without an extension to allow OAsm to add the .o\f0  \f1 extension. \f0  \f1 OAsm is only\f0  \f1 capable of producing object files.\f0   For example\f1 
\par 
\par \f0\tab\f2 OAsm /omyfile test.asm\f1 
\par 
\par \f0\tab\f1 makes an object file called \f0 MYFILE.O
\par 
\par \b -Ipath\b0\f1 
\par 
\par \f0\tab By default, \f1 OAsm will search for include\f0  \f1 paths in the current directory. If\f0  \f1 there are other paths OAsm\f0  \f1 should search\f0  for included files\f1 , \f0 this\f1  switch\f0  \f1 can be specified to have it search\f0  \f1 them.
\par \b 
\par \f0 -Ddefine = xxx\b0 
\par 
\par \tab\f1 If it is necessary to define a \f0 preprocessor definition\f1  on the command line, use \f0 this switch.  \f1 For example:
\par 
\par \f0\tab\f2 OAsm /DMYINT=4 test.asm
\par \f1 
\par \f0\tab\f1 defines the variable MYINT and gives it a value of 4. 
\par 
\par \f0\tab\f1 A \f0 preprocessor definition\f1  doesn't have to be defined with a value:
\par 
\par \f0\tab\f2 OAsm /DMSDOS test.asm\f0 
\par \f1 
\par \f0\tab\f1 might be used to specify preprocessing based on the program looking for\f0  \f1 the word MSDOS in #ifdef statements.
\par 
\par \b\f0 -lfile\b0  listing file\f1 
\par 
\par \f0\tab When this switch is specified, \f1 OAsm will\f0  \f1 produc\f0 e\f1  a\f0  \f1 listing file correlating the output byte stream with input lines of the\f0  \f1 file. Note the listing file is not available in preprocessor mode\f0  \f1 as no assembly is done. 
\par 
\par \f0\tab\f1 By default the listing file will not show macro expansions. To\f0  \f1 get a listing file where macros are shown in expanded form, \f0 add \f1 -ml. This will also\f0  \f1 expand preprocessor repeat statements.\f0   \f1 Note that there is a special qualifier used in a macro definition,\f0  \f1 .nolist, which can be used on a\f0  \f1 macro-by-macro basis. When used,\f0  \f1 it prevents macros from being expanded in the listing file even when\f0  -ml is used \f1 This is\f0  \f1 useful for example to prevent cluttering the\f0   \f1 the listing file with expansions of often-used macros or macros that\f0  \f1 are composed largely of preprocessor statements. In fact the\f0  \f1 builtin macros that map preprocessor directives to native form all use\f0  \f1 this qualifier.
\par \pard 
\par }
190
Scribble190
Directives




Writing



FALSE
33
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Directives\cf0\b0\f1\fs20 
\par 
\par Directives are used to indicate the assembler should interpret the\f0  \f1 statement as something other than an instruction to the\f0  \f1 processor. For example they can be used to define data, or to\f0  \f1 create seperate sections for grouping code and/or data. 
\par 
\par Natively, directives are always enclosed in brackets, for example to\f0  \f1 define a byte of data:
\par 
\par \f2 myvar [db 44]\f1 
\par 
\par However, the directives are redefined with default \cf2\strike multiline macros\cf3\strike0\{linkID=370\}\cf0 , so\f0  \f1 that the\f0  \f1 brackets are not necessary when typing code:
\par 
\par \f2 myvar db 44\f1 
\par 
\par This documentation describes the macro version of the directives.
\par 
\par Some of the\f0  \f1 macro redefinitions of the directives are simply a mapping from\f0  \f1 the non-bracket version to the bracketized version. Other macros\f0  \f1 are more complex, adding behavior.\f0  \f1 For example the macros for\f0  \f1 psuedo-structures define the structure size, and keep track of the\f0  \f1 current section and switch back to it when the end of structure macro\f0  \f1 is encountered.
\par 
\par There are several types of directives. 
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200\cf2\strike Data Definition \f0 Di\f1 rectives\cf3\strike0\{linkID=199\}\cf0\f0  \f1 define the value of a variable, either in terms of a numeric value or\f0  \f1 as a label (address) defined elsewhere in the assembler. 
\par \cf2\strike{\pntext\f3\'B7\tab}Data\f0  \f1 Reservation \f0 D\f1 irectives\cf3\strike0\{linkID=210\}\cf0  reserve space for data, and optionally\f0  \f1 initialize that space with default values. 
\par \cf2\strike{\pntext\f3\'B7\tab}Label Directives\cf3\strike0\{linkID=220\}\cf0\f0  \f1 give some attribute to a label, such as defining how it will be viewed\f0  \f1 from outside the module. 
\par \cf2\strike{\pntext\f3\'B7\tab}Section Directives\cf3\strike0\{linkID=230\}\cf0  group related code or data in such a\f0  \f1 way that it can be combined with other modules. 
\par {\pntext\f3\'B7\tab}The \cf2\strike EQU \f0 D\f1 irective\cf3\strike0\{linkID=240\}\cf0\f0  \f1 allows definition of a label in terms of a constant value. 
\par {\pntext\f3\'B7\tab}The\f0  \cf2\strike\f1 INCBIN \f0 D\f1 irective\cf3\strike0\{linkID=250\}\cf0  allows the possibility of directly importing binary\f0  \f1 data into the current section. 
\par {\pntext\f3\'B7\tab}The\cf2\strike  TIMES \f0 D\f1 irective\cf3\strike0\{linkID=260\}\cf0  allows a very\f0  \f1 simple form of repetitive code generation. 
\par \cf2\strike{\pntext\f3\'B7\tab}Psuedo-structure\f0  D\f1 irectives\cf3\strike0\{linkID=270\}\cf0  allow you to define structured data in a very basic way.
\par \pard 
\par 
\par 
\par 
\par }
199
Scribble199
Data Definition Directives




Writing



FALSE
109
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil Arial;}{\f1\fnil\fcharset0 Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\f0\fs32 Data Definition Directives\cf0\b0\fs20 
\par 
\par Data Definition\f1  \f0 directives define the value of a variable. There are several\f1  \f0 types of values that can be defined. 
\par 
\par The first is a number.\f1   \f0 The number can sometimes be an integer, other times floating point,\f1  \f0 depending on the directive. Another type is a character or\f1  \f0 string. \f1  \f0 Another type of value is use of a label, or the\f1  \f0 difference\f1  \f0 between labels. Another type sometimes useful in real mode\f1  \f0 programming is a segment, which is very loosely a reference to a\f1  \f0 section. The\f1  r\f0 eference can be made either by saying the section\f1  \f0 name, or using the SEG operator on a label to extract its section.
\par 
\par There are five Data Definition Directives:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 db - define byte
\par \f1{\pntext\f3\'B7\tab}d\f0 w - define word
\par {\pntext\f3\'B7\tab}dd - define dword
\par {\pntext\f3\'B7\tab}dq - define quadword
\par {\pntext\f3\'B7\tab}dt - define tbyt\f1 e\f0 
\par \pard 
\par 
\par Table 1 cross-references the various directives along with the data\f1  \f0 types can be defined with each.
\par \cf2 
\par \{html=\cf0 <\f1 center>
\par <\f0 table border="1"\f1  cellpadding="2" cellspacing="2"\f0 >
\par <tbody>\cf2\}
\par \f1\{html=
\par \cf0\f0     \f1 <tr>
\par       \f0 <td style="vertical-align: top; font-weight: bold;"></td>
\par       <td style="vertical-align: top; font-weight: bold;">integer</td>
\par       <td style="vertical-align: top; font-weight: bold;">floating point</td>
\par       <td style="vertical-align: top; font-weight: bold;">character/string</td>
\par       <td style="vertical-align: top; font-weight: bold;">label</td>
\par       <td style="vertical-align: top; font-weight: bold;">segment\f1 <\f0 /td>
\par     </tr>
\par \cf2\f1\}\f0 
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top; font-weight: bold;">db</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">no</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par \f1   \f0     <td style="vertical-align: top; font-weight: bold;">dw</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">yes\f1 <\f0 /td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top; font-weight: bold;">dd</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">no</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top; font-weight: bold;">dq</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">no</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top; font-weight: bold;">dt</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">yes</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">no</td>
\par       <td style="vertical-align: top;">no</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0   </tbody>
\par </table>
\par \cf2\}
\par \cf0 Table 1 - directives\f1  \f0 and data types
\par \cf2\{html=\cf0\f1 </center>\cf2\}
\par \f0 
\par \cf0 In 16-bit mode, it makes sense to use dw with labels, whereas in 32-bit\f1  \f0 mode, it makes sense to use dd with labels.
\par 
\par Some examples follow:
\par 
\par \f2 mylab:
\par  db 44
\par  dw 0234h
\par  dd 9999
\par  dd 43.72
\par  dd mylab
\par  dq 19.21e17
\par  dt 0.001
\par \f0 
\par Multiple values\f1  \f0 may be specified per line for these directives; for example:
\par 
\par \f2 mylab:
\par   db "hello world",13, 10, 0
\par   dq 44.7,33,2.19.8
\par 
\par }
210
Scribble210
Data Reservation Directives




Writing



FALSE
34
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil Arial;}{\f1\fnil\fcharset0 Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\f0\fs32 Data Reservation Directives\cf0\b0\fs20 
\par 
\par 
\par 
\par Data\f1  \f0 Reservation directives reserve space for data, and optionally give an\f1  \f0 initial value to load the space with. For example:
\par 
\par \f2 resb 64\f0 
\par 
\par reserves space for 64 bytes of data. This data will default to\f1  \f0 zeros in the section it is defined it. However, an alternative\f1  \f0 form of the\f1  \f0 directive specifies an initial value. Thus:\f1\\\f0 
\par 
\par \f2 resb 64,'a'\f0 
\par 
\par fills the space with lower case 'a' values.
\par 
\par There are five data reservation directives:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 resb - reserve bytes  
\par {\pntext\f3\'B7\tab}resw - reserve words
\par {\pntext\f3\'B7\tab}resd - reserve dwords
\par {\pntext\f3\'B7\tab}resq - reserve quadwords
\par {\pntext\f3\'B7\tab}rest - reserve tbyte\f1 s\f0 
\par \pard 
\par As an example:
\par 
\par \f2 mylab db "hello world"
\par   resb mylab + 80 - $, '.'
\par 
\par \f0 defines the string "hello world", then adds enough dots on the end to\f1  \f0 make up an 80-character buffer.
\par 
\par Generally, the type of data that can be defined in the optional\f1  \f0 argument to one of the Data Reservation directives is the same as for\f1  \f0 the corresponding Data Definition directive.
\par 
\par }
220
Scribble220
Label Directives




Writing



FALSE
101
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil Arial;}{\f1\fnil\fcharset0 Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\f0\fs32 Label Directives\cf0\b0\fs20 
\par 
\par 
\par Label\f1  \f0 Directives bestow some type of attribute to a label. Generally\f1  \f0 these attributes center around visibility of the label - is it visible\f1  \f0 to some entity outside the current OAsm\f1  a\f0 ssembly session or not?\f1   
\par 
\par \f0 For example the 'global' and 'extern' directives bestow attributes that\f1  \f0 allow the linker to resolve references to a label when the references\f1  \f0 are made in one source code file but the\f1  \f0 label is defined in a\f1  \f0 different source code file. Some of these directives require the\f1  \f0 label to be defined in the same file as the directive occurs; and\f1  \f0 others require the label to be defined elsewhere.
\par 
\par The Label Directives are as follows:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 global - the label is defined in this file, but the linker may\f1  \f0 use it to resolve references in other files
\par {\pntext\f3\'B7\tab}extern - the label is not defined in this file, the linker should\f1  \f0 find it elsewhere
\par {\pntext\f3\'B7\tab}export - the label is defined in this file, and will become a DLL\f1  \f0 or EXE export that Windows can use to resolve against other executables
\par {\pntext\f3\'B7\tab}import - the label is not defined in this file, it will be\f1  \f0 imported from some other DLL or EXE file by windows
\par \pard   
\par 
\par Each use of theglobal directive can assign the 'global' attribute to one or more\f1  \f0 labels. When a label is global, it has a presence that extends\f1  \f0 beyond the current file, and the linker may resolve references to the\f1  \f0 variable to it, when those references are made in other files.\f1  \f0 Those references would typically have been made by use of the 'extern'\f1  \f0 directive. For example:
\par 
\par 
\par \f2 global puterror
\par puterror:
\par   mov eax,errmsg
\par   call strput
\par   ret
\par errmsg:
\par   db "this is an error",0
\par strput:
\par    ....
\par    ret
\par 
\par \f0 creates a\f1  \f0 function 'puterror' which is visible to other files during\f1  \f0 linkage. Global may be used with multiple labels:
\par 
\par 
\par \f2 global mylab, strput
\par \f0 
\par 
\par 
\par Each use of the\f1  \f0 extern directive can assign the 'external' attribute to one or more\f1  \f0 labels. When a label is external, the definition is not found in\f1  \f0 the file currently being assembled. The purpose of the directive\f1  \f0 is to give the assembler and linker a hint that it should search\f1  \f0 elsewhere for the\f1  \f0 definition.
\par 
\par In the above example, if 'strput' was defined in a different file from\f1  \f0 the definition of puterror you might write the following:
\par 
\par \f2 global puterror
\par extern strput
\par puterror:
\par   mov eax,errmsg
\par   call strput
\par   ret
\par errmsg:
\par   db "this is an error",0
\par 
\par \f0 As with the\f1  \f0 global directive, extern can be used with multiple symbols:
\par 
\par \f2 global puterror
\par extern strput, numberput
\par puterror:
\par   push eax
\par   mov eax,errmsg
\par   call strput
\par   pop eax
\par   call numberput
\par   ret
\par  
\par errmsg db "this is error number: ",0
\par 
\par 
\par \f0 The export directive\f1  \f0 defines a symbol that can be used by the windows operating system\f1  \f0 during program load. For example a DLL might use it to declare a\f1  \f0 function that is to be available to other executable files.\f1   \f0 Unlike the global and external directives, 'export' can only be used to\f1  \f0 change the attributes of one variable at a time. For example in\f1  \f0 the above examples adding the line:
\par \f2 
\par export puterror\f1 
\par \f0 
\par would create an export symbol named 'puterror' which windows could then\f1  \f0 resolve to an import reference in another executable file at load\f1  \f0 time. Another form of the export directive can be used to change\f1  \f0 the visible name of the exported function:
\par 
\par \f2 export puterror Error\f0 
\par 
\par would export the function puterror, but other executables would see it\f1  \f0 as being named Error.
\par 
\par 
\par 
\par The import directive is used to signify\f1  \f0 that the label is exported from some other executable or DLL file, and\f1  \f0 that windows should load that executable or DLL so that the label can
\par be resolved. As with export there are two versions of the\f1  \f0 directive:
\par 
\par 
\par i\f2 mport ExitProcess Kernel32.dll
\par ...
\par  push 0
\par 
\par   call ExitProcess
\par ...
\par \f0 
\par indicates that the DLL kernel32.dll should be loaded so that a\f1  \f0 reference to the ExitProcess API call can be resolved. 
\par 
\par It might\f1  \f0 be useful to rename ExitProcess to Quit if that is asier to remember\f1  \f0 and type:
\par 
\par \f2 import ExitProcess Kernel32.dll Quit
\par ...
\par  push 0
\par  call Quit 
\par ...
\par 
\par \f0 
\par }
230
Scribble230
Section Directives




Writing



FALSE
73
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil Arial;}{\f1\fnil\fcharset0 Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\f0\fs32 Section Directives\cf0\b0\fs20 
\par 
\par Section directives help arrange data and code. In 16-bit code\f1  \f0 they are often essential, as a section in \f1 O\f0 Asm maps more or less directly to a\f1  \f0 segment in the real-mode architecture (depending on the exact\f1  \f0 definitions given in the linker specification file); and large programs\f1  \f0 simply won't\f1  \f0 fit within a single section. The section directive would then be\f1  \f0 used to partition the various code and data into segments some way that\f1  \f0 makes sense based on the application.
\par 
\par While sections aren't quite as necessary in 32-bit code, it is still\f1  \f0 customary to partition code into various sections to make it more\f1  \f0 manageable. In general, the linker will combine the code and data\f1  \f0 that is generated from assembling multiple source files so that similar\f1  \f0 types of code or data will appear close together in the output\f1  \f0 file. This is usually done based on section NAME, for example a\f1  \f0 section named CODE in one object file will be combined with sections\f1  \f0 named CODE in other object files, and similarly sections named DATA\f1  \f0 will be combined together, before the CODE and DATA sections are\f1  \f0 themselves combined together to make a part of the executable.\f1   \f0 Section names are generally arbitrary, however, there are some\f1  \f0 conventions made specifically in the linker specifications for windows\f1  \f0 programs.
\par 
\par Another use for section directives is to define absolute\f1  \f0 addresses. This is similar to using EQU to give a value to a\f1  \f0 label, but is more generic and allows use of Data Reservation\f1  \f0 directives so the assembler can calculate offsets based on the amount\f1  \f0 of data reserved. This use of section directives helps define the\f1  \f0 psuedo-struct mechanism.
\par 
\par The three section directives are:
\par 
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200\f1 s\f0 ection - define a name\f1 d\f0  section
\par {\pntext\f3\'B7\tab}absolute - start an absolute section
\par {\pntext\f3\'B7\tab}align - align code or data to a specific boundar\f1 y\f0 
\par \pard 
\par The Section\f1  \f0 directive switches to a new section. If this is the first time\f1  \f0 the section is used, the section directive may also specify attributes\f1  \f0 for the section. For example:
\par 
\par section code
\par 
\par switches to a section named code. Various attributes may be\f1  \f0 specified, such as section alignment and word size of section.\f1   \f0 Some other attributes are parsed for compatibility with x86 assemblers,\f1  \f0 but are not currently used. The attributes are:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 ALIGN=xxx\f1   \f0 - set alignment of section within EXE file
\par {\pntext\f3\'B7\tab}CLASS=xxx\f1  \f0 - set class name. This attribute is ignored by this assembler
\par {\pntext\f3\'B7\tab}STACK=xxx\f1  \f0 - this section is a stack section. This attribute is ignored by\f1  \f0 this assembler
\par {\pntext\f3\'B7\tab}USE16 -\f1  \f0 this section uses 16-bit addressing modes and data
\par {\pntext\f3\'B7\tab}USE32 -\f1  \f0 this section uses 32-bit addressing modes and data
\par \pard 
\par As an example of a simple 32-bit program
\par 
\par 
\par \f2 section code ALIGN=2 USE32
\par 
\par extern Print
\par ..start:
\par   mov eax,helloWorld
\par   call Print
\par   ret
\par section data ALIGN=4 USE32
\par helloWorld db "Hello World",0
\par 
\par \f0 Note that for\f1  \f0 convenience, 'segment' is defined as an alias for 'section'. So\f1  \f0 you could write:
\par 
\par \f2 segment code USE32
\par \f1 
\par \f0 to start a section named code, if you prefer.
\par 
\par The absolute\f1  \f0 directive is used to switch out of sections where data can be emitted,\f1   \f0 into an absolute section with\f1  \f0 the specified origin. It is called absolute because these labels\f1  \f0 never get relocated by the linker; they are essentially\f1  \f0 constants. Labels\f1  \f0 defined in an absolute section will get the value of the program\f1  \f0 counter\f1  \f0 within the section, at the time the section is defined. But these\f1  \f0 labels get a constant value, that is not relocated by the linker.\f1   \f0 For example:
\par 
\par \f2 absolute 0
\par lbl1:
\par   resb 4
\par lbl2:
\par   resw 3
\par lbl3:
\par 
\par \f0 creates an absolute section based at absolute zero, and defines three\f1  \f0 labels. These labels will have the following values based on the\f1  \f0 space that has been reserved:
\par 
\par l\f2 bl1: 0
\par lbl2: 4 * 1 = 4
\par lbl3: 4 + 3 * 2 = 10
\par \f0 
\par Note that in\f1  \f0 the definition of this section, we did not attempt to create data or\f1  \f0 code, we only reserved space. In general attempting to generate\f1  \f0 code or data in an absolute section will cause an error.
\par 
\par The Align directive\f1  \f0 aligns data to a specific boundary\f1  within a section\f0 . For example:
\par 
\par \f2 section data
\par \tab db 4
\par align 4
\par 
\par \f0 inserts enough zeroed bytes of data to align the current section to the\f1  \f0 beginning of a four-byte boundary. Note that the section\f1  \f0 attributes still need to be set to have the same\f1  \f0 alignment or better,\f1  \f0 either in the section directive or in the linker specification file, so\f1  \f0 that the linker will honor the alignment when relocating the section.
\par }
240
Scribble240
EQU Directive




Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil Arial;}{\f1\fnil\fcharset0 Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green255\blue0;}
\viewkind4\uc1\pard\cf1\b\f0\fs32 EQU Directive\cf0\b0\fs20 
\par 
\par The EQU\f1  \f0 directive allows a label to be given some value. This value must\f1  \f0 be calculable at the time the EQU directive is encountered and must\f1  \f0 resolve to a constant. However, the value itself can involve the\f1  \f0 differences between relative constructs, e.g. the difference between\f1  \f0 the current program counter and a label defined earlier in the section\f1  \f0 is a valid\f1  \f0 expression for EQU.
\par 
\par When the value is a difference between relative constructs, care must\f1  \f0 be taken that the branch optimization does not change the value after\f1  \f0 it has been calculated. For more information, see the section on\f1  \f0 expressions.
\par 
\par For example:
\par 
\par \f2 four  EQU 4   \cf2 ; value of the label 'four' is 4\cf0 
\par mylab resb 64
\par size EQU $-mylab ; \cf2 program counter - mylab = 64\cf0 
\par 
\par 
\par }
250
Scribble250
INCBIN Directive




Writing



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil Arial;}{\f1\fnil\fcharset0 Arial;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\f0\fs32 INCBIN Directive\cf0\b0\fs20 
\par 
\par The Incbin directive allows the import of a binary file into the\f1  \f0 current section. For example it could be used to copy a graphics\f1  \f0 resource such as a bitmap or font verbatim into the current\f1  \f0 section. Other uses include things like importing help text from\f1  \f0 a text file, or importing a table such as a CRC table that has been\f1  \f0 pre-generated by some other program.
\par 
\par The basic form of incbin is:
\par 
\par incbin\f1  \f0 "filename"
\par 
\par where filename is the name of the file to import. In this form,\f1  \f0 all data from the beginning to the end of the file will be\f1  \f0 imported. Another form starts importing at a specific offset and
\par goes to the end:
\par 
\par incbin\f1  \f0 "Filename", 10\f1 0\f0 
\par 
\par starts importing \f1 from\f0  the 100th byte in the file. Still another\f1  \f0 form lets you specify the length of data to import:
\par 
\par incbin\f1  \f0 "Filename", 96, 16
\par 
\par imports 16 bytes, starting at offset 96 within the file.
\par 
\par 
\par }
260
Scribble260
TIMES Directive




Writing



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil Arial;}{\f1\fnil\fcharset0 Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\f0\fs32 TIMES Directive\cf0\b0\fs20 
\par 
\par The Times\f1  \f0 directive is a primitive form of repetitive programming. It takes\f1  \f0 as operands an instruction or directive, and a count of the number of\f1  \f0 times to repeat the instruction or directive. As\f1  \f0 such its\f1  \f0 functionality can often be performed more efficiently with a Data\f1  \f0 Reservation directive. It is also much more limited than the %rep\f1  \f0 group of preprocessor directives. Times is available primarily\f1  \f0 for nasm compatibility.
\par 
\par For example the earlier example from the Data Reservation section could\f1  \f0 be alternatively written:
\par 
\par 
\par \f2 mylab db "hello world"
\par  times mylab + 80 - $ [db '.']
\par 
\par \f0 Here the native\f1  \f0 form of the db directive is used, since macro substitution is not\f1  \f0 available in this context. Times could also be used for timing:
\par 
\par \f2 times 4 NOP
\par 
\par \f0 another use for times sometimes found in NASM programs is to align data:
\par 
\par \f2 times ($$-$)%4 [db 0]
\par 
\par \f1 but in this assembler it is better to use the align directive.\f0 
\par 
\par }
270
Scribble270
Psuedo-structure Directives




Writing



FALSE
51
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil Arial;}{\f1\fnil\fcharset0 Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\f0\fs32 Psuedo-structure Directives\cf0\b0\fs20 
\par 
\par 
\par Structures\f1  \f0 aren't really a construct supported by the assembler, however, clever\f1  \f0 macro definitions allow definition of structure-like entities.\f1  \f0 For\f1  \f0 example, consider the following:
\par 
\par \f2 struc
\par astruc
\par s1 resb 4
\par s2 resw 3
\par s3 resd 5
\par s4 resb 1
\par endstruc
\par 
\par \f0 This defines the following label values similar to how they were\f1  \f0 defined in\f1  \f0 an absolute section:
\par 
\par s1: 0
\par s2: 4 * 1 = 4
\par s3: 4 + 3 * 2= 10
\par s4: 10 + 5 *4 = 30
\par 
\par The structure mechanism also defines astruc as 0, and it defines the\f1  \f0 size of the struct 'astruc_size' as 30 + 1 = 31.
\par 
\par To access the member of a structure one might use something like:
\par 
\par \f2 mov eax,[ebx + s3]
\par 
\par \f0 The structure mechanism could just as well be done with absolute\f1  \f0 sections or EQU statements (except that a side effect of the structure\f1  \f0 mechanism is to define a size). But a little more more\f1  \f0 interestingly, if you introduce local labels and\f1  \f0 remember that a local label can be accessed from anywhere if its fully\f1  \f0 qualified name is specified you might write:
\par 
\par \f2 struct astruc
\par .s1 resb 4
\par .s2 resw 3
\par .s3 resd 5
\par .s4 resb 1
\par endstruc
\par 
\par \f0 This lets you qualify the name and use:
\par 
\par \f2 mov eax,[ebx + astruc.s3]
\par 
\par \f0 However you need to be careful with this. Structures aren't\f1  \f0 really part of the assembler, but are instead an extension provided by\f1  \f0 built-in macros. So you can't make an instance of a structure and\f1  \f0 then use a period to qualify the instance name with a structure\f1  \f0 member. For example:
\par 
\par \f2 mystruc resb astruc_size,0
\par mov  eax,[mystruc.s3]
\par 
\par \f0 is not valid, because the label 'mystruc.s3' does not exist. The\f1  \f0 move would have to be changed to something like:
\par 
\par \f2 mov eax,[mystruc + astruc.s3]
\par \f0 
\par }
280
Scribble280
Labels




Writing



FALSE
60
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green255\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Labels\cf0\b0\f1\fs20 
\par 
\par Labels may\f0  \f1 begin with any alphabetic character, or with any of the characters '_',\f0  \f1 '?', or '.'. Within a label, alphabetic characters, digits, or\f0  \f1 any of the\f0  \f1 characters '_', '$', '#', '@', '~', '?', '.' may occur.
\par 
\par Labels may be followed by a ':' character, but this is optional.
\par 
\par There are various types of labels. A standard label begins with\f0  \f1 an alphabetic character, or '_', '?'. 
\par 
\par Additionally 'local' labels\f0  \f1 may be defined. Local labels always start with a '.'. Local\f0  \f1 labels may be reused, providing there is a standard label between\f0  \f1 uses. This allows you to use meaningless names like '.1' '.2' and\f0  \f1 so forth within a local context, instead of having to come up with
\par unique labels every time a label is required.
\par 
\par For example in the fragment
\par 
\par \f2 routine1:
\par   test ax,1
\par   jnz .exit ; 
\par   \cf2 ///do something\cf0 
\par .exit:
\par   ret
\par 
\par routine2:
\par   cmp bx,ax
\par 
\par   jc .exit
\par   \cf2 ; do something\cf0 
\par .exit:
\par clc
\par ret
\par 
\par \f1 .exit\f0  \f1 is defined twice, however, each definition follows a different standard\f0  \f1 label so the two definitions are actually different labels.
\par 
\par Internally, each use of a local label does have a unique name, made up\f0  \f1 by concatenating the most recent standard label name with the local\f0  \f1 label name. In the above example the internal names of the two\f0  \f1 labels are thus routine1.exit\f0  \f1 and routine2.exit.\f0   \f1 It is possible to branch to the fully qualified name from within\f0  \f1 another context.
\par 
\par The context for local labels changes each time a new standard label is\f0  \f1 defined. It is sometimes desirable to define a kind of label\f0  \f1 which is neither a standard label, that would change the local label\f0  \f1 context, or a local label, which is useful only within that\f0   \f1 context. This is done by prepending the label name with\f0  \f1 '..@'. For example in the below:
\par 
\par 
\par \f2 routine3:
\par   text ax,1
\par 
\par   jnz .exit
\par   ..@go3:
\par  ; do something
\par .exit:
\par ret
\par 
\par main:
\par   call ..@go3
\par   ret
\par 
\par \f1 the label ..@go3\f0  is accessible from anywhere, but it \f1 is not qualified by the local label context of routine 3, nor does it\f0  \f1 start a new local label context, so .exit is
\par still a local label within the context of routine3.
\par 
\par OAsm generates two forms \f0 of \f1 such\f0  \f1 labels \f0  - one \f1 within macro invocations, and\f0  \f1 within 'contexts' as shown in other sections. In these cases the\f0  \f1 label starts with '..@', has a sequence of digits, then has a '.' or\f0  \f1 '@' character followed by user-specified text. When using the\f0  \f1 nonlocal label\f0  \f1 format, these forms should be avoided to avoid clashing with\f0  \f1 assembler-generated labels.
\par 
\par 
\par OAsm defines\f0  \f1 one special label, '..start'. This label, if used, indicates that\f0  \f1 this particular position in the code space is the first code that\f0  \f1 should be executed when the program runs.\f0   Note that if two modules both declare this label and are linked together, the linker will throw an error.\f1 
\par 
\par }
290
Scribble290
Numbers and Expressions




Writing



FALSE
352
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Numbers and Expressions\cf0\b0\f1\fs20 
\par 
\par 
\par Integers may be specified in base 8, base 10, or base 16. The\f0  r\f1 ules for specifying integers follow the C language standard; e.g. if a\f0  \f1 number starts with 0 it is assumed to be base 8; if it starts with 0x\f0   \f1 it is assumed to be base 16; otherwise it is base 10. For\f0  \f1 compatibility with other assemblers OAsm also supports the trailing 'h'\f0  \f1 to indicate hexadecimal values (base 16) but such numbers must start\f0  \f1 with a digit to prevent them from being interpreted as labels.
\par 
\par For example:
\par 
\par 012 ; octal for the decimal value 10
\par 12 ; the decimal value 12
\par 0x12 ; hexadecimal for the decimal value 18
\par 012h ; hexadecimal for the decimal value 18
\par 
\par Floating point values are specified similarly as to a C compiler.
\par 
\par For example:
\par 
\par 1.03
\par 17.93e27
\par 10000.4e-27
\par 
\par Note that floating point values must start with a digit in \f0  \f1 OAsm.\f0  \f1 .003 is not a valid floating point value because character sequences\f0  \f1 starting
\par with a dot are interpreted as local labels.\f0   Use 0.003 instead.\f1 
\par 
\par 
\par OAsm makes no real distinction\f0  \f1 between single characters and sequences\f0  \f1 of characters. Single quotes (') and double quotes (") may\f0  \f1 be used interchangeably. But the interpretation of characters and\f0  \f1 strings depends on context.
\par 
\par When used in an instruction:
\par 
\par \f2 mov ax,"TO"\f1 
\par 
\par The operand\f0  \f1 will be constructed in such a way that storing it to memory will result\f0  \f1 in the characters being stored in the same order they were typed.
\par In other words, the sequence:
\par 
\par \f2 mov ax,"TO"
\par mov [label],ax
\par 
\par \f1 will result in the value at label being the same as if the assembler\f0  \f1 directive db were used to initialize the value:
\par 
\par \f2 label db "TO"\f1 
\par 
\par Characters at the end of a string that cannot be encoded in the\f0  \f1 instruction will be lost, thus:
\par 
\par \f2 mov ax,"hi roger"\f1 
\par 
\par is the same as:
\par \f2 
\par mov ax,"hi"\f1 
\par 
\par because the register ax only holds the equivalent of two\f0  c\f1 haracters.
\par 
\par On the other hand, data areas may be initialized with strings with\f0  va\f1 rious directives. There are three types of values that can be\f0  \f1 initialized this way; bytes (1byte), words(2 bytes), and double-words(4\f0  \f1 bytes). For ASCII characters, the encoding is just the\f0  \f1 character, with enough leading zeros to pad to the appropriate size.
\par 
\par 
\par The symbol '$',\f0  \f1 by itself, means the current program counter. This is an absolute\f0  \f1 program counter, and if passed through to the linker will result in an\f0  \f1 absolute offset being compiled into the program. But sometimes it\f0  \f1 doesn't need to be used as an absolute value, for example it can be\f0  \f1 used to find the length of a section of data:
\par 
\par \f2 mylabel db "hello world",10,13
\par hellosize EQU $-mylabel
\par 
\par \f1 where the EQU\f0  \f1 statement assigns the value of the expression '$-mylabel' to the label\f0  hellosize.  
\par \f1 
\par The symbol '$$' means the beginning of the current section. For\f0  \f1 example the expression $-$$ gives the offset into the current section.
\par 
\par \f0 A more complex expression may u\f1 sually \f0 be used \f1 wherever a number may be specified, consisting perhaps of numbers, labels, special symbols, and operato\f0 rs\f1 . OAsm\f0  \f1 uses operators similar to the ones found\f0  \f1 in a C compiler, with precedence similar to the C compiler, and\f0  \f1 introduces some new operators as well. See table 1 for a listing\f0  \f1 of operators, and table 2 for a listing of operator precedences.
\par \cf2 
\par \{html=\cf0\f0 <center>\b\f1 
\par \b0\f0 <\f1 table border="1"\f0  cellpadding="2" cellspacing="2"\f1 >
\par   <tbody>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">( )</td>
\par       <td style="vertical-align: top;">specify evaluation order ofsub-expressions</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">SEG</td>
\par       <td style="vertical-align: top;">refers to segment of a variable\f0  \f1 (16-bit only)</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">-</td>
\par       <td style="vertical-align: top;">unary minus</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">+</td>
\par       <td style="vertical-align: top;">u\f0 na\f1 ry plus</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">~</td>
\par       <td style="vertical-align: top;">bitwise complement\f0 <\f1 /td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">!</td>
\par       <td style="vertical-align: top;">logical not</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">*</td>
\par       <td style="vertical-align: top;">multiply</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">/</td>
\par       <td style="vertical-align: top;">divide, unsigned</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">/-</td>
\par       <td style="vertical-align: top;">divide, signed</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">%</td>
\par       <td style="vertical-align: top;">modulous, unsigned</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">%-</td>
\par       <td style="vertical-align: top;">modulous, signed</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">+</td>
\par       <td style="vertical-align: top;">addition</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">-</td>
\par       <td style="vertical-align: top;">subtraction</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">WRT</td>
\par       <td style="vertical-align: top;">offset of a variable, from a\f0  \f1 specific segment</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">\f0 &gt;&gt;\f1 </td>
\par       <td style="vertical-align: top;">unsigned shift right</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">\f0 &lt;&lt;\f1 </td>
\par       <td style="vertical-align: top;">unsigned shift left</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">\f0 &gt\f1 </td>
\par       <td style="vertical-align: top;">greater than</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">&\f0 g\f1 t;=</td>
\par       <td style="vertical-align: top;">greater than or equal</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">\f0 &lt;\f1 </td>
\par       <td style="vertical-align: top;">less than
\par </td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">\f0 &lt;\f1 =</td>
\par       <td style="vertical-align: top;">less than or equal</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">==</td>
\par       <td style="vertical-align: top;">equals</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">!=</td>
\par       <td style="vertical-align: top;">not equal to</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">&amp;</td>
\par       <td style="vertical-align: top;">binary and</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">^</td>
\par       <td style="vertical-align: top;">binary exclusive or</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">|</td>
\par       <td style="vertical-align: top;">binary or</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">&amp;&amp;</td>
\par       <td style="vertical-align: top;">logical and</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">||</td>
\par       <td style="vertical-align: top;">logical or</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0   </tbody>
\par </table>
\par \cf2\}
\par \cf0 Table 1, Operator meanings
\par \cf2 
\par \{html=
\par \cf0\f0 <\f1 table border="1"\f0  cellpadding="2" cellspacing="2"\f1 >
\par   <tbody>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">( )</td>
\par       <td style="vertical-align: top;">parenthesis</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">SEG, -, +, ~, !</td>
\par       <td style="vertical-align: top;">unary operators</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">*, /, /-, %, %-</td>
\par       <td style="vertical-align: top;">multiplicative operators</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">+, -, WRT</td>
\par       <td style="vertical-align: top;">additive operators</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">\f0 &lt;&lt,&gt;&gt;<\f1 /td>
\par       <td style="vertical-align: top;">shift operators</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">\f0 &gtl\f1 , \f0 &gt;\f1 =, \f0 &lt;\f1 , \f0 &lt;\f1 =</td>
\par       <td style="vertical-align: top;">inequality operators</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">==, !=</td>
\par       <td style="vertical-align: top;">equality operators</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">&amp;</td>
\par       <td style="vertical-align: top;">bitwise and</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">^</td>
\par       <td style="vertical-align: top;">bitwise exclusive or</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">|</td>
\par       <td style="vertical-align: top;">bitwise or</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">&amp;&amp;</td>
\par       <td style="vertical-align: top;">logical and</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">||</td>
\par       <td style="vertical-align: top;">logical or</td>
\par     </tr>
\par \cf2\}
\par \{html=
\par \cf0   </tbody>
\par </table>
\par \cf2\}
\par \cf0 Table 2, Operator precedence from\f0  \f1 highest to lowest
\par 
\par \cf2 
\par \{html=\cf0\f0 </center>\cf2\f1 
\par \}
\par 
\par \cf0 Expressions involving labels or segments\f0  \f1 will often be pushed off to the linker for evaluation, however, the\f0  \f1 linker only knows simple math such as +-*/ and SEG. Sometimes\f0  \f1 however, an expression such as '$-mylab' can be directly evaluated by\f0  \f1 the assembler, provided mylab is defined earlier in the current\f0  \f1 segment. Such evaluations would result in a constant being passed
\par to the linker.
\par 
\par Note that OAsm mimics a\f0  \f1 multipass assembler, and will attempt to\f0  \f1 optimize branches to the smallest available form. This is\f0  \f1 normally not a problem as after each optimization pass OAsm will\f0  \f1 reevaluate expressions found in the code or data. However, some\f0  \f1 assembler directives such as EQU and TIMES evaluate their operands\f0  \f1 immediately, when the instruction is encountered. And all\f0  \f1 branches start out at the largest possible size. That means that\f0  \f1 a sequence like:
\par 
\par \f2 section code
\par USE32
\par label:
\par   cmp eax,1
\par   jz forward
\par   inc eax
\par forward:
\par size EQU forward - label
\par 
\par \f1 will result in 'size' being evaluated with the jz being a 6-byte\f0  \f1 instruction, but the final code will have the jz being a two-byte\f0  \f1 instruction. This disparity between the calculated value and the
\par actual value can introduce subtle bugs in a program. To get\f0  \f1 around this explicitly clarify any jumps in a region that is going to\f0  \f1 be\f0  \f1 sized with 'short' or 'near':
\par 
\par \f2 label:
\par   cmp eax,1
\par   jz short forward
\par 
\par   inc eax
\par forward:
\par 
\par \f1 Data directives\f0  \f1 aren't subject to size optimizations, so in the usual case of taking\f0  \f1 the size of a data region this isn't an issue.
\par 
\par }
300
Scribble300
Preprocessor Directives




Writing



FALSE
414
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Preprocessor Directives\cf0\b0\f1\fs20 
\par 
\par The OAsm\f0  \f1 preprocessor is a C preprocessor, with extensions. The\f0  \f1 preprocessor can be considered to be a set of routines which go through\f0  \f1 the code before it is assembled, making certain types of textual\f0  \f1 transformations to the source code. The transformations range\f0  \f1 from simple substitution of text or lines of text when a keyword is\f0  \f1 encountered, to inclusion of text from another file, to selectively\f0  \f1 ignoring some lines based on compile-time settings. The main\f0  \f1 difference from a C preprocessor, other than the extensions, is that\f0  \f1 instead of starting a preprocessor directive with a hash ('#')\f0  \f1 preprocessor directives are started with a percent ('%').
\par 
\par Many of these directives involve \cf2\strike Conditional Processing\cf3\strike0\{linkID=330\}\cf0 , which\f0  \f1 is a way\f0  \f1 to selectively choose what lines of code to assemble and what lines to\f0  \f1 ignore based on previous declarations.
\par 
\par Table 1 shows the \cf2\strike C-style\f0  \f1 preprocessor directives\cf3\strike0\{linkID=310\}\cf0 . These directives are compatible with\f0  \f1 similar directives in a C Compiler's preprocessor.
\par 
\par \cf3\{html=\f0  \cf0 <center>\f1 
\par \f0 <\f1 table border="1"\f0  cellpadding="2" cellspacing="2"\f1 >
\par   <tbody>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%define</td>
\par       <td style="vertical-align: top;">define a constant or\f0  \f1 function-style macro</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%undef</td>
\par       <td style="vertical-align: top;">undefine a macro</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%error</td>
\par       <td style="vertical-align: top;">display an error</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%line</td>
\par       <td style="vertical-align: top;">set the file and line displayed\f0  \f1 in error messages</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%include</td>
\par       <td style="vertical-align: top;">include another file</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%if</td>
\par       <td style="vertical-align: top;">conditional which tests fornonzero expression</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elif</td>
\par       <td style="vertical-align: top;">else-style conditional which\f0  \f1 tests for nonzero-expression</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifdef</td>
\par       <td style="vertical-align: top;">condit\f0 i\f1 onal which tests to see\f0  \f1 if a macro has been %defined</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifndef</td>
\par       <td style="vertical-align: top;">conditional which tests to see\f0  \f1 if a macro has not been %defined</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%else</td>
\par       <td style="vertical-align: top;">else clause for conditionals</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%endif</td>
\par       <td style="vertical-align: top;">end of conditional</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par   </tbody>
\par </table>
\par \cf3\}
\par \cf0 Table 1 - C lanuage style preprocessor\f0  \f1 directives
\par \cf3\{html=\cf0\f0 </center>\cf3 
\par \}\cf0\f1 
\par 
\par Table\f0  \f1 2 shows \cf2\strike basic\f0  \f1 extensions to the C preprocessor\cf3\strike0\{linkID=320\}\cf0  that are similar to\f0  \f1 directives already found in the preprocessor. This includes a new\f0  \f1 directive %assign which is\f0  \f1 like %define except it\f0  \f1 evaluates the macro\f0  \f1 value \f0 based \f1 on the assumption it is a numeric expression. It also\f0  \f1 includes case insensitive\f0  \f1 macro definition directives, and the beginning of an extensive set of\f0  \f1 extensions to condtionals that are similar to the %elif\f0   \f1 mechanism.
\par \cf3 
\par \{html=\cf0\f0 <center>\f1 
\par \f0 <\f1 table border="1"\f0  cellpadding="2" cellspacing="2"\f1 >
\par   <tbody>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td>%assign</td>
\par       <td style="vertical-align: top;">Like %define, but evaluates an\f0  \f1 expression and sets the value to the result</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%iassign </td>
\par       <td style="vertical-align: top;">%assign with a case-insensitive\f0  \f1 name</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%idefine</td>
\par       <td style="vertical-align: top;">%define with a case-insensitive\f0  \f1 name</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifdef</td>
\par       <td style="vertical-align: top;">else-style conditional which\f0  \f1 tests to see if a macro has been %defined</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifndef</td>
\par       <td style="vertical-align: top;">else-style conditional which\f0  \f1 tests to see if a macro has not been %defined</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par   </tbody>
\par </table>
\par \cf3\}
\par \cf0 Table 2 - Basic extensions to C style\f0  \f1 preprocessor
\par \cf3\{html=\cf0\f0 </center>\cf3 
\par \}\cf0\f1 
\par 
\par Table 3 shows extensions to the\f0  \f1 conditional mechanism that allow \cf2\strike text comparisons\cf3\strike0\{linkID=340\}\cf0 .\f0  \f1 There are both\f0  \f1 case sensitive and case insensitive forms of these directives.
\par 
\par 
\par \cf3\{html=\f0  \cf0 <center>\f1 
\par \f0 <\f1 table border="1"\f0  cellpadding="2" cellspacing="2"\f1 >
\par   <tbody>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifidn</td>
\par       <td style="vertical-align: top;">Case sensitive test for string\f0  \f1 matching</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifnidn</td>
\par       <td style="vertical-align: top;">Case sensitive test for string\f0  \f1 not matching</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifidn</td>
\par       <td style="vertical-align: top;">else-style case sensitive test\f0  \f1 for string matching</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifnidn</td>
\par       <td style="vertical-align: top;">else-style case sensitive test\f0  \f1 for string not matching</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifidni</td>
\par       <td style="vertical-align: top;">Case insensitive test for string\f0  \f1 matching</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifnidni</td>
\par       <td style="vertical-align: top;">Case insensitive test for string
\par not matching</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifndi</td>
\par       <td style="vertical-align: top;">else-style case insensitive test
\par for string matching</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifnidni</td>
\par       <td style="vertical-align: top;">else-style case insensitive test
\par for string not matching</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par   </tbody>
\par </table>
\par \cf3\}
\par \cf0 Table 3 - Text Comparison Conditionals
\par \cf3\{html=\cf0\f0 </center>\f1 
\par \cf3\}
\par 
\par \cf0 Table 4 shows various extensions to the conditional mechanism that\f0  \f1 allow \cf2\strike classification\f0  \f1 of a token's type\cf3\strike0\{linkID=350\}\cf0 . They can be used for\f0  \f1 example in \cf2\strike multiline\f0  \f1 macros\cf3\strike0\{linkID=370\}\cf0 , to allow a single macro to have different behaviors\f0  \f1 based on the type of a macro argument.
\par 
\par 
\par 
\par \cf3\{html=\cf0\f0 <center>\f1 
\par \f0 <\f1 table border="1"\f0  cellpadding="2" cellspacing="2"\f1 >
\par   <tbody>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td>%ifid</td>
\par       <td style="vertical-align: top;">test to see if argument is an
\par identifier</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifnid</td>
\par       <td style="vertical-align: top;">test to see if argument is not
\par an identifier</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifid</td>
\par       <td style="vertical-align: top;">else-style test to see if
\par argument is an identifier</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifnid</td>
\par       <td style="vertical-align: top;">else-style test to see if
\par argument is not an identifier</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifnum</td>
\par       <td style="vertical-align: top;">test to see if argument is a
\par number</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifnnum</td>
\par       <td style="vertical-align: top;">test to see if argument is not a
\par number</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifnum</td>
\par       <td style="vertical-align: top;">else-style test to see if
\par argument is a number</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifnnum</td>
\par       <td style="vertical-align: top;">else-style test to see if
\par argument is not a number</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifstr</td>
\par       <td style="vertical-align: top;">test to see if argument is a
\par string</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifnstr</td>
\par       <td style="vertical-align: top;">test to see if argument is not a
\par string</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifstr</td>
\par       <td style="vertical-align: top;">else-style test to see if
\par argument is a string</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifnstr</td>
\par       <td style="vertical-align: top;">else-style test to see if
\par argument is not a string</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par   </tbody>
\par </table>
\par \cf3\}
\par \cf0\f0 T\f1 able 4 - Token Type Classification\f0  \f1 Conditionals
\par \cf3\{html=\cf0\f0 </center>\f1 
\par \cf3\}\cf0 
\par 
\par Table 5 shows the \cf2\strike Multiline Macro Extensions\cf3\strike0\{linkID=370\}\cf0\f0  \f1 and the \cf2\strike Repeat Blo\f0 ck Extensions\cf3\strike0\f1\{linkID=380\}\f0  \cf0\f1 . These extentions include multiline macros, as\f0  \f1 well as\f0  \f1 a powerful facility for using the preprocessor to repeat sections of\f0  \f1 code or data.
\par 
\par 
\par \cf3\{html=\cf0\f0 <center>\f1 
\par \f0 <\f1 table border="1"\f0  cellpadding="2" cellspacing="2"\f1 >
\par   <tbody>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%macro</td>
\par       <td style="vertical-align: top;">start a multiline macro</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%imacro</td>
\par       <td style="vertical-align: top;">start a multiline macro, caseinsensitive name
\par </td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%endmacro </td>
\par       <td style="vertical-align: top;">end a multiline macro</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%rotate</td>
\par       <td style="vertical-align: top;">rotate arguments in a multiline\f0  \f1 macro</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%rep </td>
\par       <td style="vertical-align: top;">start a repeat block </td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%endrep </td>
\par       <td style="vertical-align: top;">end a repeat block </td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%exitrep</td>
\par       <td style="vertical-align: top;">exit a repeat block prematurely</td>
\par     </tr>
\par \cf3\}
\par \{html=\cf0 
\par   </tbody>
\par </table>
\par \cf3\}
\par \cf0 Table 5 - Multiline Macro and Repeat\f0  \f1 Block Extensions
\par \cf3\{html=\cf0\f0 </center>\cf3 
\par \}\cf0\f1 
\par 
\par Table 6 shows the \cf2\strike Context-Related Extensions\cf3\strike0\{linkID=360\}\cf0 .\f0   \f1 Preprocessor contexts are a powerful mechanism that\f0  \f1 can be used to 'remember' data between successive macro invocations,\f0  \f1 and for example could be used to construct a high-level representation\f0  \f1 of control constructs in the assembler.
\par 
\par \cf3\{html=\cf0\f0 <center>\f1 
\par \f0 <\f1 table border="1"\f0  cellpadding="2" cellspacing="2"\f1 >
\par   <tbody>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%push</td>
\par       <td style="vertical-align: top;">start a new context</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%pop</td>
\par       <td style="vertical-align: top;">end a new context</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%repl</td>
\par       <td style="vertical-align: top;">rename the context at the topeof the context stack
\par </td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifctx</td>
\par       <td style="vertical-align: top;">test to see if a context is in\f0  \f1 effect</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%ifnctx</td>
\par       <td style="vertical-align: top;">test to see if a context is not\f0  \f1 in effect</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifctx</td>
\par       <td style="vertical-align: top;">else-style test to see if a\f0  \f1 context is in effect</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">%elifnctx</td>
\par       <td style="vertical-align: top;">else-style\f0  \f1 test to see if a\f0  \f1 context is not in effect</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par   </tbody>
\par </table>
\par \cf3\f0\}\cf0\f1 
\par Table 6 - Context - Related Extensions
\par \cf3\{html=\cf0\f0 </center>\f1 
\par \cf3\f0\}\cf0\f1 
\par 
\par }
310
Scribble310
C-Style Preprocessor directives




Writing



FALSE
309
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\fi-720\li720\cf1\b\fs32 C-Style Preprocessor directives\cf0\b0\f1\fs20 
\par 
\par \pard The C-Style preprocessor directives are compatible with similar\f0  \f1 directives that are existant in preprocessors for C compilers.\f0   \f1 OAsm does not change the behavior of these directives other than to\f0  \f1 change the initial character which introduces the directive from '#' to\f0  \f1 '%'.
\par 
\par \pard\fi-720\li720\b %define\b0 
\par \f0\tab\f1 %define introduces a\f0  \f1 method to perform textual substitutions. In its simplest form it\f0  \f1 will just replace an identifier with some text:
\par 
\par \f2\tab %define HELLO_WORLD "Hello World"\f1 
\par 
\par \f0\tab\f1 replaces all instances of the identifier HELLO_WORLD with the indicated\f0  \f1 string. For example after this definition the following statement:
\par 
\par \f0\tab\f2 db HELLO_WORLD\f1 
\par 
\par \f0\tab\f1 will result in the string "Hello World" being compiled into the program.
\par 
\par \f0\tab\f1 %define is not limited to\f0  \f1 giving names to strings, it will do any sort\f0  \f1 of text substitution you want. That could include defining names\f0  \f1 for constants, or giving a name to an often-used instruction, for\f0  \f1 example.
\par 
\par \f0\tab\f1 In the below:
\par 
\par \f0\tab\f2 %define ARRAY_MAX 4
\par \tab mov eax,ARRAY_MAX
\par 
\par \f0\tab\f1 the text "4" gets substituted for ARRAY_MAX prior to assembling the mov\f0  \f1 instruction, so what the assembler sees is:
\par 
\par \f0\tab\f2 mov eax,4\f1 
\par 
\par \f0\tab\f1 Note that definitions are also processed for substitution:
\par 
\par \f0\tab\f2 %define ONE 1
\par \tab %define TWO (ONE + 1)
\par \tab %define THREE (TWO + 1)
\par \tab mov eax,THREE
\par \f0\tab 
\par \tab\f1 is substituted multiple times during processing, with the final result\f0  \f1 being:
\par 
\par \f0\tab\f2 mov eax,((1 +1) + 1)
\par \f1 
\par \f0\tab\f1 OAsm will detect recursive substitutions and halt the substitution\f0  \f1 process, so things like:
\par 
\par \f0\tab\f2 %define ONE TWO
\par \tab %define TWO\tab ONE
\par \f1 
\par \f0\tab\f1 will halt after detecting the recursion.
\par 
\par \f0\tab\f1 Also, the substitution text can be empty:
\par 
\par \f0\tab\f2 %define EMPTY
\par \tab mov eax, EMPTY
\par \f1 
\par \f0\tab\f1 results in the translated text:
\par 
\par \f0\tab\f2 mov eax,
\par \f1 
\par \f0\tab\f1 which cannot be assembled and will result in a syntax error during\f0  \f1 assembly.
\par 
\par 
\par \f0\tab\f1 %define can also be used in its\f0  \f1 functional form for more advanced text\f0  \f1 replacement activities. In this form, the identifier is\f0  \f1 parameterized. During substitutions, arguments are also\f0  \f1 specified; and the original parameters are replaced with the arguments\f0  \f1 while substitution is occurring. For example:
\par 
\par \f0\tab\f2 %define mul(a,b) a * b
\par \tab mov eax,mul(4,7)
\par \tab mov eax,4 * 7
\par \f1 
\par \f0\tab\f1 prior to assembly. It is usually not a good idea to write this\f0  \f1 quite the way it was done however. The user may elect to put any\f0  \f1 text they want in the invocation, so one thing that can happen is they\f0  \f1 write:
\par 
\par \f2\tab mov eax, mul(4+3, 7+2)
\par \f1 
\par \f0\tab\f1 This gets translated to:
\par 
\par \f0\tab\f2 mov eax, 4 + 3 * 7 + 2\f1 
\par 
\par \f0\tab\f1 which was probably not the intent. Below is what was probably\f0  \f1 desired:
\par 
\par \f0\tab\f2 mov eax, (4+3) * (7+2)\f1 
\par 
\par \f0\tab\f1 For this reason it is a good idea to fully parenthesize the parameters\f0  \f1 used in the original definition:
\par 
\par \f0\tab\f2 %define mul(a,b) ((a) * (b))
\par \tab mov eax, mul(4+3, 7+2)
\par \f0 
\par \tab becomes
\par 
\par \f2\tab mov eax, ((4 + 3) * (7 + 2))
\par 
\par \f0\tab\f1 The extra set of parenthesis is used to prevent similar situations from\f0  \f1 happening when 'mul' is used as a subexpression of another expression.
\par 
\par 
\par \f0\tab\f1 Note\f0  \f1 that when using %define,\f0  \f1 substituted text is not evaluated in any way, other than to process\f0  \f1 substitutions on the identifier and any specified\f0  \f1 parameters. So the move \f0 s\f1 tatement in the last example\f0  \f1 would be visible to the assembler exactly as the substitutions\f0  \f1 dictate, and the assembler has to do further evaluation of the\f0  \f1 expression if it wants a constant value.
\par 
\par \f0\tab\f1 Within a\f0  \f1 definition, there are a couple of special-case substitutions\f0  \f1 that can occur with function-style definitions. In Stringizing, a\f0  \f1 parameter can be turned into a string. For example if you write:
\par 
\par \f2\tab %define STRINGIZE(str) #str
\par \tab db STRINGIZE(Hello World)
\par 
\par \f0\tab\f1 quotes will be placed around the substituted parameter. So this\f0  \f1 translates to:
\par 
\par \f2\tab db "Hello World"
\par 
\par \f0\tab\f1 prior to assembly.
\par 
\par \f0\tab\f1 In Tokenizing, new identifiers may be produced. For example:
\par 
\par \f2\tab %define Tokenizing(prefix, postfix) (prefix ## postfix + 4)
\par \tab mov eax,Tokenizing(Hello,World)
\par 
\par \f0\tab\f1 would be translated to:
\par 
\par \f2\tab mov eax,HelloWorld + 4
\par \f0\tab 
\par \tab\f1 prior to assembly.
\par 
\par \f0\tab\f1 Note that even though the hash character used to start a preprocessor\f0  \f1 statement has been changed to '%', hash is still used in stringizing\f0  \f1 and tokenizing.
\par 
\par \f0\tab\f1 Finally, OAsm supports the C99\f0  \f1 extension to function-style definitions,\f0  \f1 which allows variable-length argument lists. For example:
\par 
\par \f2\tab %define mylist(first, ...) dw first, __VA_ARGS__
\par \f0\tab 
\par \tab\f1 where __VA_ARGS__ means append all remaining arguments that are\f0  \f1 specified, could be used like this:
\par 
\par \f0\tab\f2 mylist(1)
\par \tab mylist(1,2)
\par \tab mylist(1,2,3,4,5)
\par 
\par \f0\tab\f1 and so on. These would expand to:
\par 
\par \f2\tab dw 1
\par \tab dw 1,2
\par \tab dw 1,2,3,4,5
\par 
\par \f0\tab\f1 Note\f0  \f1 that the\f0  \f1 name of the identifier that is replaced is case-sensitive with %define,\f0  \f1 for example HELLO_WORLD is not the same as Hello_World. There is\f0  \f1 a\f0  \f1 case-insensitive form of this directive %idefine which can be used to\f0  \f1 make these and other related identifiers the same.
\par 
\par \f0\tab\f1 Note: OAsm does\f0  \f1 not support overloading function-style macros. 
\par 
\par \f0\tab\f1 For convenience\f0  \f1 OAsm allows %define\f0  \f1 definitions on the command line, which are useful\f0  \f1 for tailoring build behavior either directly or through the conditional\f0  \f1 processing facility.
\par 
\par \b %undef
\par 
\par \b0\f0\tab\f1 %undef undoes a\f0  \f1 previous definition, so that it will not be considered for further\f0  \f1 substitutions (unless defined again). For example:
\par 
\par \f0\tab\f1 %define\f0  \f1 REG_EBX 3
\par \f0\tab\f1 %undef REG_EBX
\par \f0\tab\f1 mov eax,\f0  \f1 REG_EBX
\par 
\par \f0\tab\f1 results in no substitution occurring for the use of REG_EBX.\f0   In this case this will result in an error.  \f1 
\par 
\par 
\par \b %error\b0 
\par \f0\tab\f1 %error displays an\f0  \f1 error, causing the assembler to not generate code. For example:
\par 
\par \f0\tab\f2 %error my new error\f1 
\par 
\par \f0\tab\f1 might display something like:
\par \b 
\par \f0\tab\f1 Error\f0  \f1 errdemo.asm(1): Error Directive: my new error
\par \b0 
\par \f0\tab\f1 When the file\f0  \f1 is assembled.
\par 
\par \b %line\b0 
\par \f0\tab\f1 %line is used to\f0  \f1 change the file and line number listed in the error reporting. By\f0  \f1 default the\f0  \f1 error reporting indicates the file and line an error occur on.\f0   \f1 Sometimes in generated source code files, it is useful to refer to the\f0  \f1 line number in the original source code rather than in the file that is\f0  \f1 currently being assembled. %line\f0  a\f1 ccomplishes this by updating\f0  \f1 internal tables to indicate to the preprocessor that it should use\f0  \f1 alternate information when reporting an error. For example\f0  \f1 inserting the following at line 443 of test.asm:
\par 
\par \f2\tab mov eax,^4
\par \f1 
\par \f0\tab\f1 produces a\f0  \f1 syntax error when the code is assembled:
\par 
\par \b\f0\tab\f1 Error\f0  \f1 test.asm(443): Syntax Error
\par \f0\tab 
\par \b0\tab\f1 If an\f0  \f1 additional %line directive is inserted:
\par \f0\tab 
\par \f2\tab %line 10, "demo.c"
\par \tab mov eax,^4
\par 
\par \f0\tab\f1 the error\f0  \f1 changes to:
\par 
\par \b\f0\tab\f1 Error\f0  \f1 demo.c(10): Syntax Error
\par 
\par \b0\f0\tab\f1 Note that once %line is used\f0  \f1 to change the line number and file name, OAsm remembers the new information\f0  \f1 and continues to increment the new line number each time it\f0  \f1 processes a\f0  \f1 line of source code.
\par 
\par 
\par %include
\par 
\par \f0\tab\f1 %include is used to\f0  \f1 start the interpretation of another source file. The current\f0  \f1 source file is suspended, and the new source file is loaded and\f0  \f1 assembled. Once the assembler is done with the new source file\f0  \f1 (and anything it also %includes)\f0   \f1 assembly resumes beginning where it\f0  \f1 left off in the current source file.
\par 
\par \f0\tab\f1 This facility is useful for example to hold preprocessor constants and\f0  \f1 structures that are shared between multiple source files. But the\f0  \f1 included file can include any valid assembler statement, including\f0  \f1 GLOBAL and EXTERN definitions. 
\par 
\par \f0\tab\f1 For example if test.asm is\f0  \f1 being assembled and the statement:
\par 
\par \f0\tab\f1 %include\f0  \f1 "test1.asm"
\par 
\par \f0\tab\f1 is encountered, the assembly of test.asm\f0  \f1 will temporarily be suspended\f0  \f1 while OAsm goes off to\f0  \f1 assemble test1.asm.
\par 
\par \f0\tab\f1 After it is done with\f0  \f1 test1.asm, \f0  \f1 OAsm remembers that it was\f0  \f1 previously assembling test.asm\f0  \f1 and\f0  \f1 picks up in that file where it left off (e.g. at the line after the\f0  \f1 %include statement).
\par 
\par \f0\tab\f1 This is not quite the same as specifying both\f0  \f1 test.asm and test1.asm on the command line.\f0   \f1 In the current example there is only one output file which contains the\f0  \f1 contents of both test.asm and test1.asm, where as if both were\f0  \f1 specified on the command line they would result in separate output\f0  \f1 files.\f0   Additionally, doing it this way can allow the two files to depend on one another in a way that couldn't happen if they were compiled separately.\f1 
\par 
\par \f0\tab\f1 For convenience, an include path may be specified on the command line,\f0  \f1 and OAsm will search for\f0  \f1 included files both in the current directory,\f0  \f1 and in directories specified on that path.
\par 
\par \b %if\b0 
\par 
\par \f0\tab\f1 %if is a %if-style\f0  \f1 conditional that takes a numeric expression as an\f0  \f1 argument. If\f0  \f1 the numeric expression evaluates to a non-zero value, the result of the\f0  \f1 evaluation will be true, otherwise it will be false.  Note that\f0  \f1 for purposes of this conditional, expressions are always evaluated; if\f0  \f1 an undefined identifier is used in a %if\f0  \f1 expression it is replaced with '0' and \f0  \f1 valuation continues. \f0  \f1 Subsequent blocks of code will either be\f0  \f1 assembled if the result of the evaluation is non-zero, or ignored if\f0  \f1 the result of the evaluation is zero.
\par 
\par \f0\tab\f1 For example:
\par 
\par \f2\tab %define COLOR 3
\par \tab %if COLOR == 3
\par 
\par \tab mov eax,4
\par \tab %endif
\par \f1 
\par \f0\tab\f1 will result in the mov statement being assembled because the result of\f0  \f1 the argument evaluation is a nonzero value.
\par 
\par \f0\tab\f1 On the other hand:
\par 
\par \f2\tab %define ZERO 0
\par \tab %if ZERO
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 results in nothing being assembled because the value of 'ZERO' is zero.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style\f0  \f1 conditionals.
\par 
\par 
\par %elif
\par \f0\tab\f1 %elif\f0  \f1 is a %elif-style conditional\f0  \f1 that takes a numeric expression as an\f0  \f1 argument. If the numeric expression evaluates to a non-zero\f0  \f1 value, the next block will be assembled,\f0  \f1 otherwise it will be\f0  \f1 ignored. As with %if,\f0  \f1 undefined symbols will be\f0  \f1 replaced with '0' for purposes of the evaulation.
\par 
\par \f0\tab\f1 For example:
\par 
\par \f2\tab %define COLOR 3
\par \tab %if COLOR == 2
\par 
\par \tab mov eax,4
\par \tab %elif COLOR == 3
\par \tab inc eax
\par \tab %endif
\par 
\par \f0\tab\f1 will result in the mov statement being ignored and the inc statement\f0  \f1 being a\f0 s\f1 sembled because the result of the %if argument evaluation is\f0  \f1 zero, and the result of the %elif argument\f0  \f1 evaluation is nonzero.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %elif-style conditionals.
\par 
\par 
\par 
\par \b %ifdef\b0 
\par \f0\tab\f1 %ifdef\f0  \f1 is a %if-style conditional\f0  \f1 that takes an identifer as an\f0  \f1 argument. If the identifier has been defined with a previous\f0  \f1 %define or\f0  \f1 %assign statement, the next\f0  \f1 block will be \f0  a\f1 ssembled, otherwise it will be ignored.
\par 
\par \f0\tab\f1 For example:
\par 
\par \f2\tab %define COLOR 3
\par \tab %ifdef COLOR
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 will result in the mov statement being assembled because COLOR has been\f0  \f1 defined.
\par 
\par \f0\tab\f1 Note that a definition declared with %define\f0  \f1 or %assign must match the\f0  \f1 argument exactly, whereas a definition declared with %idefine or\f0  \f1 %iassign can differ in case and\f0  \f1 still match. %ifdef will\f0  \f1 not\f0  \f1 match identifiers declared with %macro\f0  \f1 or %imacro.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style\f0  \f1 conditionals.
\par 
\par 
\par 
\par \b %ifndef\b0 
\par \f0\tab\f1 %ifndef\f0  \f1 is a %if-style conditional\f0  \f1 that takes an identifer as\f0  \f1 an argument. If the identifier has not been defined with a\f0  \f1 previous\f0  \f1 %define or \f0 %\f1 assign statement, the next\f0  \f1 block will be assembled, otherwise it will be ignored.
\par 
\par \f0\tab\f1 For\f0  \f1 example:
\par 
\par \f2\tab %define COLOR 3
\par \tab %ifndef COLOR
\par 
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 will result in the mov statement being ignored because COLOR has been\f0  \f1 defined. Alternatively:
\par 
\par \f2\tab %undef COLOR
\par \tab %ifndef COLOR
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 will result in the mov statement being assembled because COLOR is not\f0  \f1 currently defined.
\par 
\par \f0\tab\f1 Note\f0  \f1 that a\f0  \f1 definition declared with %define\f0  \f1 or %assign can have any\f0  \f1 difference\f0  \f1 from the argument and trigger assembly of the block, whereas a\f0  \f1 definition declared with %idefine\f0  \f1 or %iassign must differ in\f0  \f1 some way\f0  \f1 other than in case. %ifndef\f0  \f1 will assemble the following block for identifiers declared\f0  \f1 with %macro or %imacro.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style\f0  \f1 conditionals.
\par 
\par 
\par \b %else\b0 
\par \f0\tab\f1 %else is used to\f0  \f1 select a block for assembly, when all previous\f0  \f1 %if-style conditionals and %elif-style\f0  \f1 conditionals in the same sequence have had their argumentsevaluate to\f0  \f1 false. For example:
\par 
\par \f2\tab %define COLOR = 3
\par \tab %if COLOR ==4
\par \tab mov eax,3
\par \tab %else
\par \tab inc eax
\par \tab %endif
\par 
\par \f0\tab\f1 will result in the mov being ignored, but the inc being assembled,\f0  \f1 because the evaluation of the %if\f0  argument is false.
\par \f1 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 else.\f1 
\par 
\par 
\par \b %endif\b0 
\par 
\par 
\par \f0\tab\f1 %endif is\f0  \f1 used to end a conditional sequence. Once a conditional sequence\f0  \f1 is ended, code assembly proceeds as normal, unless the conditional\f0  \f1 sequence was itself nested within a block of a higher-level conditional\f0  \f1 sequence that is not being assembled.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 endif.\f1 
\par 
\par \pard 
\par }
320
Scribble320
Basic Extensions to C Preprocessor




Writing



FALSE
92
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\fi-700\li700\cf1\b\fs32 Basic Extensions to C Preprocessor\cf0\b0\f1\fs20 
\par \pard 
\par The basic\f0  \f1 extensions to the C Preprocessor generally \f0 add\f1  functionality that is\f0  \f1 very similar to the functionality already found in the C Preprocessor,\f0  \f1 but extends it in some way.
\par 
\par \pard\fi-700\li700\b %assign
\par \b0\f0\tab\f1 %assign is similar\f0  \f1 to the non-functional form of %define,\f0  \f1 in that it defines text to be\f0  \f1 substituted for an identifier. Where %assign differs is that the\f0  \f1 text to be substituted is evaluated\f0  \f1 as if it were an expression at the\f0  \f1 time the %assign is\f0  \f1 encountered. This is useful for example in\f0  \f1 %rep loops. For example\f0  \f1 the following code makes a data structure\f0  \f1 that consists of the integers from 1 to 100, in ascending order:
\par 
\par \f2\tab %assign WORKING 1
\par \tab %rep 100
\par \tab   db WORKING
\par \tab %assign WORKING WORKING + 1
\par \tab %endrep
\par 
\par \f0\tab\f1 But there is another difference with %assign.\f0  \f1 It is the only\f0  \f1 preprocessor directive that interacts with data structures in the\f0  \f1 assembler; so for example the %assign\f0  \f1 expression can contain\f0  \f1 expressions involving labels and the program counter. Thus:
\par 
\par \f2\tab helloWorld db "hello world"
\par  \tab %assign COUNTER 64 - ($-helloWorld) 
\par \tab %assign PADDING ($-helloWorld)
\par \tab %rep COUNTER
\par \tab   db PADDING
\par 
\par \tab %assign PADDING PADDING + 1
\par \tab %endrep
\par 
\par \f0\tab\f1 puts the string "hello world" in the text, followed by the byte for 11,\f0  \f1 the byte for 12, etc... up to the byte for 63.
\par 
\par \f0\tab\f1 Note that this latter behavior of interacting with the assembler is\f0  \f1 only valid if code is being assembled. If the preprocess-only\f0  \f1 switch is specified on the command line, assembler symbols will not be\f0  \f1 available, and the latter example will result in errors.
\par 
\par 
\par 
\par \b\f0 %iassign\b0\f2 
\par \f0\tab\f1 %iassign is a form\f0  \f1 of %assign where the\f0  \f1 identifier is considered to be case\f0  \f1 insensitive. So for example:
\par \f2 
\par \tab %iassign
\par \tab COUNTER 63
\par \tab %rep Counter
\par \tab ...
\par \tab %endrep
\par 
\par \f0\tab\f1 and similar case variants on the word counter would still result in a\f0  \f1 loop\f0  \f1 that executes 63 times.
\par 
\par 
\par \b\f0 %idefine\b0\f1 
\par \f0\tab\f1 %idefine is a form\f0  \f1 of %define where the\f0  \f1 identifier is assumed to be case\f0  \f1 insensitive. So for example:\f0  \f1 
\par 
\par \f2\tab %idefine COUNTER 4
\par \tab %idefine counter 4
\par \tab %idefine Counter 4
\par 
\par \f0\tab\f1 are equivalent statements, and any case variant of the word COUNTER\f0  \f1 will match for substitution. Note that this case sensitivity only\f0  \f1 extends to the identifier; any parameters\f0  \f1 specified in a function-style\f0  \f1 %idefine are still\f0  \f1 case-sensitive for purposes of substitution.
\par 
\par \b %elifdef\b0 
\par 
\par \f0\tab\f1 %elifdef\f0  \f1 is a %elif-style conditional\f0  \f1 that takes an identifer as an\f0  \f1 argument. If\f0  \f1 the identifier has been defined with a previous %define or %assign\f0  \f1 statement, the next block will be assembled, otherwise the next block\f0  \f1 will be ignored.\f0   \f1 For example:
\par 
\par \f2\tab %define COLOR 3
\par \tab %if COLOR == 2
\par \tab mov eax,4
\par \tab %elifdef COLOR
\par \tab  inc eax
\par \tab %endif
\par 
\par \f0\tab\f1 will result in the mov statement being ignored and the inc statement\f0  \f1 being assembled because COLOR has been defined but is not 2.
\par 
\par \f0\tab\f1 Note\f0  \f1 that a\f0  \f1 definition declared with %define\f0  \f1 or %assign must match the\f0  \f1 argument\f0  \f1 exactly, whereas a definition declared with %idefine or\f0  \f1 %iassign can\f0  \f1 differ in case and still match.\f0    \f1 %elifdef\f0  \f1 will not match\f0  \f1 identifiers\f0  \f1 declared with %macro or %imacro.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %elif-style conditionals.
\par 
\par \b 
\par %elifndef\b0 
\par 
\par \f0\tab\f1 %elifndef\f0  \f1 is a %elif-style conditional\f0  \f1 that takes an identifer as an\f0  \f1 argument. If\f0  \f1 the identifier has not been defined with a previous %define or %assign\f0  \f1 statement, the next block will be assembled, otherwise the next block\f0  \f1 will be ignored.
\par 
\par \f0\tab\f1 For example:
\par 
\par \f2\tab %define COLOR 3
\par \tab %if COLOR == 2
\par \tab mov eax,4
\par \tab %elifndef COLOR
\par \tab  inc eax
\par \tab %endif
\par 
\par \f0\tab\f1 will result in nothing being assembled because COLOR is defined but not
\par \f0\tab\f1 equal to 2.
\par 
\par \f0\tab\f1 Note\f0  \f1 that a\f0  \f1 definition declared with %define\f0  \f1 or %assign can have any\f0  \f1 difference\f0  \f1 from the argument and trigger assembly of the block, whereas a\f0  \f1 definition declared with %idefine\f0  \f1 or %iassign must differ in\f0  \f1 some way\f0  \f1 other than in case.\f0   \f1 %elifndef\f0  \f1 will assemble the following block for identifiers declared with\f0  \f1 %macro or %imacro.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %elif-style conditionals.
\par \pard 
\par }
330
Scribble330
Conditional Processing




Writing



FALSE
60
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Conditional Processing\cf0\b0\f1\fs20 
\par 
\par Conditional processing is a way to tell the assembler that som\f0 e \f1 lines of code should be assembled into the program, and other lines may\f0  \f1 be ignored. There are a variety of conditional \f0 pro\f1 cessing\f0  \f1 directives, which use conditions ranging from evaluation of an\f0  \f1 expression, to string comparison, to type or state of a previous symbol\f0  \f1 definition. It is useful particularly in \f0 c\f1 onfigurationmanagement, to allow different configurations of the program to be\f0  \f1 built for example by changing the command line. It is also useful\f0  \f1 in conjunction with \cf2\strike multiline\f0  \f1 macros\cf3\strike0\{linkID=370\}\cf0 , where it can be used to evaluate\f0  \f1 some characteristic of an argument to a macro.
\par 
\par The conditional processing statements can be broken into four basic\f0  \f1 types:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 %if-style conditional
\par {\pntext\f3\'B7\tab}%elif-style conditional
\par {\pntext\f3\'B7\tab}%else
\par {\pntext\f3\'B7\tab}%endif
\par \pard 
\par Conditional processing always starts with a %if-style conditional, and\f0  \f1 ends with a %endif conditional.\f0  \f1 Between them them, there can be\f0  \f1 any number of %elif-style\f0  \f1 conditionals (including none at all),\f0  \f1 followed by zero or one %else\f0  \f1 conditional. A sequence of these\f0  \f1 conditionals breaks the enclosed code up into multiple blocks.
\par 
\par Conditionals may be nested; in other words each block can be\f0  \f1 further broken up into smaller blocks with more conditionals\f0  \f1 that are placed inside the initial conditional. Two conditional
\par statements are considered to be at the same level if all sets of\f0  \f1 conditionals within the blocks specified by the conditionals have both\f0  \f1 a %if-style conditional and a\f0  \f1 %endif conditional.
\par 
\par Processing starts w\f0 ith\f1  the %if-style\f0  \f1 conditional. Its arguments are\f0  \f1 evaluated according to the rules for that conditional. If the\f0  \f1 evaluation returns a value of true, the following block of code is
\par assembled, and any other blocks up to the %endif conditional \f0 which matches this %if-style conditional \f1 are ignored
\par 
\par If however the evaluation of the %if-style\f0  \f1 conditional returns false,\f0  \f1 the following block of code is ignored. Then processing begins\f0  \f1 with any %elif-style conditionals\f0  \f1 at the same level, in the order they\f0  \f1 appear in the code. Each %elif-style\f0  \f1 conditional is successively\f0  \f1 evaluated, until the evaluation of one of the arguments returns\f0  \f1 'true'. For each %elif-style\f0  \f1 conditional that evaluates to false,\f0  \f1 the corresponding block is skipped; if one returns true its code\f0  \f1 block is assembled and all remaining blocks of code up to the\f0  \f1 %endif\f0  \f1 conditional are ignored.
\par 
\par If the evaluation of the %if-style\f0  \f1 conditional returns false, and a\f0  \f1 corresponding %elif-style conditional\f0  \f1 that returns true cannot be found\f0  \f1 at the same level (e.g. they all return false or there aren't any), the\f0  \f1 %else conditional block is\f0  \f1 assembled\f0  if it exists\f1 .
\par 
\par If all the evaluations return false, and there is no %else conditional, none of the\f0  \f1 blocks are assembled.
\par 
\par Various examples follow, for the %if-style\f0  \f1 conditional that evaluates expressions:
\par 
\par \f2 %if COLOR == 4
\par   mov eax,4
\par %endif
\par 
\par %if COLOR == 4
\par   mov eax,4
\par %else
\par   mov eax,1
\par %endif
\par 
\par %if COLOR == 4
\par  mov eax,4
\par %elif COLOR == 3
\par   mov eax,3
\par %elif COLOR == 2
\par mov eax,2
\par %endif
\par 
\par %if COLOR == 4
\par   mov eax,4
\par %elif COLOR== 3
\par   mov eax,3
\par %elif COLOR == 2
\par mov eax,2
\par %else
\par   mov eax,1
\par %endif
\par \f1 
\par Note that when a conditional block is not being assembled, no\f0  \f1 preprocessor directives within that block will be evaluated either\f0  \f1 (other than to allow OAsm's\f0  \f1 preprocessor to \f0 keep track of things like nested conditionals)\f1 
\par 
\par }
340
Scribble340
Text Comparison Conditionals




Writing



FALSE
138
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\fi-720\li720\cf1\b\fs32 Text Comparison Conditionals\cf0\b0\f1\fs20 
\par \pard 
\par The Text\f0  \f1 Comparison Conditionals are used in\f0  \f1  \cf2\strike Conditional Processing\cf3\strike0\{linkID=330\}\cf0 , to\f0  \f1 compare two peices of textual data. Each takes as an argument a\f0  \f1 list of two character sequences,\f0  s\f1 eparated by a comma. Each\f0  \f1 sequence\f0  \f1 is stripped of leading and trailing spaces, and then the sequences are\f0  \f1 compared. It does not matter if the sequences are enclosed in\f0  \f1 quotes.
\par 
\par Depending on the result of the comparison, successive code will be\f0  \f1 assembled or ignored based on the specific directive specified.
\par 
\par Text Comparison Conditionals are useful for example in conjunction with\f0  \f1 %macro and %imacro, to evaluate macro arguments.
\par \pard\fi-720\li720 
\par 
\par \b %ifidn\b0 
\par \f0\tab\f1 %ifidn is a\f0  \f1 %if-style conditional that\f0  \f1 compares the two peices of textual data in a\f0  \f1 case-sensitive manner, and the accompanying code block is assembled if\f0  \f1 the two peices of data match. For example:
\par 
\par \f2\tab %define HELLO\tab goodbye
\par \tab %ifidn HELLO, goodbye
\par 
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because thesubstitution for HELLO matches the text goodbye.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par \b %ifnidn\b0 
\par 
\par \f0\tab\f1 %ifnidn\f0  \f1 is a %if-style conditional\f0  \f1 that compares the two peices of textual data\f0  \f1 in a case-sensitive manner, and the accompanying code block is\f0  \f1 assembled if the two peices of data do not match. For example:
\par 
\par \f0\tab\f1 would result in nothing being assembled because the substitution for\f0  \f1 HELLO matches the text goodbye. Alternatively:
\par 
\par \f2\tab %define HELLO goodbye
\par \tab %ifnidn HELLO, hello
\par 
\par \tab mov eax,3
\par \tab %endif
\par \f0 
\par \tab\f1 would result in the the mov instruction being assembled.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par \b %elifidn\b0 
\par \f0\tab\f1 %elifidn\f0  \f1 is a %elif-style conditional\f0  \f1 that compares the two peices of textual\f0  \f1 data\f0  \f1 in a case-sensitive manner, and the accompanying code block is\f0  \f1 assembled if the two peices of ata match. For example:
\par 
\par \f2\tab %define HELLO goodbye
\par \tab %if HELLO == 1
\par \tab %elifidn HELLO , goodbye
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because the\f0  \f1 substitution for HELLO matches the text goodbye.\f0   ('goodbye' is not defined so when it is evaluated as a number the result is zero).\f1 
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par \b 
\par \f0 %\f1 elifnidn\b0 
\par \f0\tab\f1 %elifnidn\f0  \f1 is a %elif-style conditional\f0  \f1 that compares the two peices of textual\f0  \f1 data\f0  \f1 in a case-sensitive manner, and the accompanying code block is\f0  \f1 assembled if the two peices of data do not match. For example:
\par 
\par \f2\tab %define HELLO goodbye
\par \tab %if HELLO = 1
\par 
\par \tab %elifnidn goodbye , HELLO
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in nothing being assembled because the substitution for\f0  \f1 HELLO matches the text goodbye.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par 
\par 
\par 
\par \b %ifidn\f0 i\f1 
\par \b0\f0\tab\f1 %ifidn\f0 i\f1  is a\f0  \f1 %if-style conditional that\f0  \f1 compares the two peices of textual data in a\f0  \f1 case-\f0 in\f1 sensitive manner, and the accompanying code block is assembled if\f0  \f1 the two peices of data match. For example:
\par 
\par \f2\tab %define HELLO\tab goodbye
\par \tab %ifidni HELLO, GOODBYE
\par 
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because thesubstitution for HELLO matches the text goodbye.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par \b %ifnidn\f0 i\b0\f1 
\par 
\par \f0\tab\f1 %ifnidn\f0 i \f1 is a %if-style conditional\f0  \f1 that compares the two peices of textual data\f0  \f1 in a case-\f0 in\f1 sensitive manner, and the accompanying code block is\f0  \f1 assembled if the two peices of data do not match. For example:
\par 
\par \f2\tab %define HELLO goodbye
\par \tab %ifnidni HELLO , GOODBYE
\par 
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in nothing being assembled because the substitution for\f0  \f1 HELLO matches the text goodbye. Alternatively:
\par 
\par \f2\tab %define HELLO goodbye
\par \tab %ifnidni HELLO, hello
\par 
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the the mov instruction being assembled.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par \b %elifidn\f0 i\f1 
\par \b0\f0\tab\f1 %elifidn\f0 i \f1 is a %elif-style conditional\f0  \f1 that compares the two peices of textual\f0  \f1 data\f0  \f1 in a case-\f0 in\f1 sensitive manner, and the accompanying code block is\f0  \f1 assembled if the two peices of ata match. For example:
\par 
\par \f2\tab %define HELLO goodbye
\par \tab %if HELLO == 1
\par \tab %elifidni HELLO , GOODBYE
\par \tab mov eax,3
\par \tab %endif
\par \f1 
\par \f0\tab\f1 would result in the mov statement being assembled because the\f0  \f1 substitution for HELLO matches the text goodbye.\f0   ('goodbye' is not defined so when it is evaluated as a number the result is zero).\f1 
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par \b\f0 %\f1 elifnidn\f0 i\b0\f1 
\par \f0\tab\f1 %elifnidn\f0 i \f1 is a %elif-style conditional\f0  \f1 that compares the two peices of textual\f0  \f1 data\f0  \f1 in a case-\f0 in\f1 sensitive manner, and the accompanying code block is\f0  \f1 assembled if the two peices of data do not match. For example:
\par 
\par \f2\tab %define HELLO goodbye
\par \tab %if HELLO = 1
\par 
\par \tab %elifnidni GOODBYE , HELLO
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in nothing being assembled because the substitution for\f0  \f1 HELLO matches the text goodbye.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par 
\par 
\par 
\par 
\par \pard 
\par }
350
Scribble350
Token Classification Conditionals




Writing



FALSE
163
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\fi-720\li720\cf1\b\fs32 Token Classification Conditionals\cf0\b0\f1\fs20 
\par 
\par \pard The Token Type\f0  \f1 Classification conditionals are used in \cf2\strike Conditional Processing\cf3\strike0\{linkID=330\}\cf0 .\f0   \f1 They take a character stream, and determine what kind\f0  \f1 of token the assembler will think it would be if it were to assemble\f0  \f1 the stream as part of processing an instruction or directive.\f0   \f1 This is useful for example within multiline macro definitions, to\f0  \f1 change the behavior of a macro based on the type of data in one or more\f0  \f1 of the arguments. Token Type classification\f0  \f1 directives can detect one of three types of token: labels,\f0  \f1 numbers, and strings.
\par 
\par \pard\fi-720\li720\b %ifid\b0 
\par \f0\tab\f1 %ifid is a\f0  \f1 %if-style conditional that\f0  \f1 detects if the token could be a label, and\f0  \f1 processes the following block if it is.
\par \f2 
\par \tab %ifid myLabel
\par \tab mov eax,3
\par \tab %endif
\par \f0 
\par \tab\f1 would result in the mov statement being assembled because myLabel\f0  \f1 matches a character sequence that could be used in a label. It\f0  \f1 does not matter if myLabel has actually been defined; the fact that it\f0  \f1 could be assembled as a label is all that is needed.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par 
\par \b 
\par %ifnid\b0 
\par \f0\tab\f1 %ifnid is a\f0  \f1 %if-style conditional that\f0  \f1 detects if the token could be a label, and\f0  \f1 processes the following block if it is not.
\par 
\par \f2\tab %ifnid 5
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because 5 is anumber, and does not match the character sequence required for a label.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par 
\par \b %elifid\b0 
\par \f0\tab\f1 %elifid is a\f0  \f1 %elif-style conditional that\f0  \f1 detects if the token could be a label, and\f0  \f1 processes the following block if it is.
\par 
\par \f2\tab %if 1 == 2
\par \tab %elifid myLabel
\par 
\par \tab mov eax,3
\par \tab %endif
\par \f1 
\par \f0\tab\f1 would result in the mov statement being assembled because myLabel\f0  \f1 matches a character sequence that could be used in a label. It\f0  \f1 does\f0  \f1 not matter if myLabel has actually been defined; the fact that it could\f0  \f1 be assembled as a label is all that is needed.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par 
\par \b %elifnid\b0 
\par \f0\tab\f1 %elifnid is a\f0  \f1 %elif-style conditional that\f0  \f1 detects if the token could be a label, and\f0  \f1 processes the following block if it is not\f0 .\f1 
\par \f0 
\par \f2\tab %if 1 == 2
\par \tab %elifnid 5
\par \tab mov eax,3
\par \tab %endif
\par \f1 
\par \f0\tab\f1 would\f0  \f1 result in the mov statement being assembled because 5 is a number, and\f0  \f1 does not match the character sequence required for a label.\f0  \f1 
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par \b %if\f0 num\b0\f1 
\par \f0\tab\f1 %if\f0 num\f1  is a\f0  \f1 %if-style conditional that\f0  \f1 detects if the token could be a \f0 number\f1 , and\f0  \f1 processes the following block if it is.
\par 
\par \f2\tab %ifnum 5
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because \f0 5 \f1 matches a character sequence that could be used \f0 as a number.\f1 
\par \f0 
\par \tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par 
\par \b %ifn\f0 num\f1 
\par \b0\f0\tab\f1 %ifn\f0 num\f1  is a\f0  \f1 %if-style conditional that\f0  \f1 detects if the token could be a \f0 number\f1 , and\f0  \f1 processes the following block if it is not.
\par 
\par \f2\tab %ifnnum mylabel
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because \f0 mylabel does not match a characer sequence that could be a number.\f1 
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par 
\par \b %elif\f0 num\b0\f1 
\par \f0\tab\f1 %elif\f0 num\f1  is a\f0  \f1 %elif-style conditional that\f0  \f1 detects if the token could be a \f0 number\f1 , and\f0  \f1 processes the following block if it is.
\par 
\par \f2\tab %if 1 == 2
\par \tab %elifnum 5
\par 
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because \f0 5 \f1 matches a character sequence that could be used \f0 as a number\f1 .
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par 
\par \b %elifn\f0 num\b0\f1 
\par \f0\tab\f1 %elifn\f0 num\f1  is a\f0  \f1 %elif-style conditional that\f0  \f1 detects if the token could be a \f0 number\f1 , and\f0  \f1 processes the following block if it is not\f0 .\f1 
\par \f0 
\par \f2\tab %if 1 == 2
\par \tab %elifnnum myLabel
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would\f0  \f1 result in the mov statement being assembled because\f0  mylabel does not match a character sequence which could be a number\f1 
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par 
\par \b %if\f0 str\f1 
\par \b0\f0\tab\f1 %if\f0 str\f1  is a\f0  \f1 %if-style conditional that\f0  \f1 detects if the token could be a \f0 string\f1 , and\f0  \f1 processes the following block if it is.
\par 
\par \f2\tab %ifstr "Hello World"
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because \f0 "Hello World" \f1  a character sequence that could be used \f0 as a string\f1 .
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par \b %if\f0 nstr\b0\f1 
\par \f0\tab\f1 %ifn\f0 str\f1  is a\f0  \f1 %if-style conditional that\f0  \f1 detects if the token could be a\f0  string\f1 , and\f0  \f1 processes the following block if it is not.
\par \f2 
\par \tab %ifnstr 5
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because 5 is anumber, and does not match the character sequence required for a \f0 string.\f1 
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par \b 
\par %elif\f0 str\b0\f1 
\par \f0\tab\f1 %elif\f0 str\f1  is a\f0  \f1 %elif-style conditional that\f0  \f1 detects if the token could be a\f0  string\f1 , and\f0  \f1 processes the following block if it is.
\par 
\par \f2\tab %if 1 == 2
\par \tab %elifstr "hello world"
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would result in the mov statement being assembled because \f0 "hello world" \f1 matches a character sequence that could be used \f0 as a string\f1 .
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par \b %elifn\f0 str\b0\f1 
\par \f0\tab\f1 %elifn\f0 str\f1  is a\f0  \f1 %elif-style conditional that\f0  \f1 detects if the token could be a \f0 string\f1 , and\f0  \f1 processes the following block if it is not\f0 .\f1 
\par \f0 
\par \f2\tab %if 1 == 2
\par \tab %elifnstr 5
\par \tab mov eax,3
\par \tab %endif
\par 
\par \f0\tab\f1 would\f0  \f1 result in the mov statement being assembled because 5 is a number, and\f0  \f1 does not match the character sequence required for a \f0 string\f1 .\f0  \f1 
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par 
\par 
\par 
\par 
\par 
\par 
\par \pard 
\par }
360
Scribble360
Context-Related Extensions




Writing



FALSE
166
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\fi-720\li720\cf1\b\fs32 Context-Related Extensions\cf0\b0\f1\fs20 
\par 
\par 
\par \pard The Context Related extensions are used to define preprocessor\f0  \f1 contexts. A preprocessor context can be used to create a memory\f0  \f1 of state information, between different invocations of multiline\f0  \f1 macros. For example, a set of macros could be defined to mimic\f0  \f1 high-level control functions such as loops and if statements.
\par 
\par Many combinations of contexts can exist simultaneously.\f0   \f1 OAsm\f0  \f1 maintains a stack of all open contexts, pushing new contexts on the top\f0  \f1 of the stack and removing old contents from the top of the stack.\f0   \f1 Each context has a name, and a state which can include context-specific\f0  \f1 variables and definitions. The name of the context on top of the\f0  \f1 stack can be examined to\f0  \f1 determine what the current context is, or\f0  \f1 changed. It might be useful to change it for example if two\f0  \f1 macros maintain a context, but a third macro might change the context\f0  \f1 based on its arguments, e.g. to allow processing by a fourth macro\f0  \f1 which would not succeed if the name on top of the context stack wasn't\f0  \f1 correct.
\par 
\par Within a context, context-specific definitions and labels may be\f0  \f1 defined for reference from other macros. Each context-specific\f0  \f1 definition is in scope while that context is in scope, e.g. while the\f0  \f1 context is on top of the context-stack. If a context is open\f0  \f1 multiple times\f0  \f1 simultaneously, each instance of the open context is unique, even\f0  \f1 though the textual representation labels in in the source code for that\f0  \f1 context may be the same.
\par 
\par Context-specific definitions and labels start with the sequence '%$'\f0  \f1 and then contain a label start character and other label characters,\f0  \f1 just like other identifiers. For example:
\par 
\par \f2 %$HelloWorld\f1 
\par 
\par could be used in a context-specific definition or label, and would\f0  \f1 signify that that label goes with the current context. As an\f0  \f1 example consider the two macros
\par 
\par \f2 %macro BEGIN 0
\par %push MyBegin
\par %$HelloWorld:
\par %endmacro
\par 
\par %macro FOREVER 0
\par %ifctx MyBegin
\par jmp %$HelloWorld
\par %pop
\par %else
\par %error FOREVER loop without matching BEGIN
\par %endif
\par %endmacro
\par \f1 
\par could be used to implement an infinite loop as follows, if the macros\f0  \f1 are used in a pair. 
\par 
\par \f2 BEGIN
\par inc EAX
\par FOREVER
\par 
\par \f1 Contexts can\f0  \f1 also have 'local' \f0 macro \f1 definitions:
\par 
\par \f2 %push MY_CONTEXT
\par %define %$four 4
\par 
\par \f1 results in a definition that will only be valid while this instance of\f0  \f1 MY_CONTEXT is on top of the context stack.
\par 
\par When contexts are used, they don't have to appear within a multiline\f0  \f1 macro definition, but it is often useful to use them this way.
\par 
\par Note: OAsm does not\f0  \f1 separate context-specific label names into\f0  \f1 different namespaces. Instead, a prefix is inserted before the\f0  \f1 symbol's name and the symbol is entered in the global symbol\f0  \f1 table. The prefix takes the form of a non-local label, with\f0  \f1 context-instance identifying information. This identifying\f0  \f1 information is simply an integer followed by the character '$'.\f0  \f1 For example if the context instance number is 54, the label %$Hello\f0  \f1 would be translated to:
\par 
\par \f0\tab\f2 ..@54$Hello\f1 
\par 
\par by the preprocessor. Non-local labels of this general form should\f0  \f1 be avoided as they may collide with labels defined locally within a
\par context. This also applies to locally defined  %define statements.
\par 
\par \pard\fi-720\li720\b %push\b0 
\par \f0\tab\f1 %push creates a\f0  \f1 new context and pushes it on the top of the context stack:
\par 
\par \f0\tab\f2 %push CONTEXT_NAME\f1 
\par 
\par \f0\tab\f1 'local' definitions can be made within this context as indicated in the\f0  \f1 introduction.
\par 
\par \f0\tab\f1 If %push is used multiple times with the same context name, each\f0  \f1 context is unique even though the names are the same. So for\f0  \f1 exampl\f0 e:\f1 
\par 
\par \f0\tab\f2 %push MY_CONTEXT
\par \tab %$contextLabel:
\par \tab %push MY_CONTEXT
\par \tab %$contextLabel:
\par \f1 
\par \f0\tab\f1 is valid, because the two labels are named locally to the context and\f0  \f1 are in different context instances. When the label is used, it\f0  \f1 will be matched to the context currently on top of the \f0 c\f1 ontext stack.
\par 
\par \b\f2 %pop\b0 
\par \f0\tab\f1 %pop removes the context at the\f0  \f1 top of the context stack:
\par 
\par \f2\tab %push MY_CONTEXT
\par \tab %pop 
\par 
\par \f0\tab\f1 results in MY_CONTEXT no longer being active, and the context that was\f0  \f1 below it on the context-stack becomes active. Note, you should\f0  \f1 use any labels or definitions that are\f0  \f1 specific to a context before it\f0  \f1 is popped. Once a context is popped off the stack, its state is\f0  \f1 never recoverable. 
\par 
\par \b %repl\b0 
\par \f0\tab\f1 %repl changes the name of the\f0  \f1 context at the top of the\f0  \f1 context-stack. For example:
\par 
\par \f2\tab %push MY_CONTEXT
\par 
\par \f0\tab\f1 creates a context called MY_CONTEXT. If that is followed by:
\par 
\par \f2\tab %repl NEW_NAME
\par 
\par \f0\tab\f1 the context will now be called NEW_NAME. When a context is\f0  \f1 renamed this way, all previous local definitions and labels are still\f0  \f1 accessible while that context is on top of the\f0  \f1 context stack. The\f0  \f1 only affect of renaming the context is that\f0  \f1 conditionals which act on context names will be matched against the new\f0  \f1 name instead of the old one.
\par 
\par 
\par \b %ifctx\b0 
\par \f0\tab\f1 %ifctx is a\f0  \f1 %if-style conditional that\f0   \f1 takes a context name as an argument.\f0  \f1 If the context name matches the name of the context on top of the\f0  \f1 context stack, the next block is assembled, otherwise it is not.\f0   \f1 For example:
\par 
\par \f2\tab %push MY_NAME
\par \tab %ifctx MY_NAME
\par \tab mov eax,4
\par \tab %endif
\par \f1 
\par \f0\tab\f1 will result in the mov statement being assembled because the top of the\f0  \f1 context stack is named MY_NAME, whereas:
\par 
\par \f2\tab %push MY_NAME
\par \tab %ifctx ANOTHER_NAME
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 will result in nothing being assembled because the name of the top of\f0  \f1 the context stack does not match the argument to %ifctx.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par 
\par \b %ifnctx\b0 
\par \f0\tab\f1 %ifnctx is a\f0  \f1 %if-style conditional that\f0  \f1 takes a context name as an argument.\f0  \f1 If the\f0  \f1 context name does not match the name of the context on top of the\f0  \f1 context\f0  \f1 stack, the next block is assembled, otherwise it is not.
\par 
\par \f0\tab\f1 For example:
\par 
\par \f2\tab %push MY_NAME
\par \tab %ifnctx MY_NAME
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 will result in nothing being assembled because the name of the context\f0  \f1 on top of the stack matches the argument.
\par 
\par \f2\tab %push MY_NAME
\par \tab %ifnctx ANOTHER_NAME
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 will result in the mov being assembled because the names do not match.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %if-style conditionals.
\par 
\par 
\par 
\par \b %elfictx\b0 
\par \f0\tab\f1 %elifctx is a\f0  \f1 %elif-style conditional that\f0  \f1 takes a context name as an argument.\f0   \f1 If the\f0  \f1 context name matches the name of the context on top of the context\f0  \f1 stack, the next block is assembled, otherwise it is not.
\par 
\par \f0\tab\f1 For example:
\par 
\par \f2\tab %push MY_NAME
\par \tab %if 44
\par \tab %elifctx MY_NAME
\par 
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 will result in the mov statement being assembled because the top of the\f0  \f1 context stack is named MY_NAME.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par 
\par \b %elifnctx
\par \b0\f0\tab\f1 %elifnctx is a\f0  \f1 %elif-style conditional that\f0  \f1 takes a context name as an argument.\f0  \f1 If the\f0  \f1 context name does not match the name of the context on top of the\f0  \f1 context\f0  \f1 stack, the next\f0  \f1 block is assembled, otherwise it is not.
\par 
\par \f0\tab\f1 For example:
\par \f2 
\par \tab %push MY_NAME
\par \tab %if 44
\par 
\par \tab %elifctx MY_NAME
\par 
\par \tab mov eax,4
\par \tab %endif
\par 
\par \f0\tab\f1 will result in nothing being assembled because the names match.
\par 
\par \f0\tab\f1 See the section on \cf2\strike Conditional\f0  \f1 Processing\cf3\strike0\{linkID=330\}\cf0  for more on %\f0 el\f1 if-style conditionals.
\par \pard 
\par }
370
Scribble370
Multi-Line Macro Extensions




Writing



FALSE
188
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Multi-Line Macro Extensions\cf0\b0\f1\fs20 
\par 
\par Multiline macro extensions allow definition of types of macros that\f0  \f1 are more familiar to assembly language programmers. Such macros\f0  \f1 may contain an arbitrary number of assembly\f0  l\f1 anguage statements and\f0  \f1 preprocessor directives. These macros have three parts: the\f0  \f1 macro header, the macro body, and the macro invocation.
\par 
\par The macro header and macro body are used when defining the macro.\f0   \f1 For example a simple multiline macro that gives a new name to NOP might\f0  \f1 look as follows:
\par 
\par \f2 %macro MY_NOP 0
\par nop
\par %endmacro
\par 
\par \f1 here MY_NOP is the name of the macro, which is case-sensitive since\f0  \f1 %macro was used rather than %imacro. The zero in the header\f0  \f1 following MY_NOP indicates this macro has no parameters. The body\f0  \f1 of the macro is the 'nop' statement, and the macro definition ends with\f0  \f1 %endmacro.
\par 
\par After the macro is defined, it can be invoked as many times as\f0  \f1 necessary. For example:
\par 
\par \f2 MY_NOP
\par MY_NOP
\par 
\par \f1 causes the assembler to assemble two 'nop' statements based on the\f0  \f1 above definition.
\par 
\par A macro can have parameters:
\par 
\par \f2 %macro MY_MOV 2
\par mov %1, %2
\par %endmacro\f1 
\par 
\par In this macro, the 2 in the header signifies the macro takes exactly\f0  \f1 two arguments. Specifying more arguments, or less, during and\f0  \f1 invocation, will result in an error. The %1 and %2 are
\par substituted\f0  \f1 with the first and second arguments. For example:
\par 
\par \f2 MY_MOV eax,ebx
\par 
\par \f0 becomes:
\par \f1 
\par \f2 mov eax,ebx
\par 
\par \f1 Macros can have a variable number of parameters. In:
\par 
\par \f2 %macro MY_PUSH 1-4
\par %rep %0
\par push %1
\par %rotate 1
\par %endrep
\par %endmacro
\par 
\par \f1 the header specifies the macro can have between one and four\f0  \f1 arguments. In the body, the %0 gives the number of arguments that\f0  \f1 were actually specified. The %rotate rotates the\f0  \f1 arguments left\f0  \f1 by one, so that the contents of %2 moves to %1, the contents of %3\f0   \f1 moves to %2, and so forth. The contents of %1 moves to the end of\f0  \f1 the list.
\par 
\par Invoking this macro as follows:
\par 
\par \f2 MY_PUSH eax,ebx,ecx
\par 
\par \f1 results in the preprocessor generating the following instructions:
\par 
\par \f2 push eax
\par push ebx
\par push ecx
\par 
\par \f1 This can be made just a little bit better through the use of the\f0  \f1 infinite argument list specifier:
\par 
\par \f2 %macro MY_PUSH 1-*
\par %rep %0
\par push %1
\par %rotate 1
\par %endrep
\par %endmacro
\par 
\par \f1 where the * in the header means there is no practical limit to the\f0  \f1 number of arguments (there is a limit, but it wouldn't be realistic to\f0  \f1 type that many arguments even with a code-generating program).\f0  \f1 Now the macro isn't limited to four push statements; it can push as\f0  \f1 many things as are listed in the macro invocation.
\par 
\par A corresponding MY_POP can be created with minor changes:
\par 
\par \f2 %macro MY_POP 1-*
\par %rep %0
\par %rotate -1
\par pop %1
\par %endrep
\par %endmacro
\par 
\par \f1 where the rotate statement now shifts right instead of left, with the\f0  \f1 rightmost argument appearing in %1.
\par 
\par Occasionaly it is beneficial to specify that you need some arguments,\f0  \f1 then you want the rest of the command line:
\par 
\par \f2 %define STRINGIZE(s) #s
\par %macro MY_MSG 1+ 
\par db  %1,STRINGIZE(%2)
\par %endmacro
\par 
\par \f1 Where the + symbol means anything beyond the first argument should be\f0  \f1 gathered together and make another argument. This does include\f0  \f1 comma characters; after the first argument's\f0  \f1 separating comma commas\f0  \f1 will no longer be processed with this syntax. As an example\f0  \f1 invocation:
\par 
\par \f2 MY_MSG 44, hello there, world
\par would translate to:
\par 
\par db 44,"hello there, world"
\par \f1 
\par Of course the + symbol may be combined with specifying variable length\f0  \f1 argument lists as shown in the following header:
\par 
\par \f2 %macro do_something 1-4+
\par \f1 
\par Another use for the + symbol is to get the entire argument list of a\f0  \f1 macro invocation, unparsed, as shown in the following header:
\par 
\par \f2 %macro do_something 0+
\par \f0 
\par \f1 Sometimes it is useful to include the comma character in the argument\f0  \f1 for a macro invocation:
\par 
\par \f2 %macro define_numbers 3
\par db %1,%2,%3
\par %endmacro
\par 
\par define_numbers \{1,2\},3,4
\par \f1 
\par to result in:
\par 
\par \f2 db 1,2,3,4
\par 
\par \f1 When variable length argument lists are used, everything starting with\f0  \f1 the first variable argument can have a default value. For example\f0  \f1 with the macro header:
\par 
\par \f2 %macro define_strings 1-3 "hello", "there"
\par %rep %0
\par db %1
\par %rotate 1
\par %endrep
\par %endmacro
\par 
\par \f1 defaults the second and third arguments to "hello" and "there"\f0  \f1 respectively, if they are not specified in the invocation. For\f0  \f1 example:
\par 
\par \f2 define_strings "one"
\par 
\par \f1 results in:
\par 
\par \f2 db "one"
\par db "hello"
\par db "there"
\par \f1 
\par whereas\f0 :
\par 
\par \f2 define_strings "one", "two"
\par 
\par \f1 results in:
\par 
\par \f2 db "one"
\par db "two"
\par db "there"
\par 
\par \f1 Many of the above macros are unexciting, and perform functions that\f0  \f1 could be done other ways e.g. with %define.\f0  \f1 A more interesting\f0  \f1 example of a multiline macro is as follows:
\par 
\par \f2 %macro power 2
\par 
\par mov ecx,%1
\par mov eax,1
\par jecxz %noop
\par mov eax,%2
\par cmp ecx,1
\par jz %noop
\par %local:
\par imul eax,%2
\par loop %local
\par %noop:
\par %endmacro
\par 
\par \f1 which creates code to raise the second argument to the power of the\f0  \f1 first argument,\f0  \f1 and leaves the result in eax. An example invocation:
\par 
\par \f2 power 2,3\f1 
\par 
\par which generates code to return 3 squared in eax. Here we have\f0  \f1 introduced local\f0  \f1 labels in macros, which are similar in form to local labels in\f0  \f1 contexts, except that the macro version does\f0  \f1 not have a dollars\f0  \f1 symbol. Such local labels are in scope for a single invocation of\f0  \f1 the macro; each time the macro is invoked the label will have a\f0  \f1 different context. 
\par 
\par As with context-specific labels, the assembler does not implement\f0  \f1 multiple symbol tables but instead uses a non-local label name.The non-local label name consists of ..@ followed by a context id\f0  \f1 followed by a period followed by the label name. For example, the\f0  \f1 labels in the above example would be translated to:
\par 
\par \f2 ..@54.local
\par ..@54.noop
\par 
\par \f1 if the context identifier for the current macro invocation is 54.\f0  \f1 non-local labels fitting this general format should not appear in the\f0  \f1 source code, as there is a chance they will conflict with label names\f0  \f1 chosen by the preprocessor.
\par 
\par 
\par \pard\fi-700\li700\b %macro
\par \b0\f0\tab\f1 %macro starts a macro\f0  \f1 definition. The name of the macro is case-sensitive.
\par 
\par \b %imacro\b0 
\par \f0\tab\f1 %imacro starts a macro\f0  \f1 definition. The name of the macro is not case-sensitive.
\par 
\par 
\par \b %endmacro\b0 
\par \f0\tab\f1 %endmacro ends a macrodefinition.
\par 
\par 
\par \b %rotate\b0 
\par \f0\tab\f1 %rotate rotates the\f0  \f1 macro argument list for the current invocation a number of times\f0  \f1 specified in the argument. If the number of times is positive,\f0  \f1 the arguments are rotated left, with\f0  \f1 the leftmost arguments going to\f0  \f1 the end of the list. If the number of times is negative, the\f0  \f1 arguments are rotated right, with the rightmost arguments going to the\f0  \f1 beginning of the list.
\par 
\par \pard 
\par }
380
Scribble380
Repeat Block Extensions




Writing



FALSE
69
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green255\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Repeat Block Extensions\cf0\b0\f1\fs20 
\par 
\par The Repeat Block Extensions allow a method for replicating\f0  \f1 lines of code. In the simplest case, a sequence of instructions\f0  \f1 or data can be literally repeated a fixed number of times:
\par 
\par \f2 %rep 10
\par  nop
\par %endrep
\par \f1 
\par causes the preprocessor to present 10 nop instructions to the\f0  \f1 assembler. In a more complex case, %assign can be used to define\f0  \f1 a function that varies with each loop iteration, allowing easy\f0  \f1 development of lookup tables:
\par 
\par \f2 %assign i 20
\par %rep 10
\par db i
\par %assign i i - 1
\par %endrep
\par 
\par \f1 puts the numbers from 20 to 11 in a table, in decreasing order.
\par 
\par This type of functionality could be used with more complex functions,\f0  \f1 for example %rep would be one\f0  \f1 way a CRC lookup table could be developed\f0  \f1 with OAsm.
\par 
\par In another case the loop count could be made to vary based on previous\f0  \f1 declarations:
\par 
\par \f2 hello db "Hello World"
\par %assign count 64 - ($-Hello)
\par %rep count
\par db 0
\par %endrep
\par \f0 
\par \f1 While the latter example is not too exciting and could be done other\f0  \f1 ways, e.g. with the resb or times directives, more complex functions\f0  \f1 could be integrated into this type of loop to generate different kinds\f0  \f1 of data.
\par 
\par Repeat blocks may be nested. For example:
\par 
\par \f2 %assign i 10
\par %rep 3
\par %rep 3
\par db i
\par %assign i i + 1
\par %endrep
\par %assign i i - 6
\par %endrep
\par \f1 
\par generates enough db statements to define the following sequence:
\par 
\par 10, 11, 12, 7, 8, 9, 4, 5, 6
\par 
\par Repeat blocks can be exited prematurely. If a %exitrep directive\f0  \f1 is parsed while a repeat block is being processed, the innermost repeat\f0  \f1 block exits immediately. Generally, one would use preprocessor\f0  \f1 conditionals to prevent the %exitrep\f0  \f1 directive from being processed,\f0  \f1 until some condition occurs. For example to pop all contexts\f0  \f1 named "MY_CONTEXT" from the top of the context stack:
\par 
\par \f2 %repeat 1000
\par     
\par \cf2  // 1000 is an arbitrary value
\par \cf0 %ifnctx MY_CONTEXT
\par %exitrep
\par %endif
\par %pop
\par %endrep
\par \f1 
\par \pard\fi-700\li700\b %rep
\par \b0\f0\tab\f1 %rep is used to\f0  \f1 start a repeat block. It takes one argument: the number of\f0  \f1 repetitions to go through.
\par 
\par \b %endrep\b0 
\par 
\par \f0\tab\f1 %endrep is used to\f0  \f1 end a repeat block. It takes no arguments
\par \b 
\par %exitrep\b0 
\par \f0\tab\f1 %exitrep is used to\f0  \f1 exit a repeat block prematurely.
\par 
\par }
390
Scribble390
OGrep




Writing



FALSE
27
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 OGrep\cf0\b0\f1\fs20 
\par 
\par OGrep is a\f0  \f1 utility that searches for text within files. It is capable of\f0  \f1 matching text with a direct string comparison, with or\f0  \f1 without case sensitivity. It also is capable of matching text\f0  \f1 within a file against \cf2\strike regular\f0  \f1 expressions\cf3\strike0\{linkID=410\}\cf0 . Regular expressions\f0  \f1 allow a mechanism for specifying ways to match text, while specifying\f0  \f1 parts of the text which can vary and still match. For example the\f0  \f1 '.' character matches any other character, so a regular expression such\f0  \f1 as 'a.c' would match any three-character string starting with 'a'\f0  \f1 and ending with 'c'. More powerful matching is also\f0  \f1 possible, such as matching sequences of the same character, matching\f0  \f1 against any character in a specified set, and so forth.
\par 
\par The general\f0  \f1 format of an OGrep \cf2\strike command line\cf3\strike0\{linkID=400\}\cf0  is as\f0  \f1 follows:
\par 
\par \f2 OGrep [options] match-string list-of-files\f1 
\par 
\par OGrep will search in the list-of-files for text that matches\f0  \f1 the match-string, and\f0  \f1 list file and optionally line number information for each match
\par found. In\f0  \f1 simple cases the match string \f0 will not\f1  be surrounded by quotes,\f0  \f1 but in more complex cases involving spacing characters and special
\par symbols it may be necessary to quote the match-string.
\par 
\par OGrep has a\f0  \f1 powerful regular\f0  \f1 expression matcher, which is turned on by default.However there is a command\f0  \f1 line option to disable it. When it is\f0  \f1 turned on some characters will not be matched directly against the\f0  \f1 text, but will be interpreted in a way that allows the program to\f0  \f1 perform abstract types of matches. 
\par 
\par There are several types of\f0  \f1 matching groups:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 Match a character or sequence of the same characte\f0 r\f1 
\par {\pntext\f3\'B7\tab}Match the start or end of a lin\f0 e\f1 
\par {\pntext\f3\'B7\tab}Match the start, end, or middle of a wor\f0 d
\par \f1{\pntext\f3\'B7\tab}Match one character out of a set of characters
\par {\pntext\f3\'B7\tab}A match can be specified to be repeated at another point within\f0  \f1 the sequence
\par \pard 
\par Some of these matching algorithms can be combined, for example matching\f0  \f1 one character out of a set of characters can be combined with matching\f0  \f1 a sequence of characters to find words consisting of characters in a\f0  \f1 subset of the letters and numbers.
\par 
\par }
400
Scribble400
Command Line




Writing



FALSE
55
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Command Line\cf0\b0\f1\fs20 
\par 
\par The general\f0  \f1 format of an OGrep command\f0  \f1 line is as follows:
\par 
\par \f2 OGrep [options] match-string list-of-files\f1 
\par 
\par Where [options]\f0  \f1 are command line options, and the match-string\f0  \f1 is searched for within\f0  \f1 the list-of-files.\f0   \f1 Matches are listed with file and optionally\f0  \f1 line number. The files in the list-of-files\f0  \f1 may contain\f0  \f1 wildcards, for example:
\par \f2 
\par OGrep "while" *.c\f1 
\par 
\par Looks through all C language source files in the current directory for\f0  \f1 the word while.
\par 
\par By default the match-string is assumed to hold a \cf2\strike regular expression\cf3\strike0\{linkID=410\}\cf0 .
\par 
\par \f0 Following is a list of the command line switches that OGrep supports.\f1 
\par 
\par \pard\fi-700\li700\b\f0 -?\b0  Getting help
\par 
\par \tab OGrep usually has rather abbreviated usage text.  To get more detail, use this switch.\f1 
\par 
\par \b\f0 -i\b0  case sensitivity
\par 
\par \tab By default OGrep performs case-sensitive matching.  This switch will change it to use case-insensitive matching.\f1 
\par 
\par \b\f0 -r\b0  disable regular expressions
\par 
\par \tab By default OGrep uses regular expression pattern matching.  To use exact matches instead, specify this option\f1 
\par 
\par \b\f0 -d\b0  recurse through subdirectories
\par 
\par \tab In some instances it is necessary to have OGrep parse all the files in an entire directory tree.  This switch is used to allow for that behavior.  F\f1 or example:
\par 
\par \f2\tab OGrep -d "while" *.c
\par 
\par \f0\tab\f1 searches for the word while in\f0  \f1 all files ending in .c, in the\f0  \f1 current directory as well as all its subdirectories.
\par 
\par \b\f0 -w\b0  match whole words
\par 
\par \tab By default OGrep will match text regardless of where it appears.  To make it only match entire words specify this switch.\f1 
\par \f0 
\par \pard\f1 For example by default the regular expression 'abc'\f0  \f1 would match\f0  \f1 within both 'abc' and 'xabcy'. There are regular expression\f0  \f1 modifiers that can be used to make it match only 'abc' since in the\f0  \f1 other case abc occurs within another word. With \f0 this \f1 switch, OGrep automatically\f0  \f1 takes the match string and makes it into\f0  \f1 this type of regular expression. E.g, when the <span
\par 
\par 
\par By default OGrep will\f0  \f1 list each filename when it finds the first match within a file, then\f0  \f1 list each matching line underneath it. At the end it will show a\f0  \f1 count of the number of matches. But there are various command\f0  \f1 line options which can modify the format of the output.
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200\f0 -\f1 c list the file\f0  \f1 names matched, along with a count of\f0  \f1 matches 
\par {\pntext\f3\'B7\tab}-l list only the\f0  \f1 file names for files that have matches\f0  \f1   
\par {\pntext\f3\'B7\tab}-n list the line number\f0  \f1 of matching lines next to the matching\f0  \f1 line
\par {\pntext\f3\'B7\tab}-o (Unix format) list the\f0  \f1 file name and line number to the left\f0  \f1 of each matching line, instead of showing the file names separately.
\par {\pntext\f3\'B7\tab}-v lines nonmatching\f0  \f1 lines instead of matching lines
\par {\pntext\f3\'B7\tab}-z list the file names\f0  \f1 matched, line numbers, and matched lines
\par \pard   
\par }
410
Scribble410
Regular Expressions




Writing



FALSE
83
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Regular Expressions\cf0\b0\f1\fs20 
\par 
\par Regular expressions can be used as expressions within\f0  \f1 match-strings. They allow a more general mechanism for doing\f0  \f1 pattern-matches than having to specify each character\f0  \f1 specifically. For example, the '.' matches any character, so\f0  \f1 using the sequence 'a.c' would match any three character sequence\f0  \f1 starting with 'a' and ending with 'c'. This page discusses the\f0  \f1 various\f0  \f1 pattern matching mechanisms available when regular expression\f0  \f1 matching is enabled.
\par 
\par Because the\f0  \f1 pattern matching function of regular expressions uses characters to\f0  \f1 specify patterns, those characters can not be matched directly.
\par 
\par For example '.' matches any character, but there may be instances when\f0  \f1 you want it to match only a period. To resolve this, the pattern\f0  \f1 matching symbols may be quoted by preceding them with the '\\'\f0  \f1 character. This means that the pattern 'a\\.b' matches only the\f0  \f1 sequence a.b.
\par 
\par Since the quote character generally means quote the next character,\f0  \f1 trying to match a quote character means the quote character itself has\f0  \f1 to be quoted. For example to match the text 'a\\b' one one have to\f0  \f1 write the pattern 'a\\\\b'.
\par 
\par The quote character is sometimes used to extend the working of the\f0  \f1 pattern matcher, for example the sequence \\b does not mean a 'b'\f0  \f1 character is being quoted, it means match the beginning or ending of a\f0  \f1 word.
\par 
\par The following symbols match various types of patterns:
\par 
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 '.' match any character
\par {\pntext\f3\'B7\tab}'*' match zero or more occurrances of the preceding character
\par {\pntext\f3\'B7\tab}'+' match one or more occurrances of the preceding character
\par {\pntext\f3\'B7\tab}'?' match zero or one occurrances of the preceding character
\par {\pntext\f3\'B7\tab}'^' match the start of a line
\par {\pntext\f3\'B7\tab}'$' match the end of a line
\par {\pntext\f3\'B7\tab}'\\b' match the beginning or ending of a word
\par {\pntext\f3\'B7\tab}'\\B' match within a word
\par {\pntext\f3\'B7\tab}'\\w' match the beginning of a word
\par {\pntext\f3\'B7\tab}'\\W' match the ending of a word
\par {\pntext\f3\'B7\tab}'\\\f0 <\f1 ' match the beginning, ending, or inside a word
\par {\pntext\f3\'B7\tab}'\\\f0 >\f1 ' match anything other than the beginning, ending, or\f0  \f1 inside a word
\par \pard 
\par Some of the basic pattern matching such as '+' can match multiple\f0  \f1 occurances of a character. Range Matching is a more general\f0  \f1 statement on the number of occurances of a character, for example you\f0   \f1 can match a bounded range, say from two to four 'a' characters by doing\f0  \f1 the following:
\par 
\par \f2 OGrep "a\\\{2,4\\\}" *.c 
\par 
\par \f1 Brackets [] can be used to delimit a set of characters. Then the\f0  \f1 bracketed sequence will match any character in the set. For\f0  \f1 example
\par 
\par \f2 OGrep "[abc]" *.c\f1 
\par 
\par matches any of the characters a,b,c. A range of characters can be\f0  \f1 specified:
\par 
\par \f2 OGrep "[a-m]" *.c\f1 
\par 
\par matches any characters in the range a-m.
\par 
\par Set negation is possible:
\par 
\par \f2 OGrep "[^a-z]" *.c\f1 
\par 
\par matches anything but a lowercase letter.
\par 
\par Sets can be more complex:
\par 
\par \f2 OGrep "[A-Za-z0-9]" *.c\f1 
\par 
\par matches any alphanumeric value.
\par 
\par Sets can be combined with more basic pattern matching:
\par 
\par \f2 OGrep "[A-Z]?" *.c\f1 
\par 
\par matches zero or one upper case characters.
\par 
\par \f2 OGrep "[0-9]\\\{2,4\\\}"\f1 
\par 
\par matches from two to four digits.
\par 
\par Sometimes, it is desirable to match the same sub-pattern multiple times\f0  \f1 before having grep declare the pattern as a match for the text.\f0   \f1 In a\f0  \f1 simple case:
\par 
\par \f2 OGrep "\\(myword\\)|\\0" *.c 
\par 
\par \f1 matches the string:
\par 
\par myword|myword
\par 
\par In this case\f0  \f1 the quoted parenthesis surround the region to match, and the \\0 says\f0  \f1 match that region again. This is not a very interesting case, but\f0  \f1 when combined with other pattern matching it becomes more powerful,\f0  \f1 because the \\0 doesn't reapply the pattern but instead matches exactly\f0  \f1 the same pattern as before. For example to combine it with a set:
\par 
\par \f2 OGrep "\\([a-z]+\\)||\\0" *.c 
\par 
\par \f1 matches any lower-case word twice, as long as it is separated from\f0  \f1 itself by two | characters. This pattern would match 'ab||ab' but\f0  \f1 not 'ab||xy'.
\par 
\par Up to ten such regions may be specified in the pattern; to access them\f0  \f1 use \\0 \\1 \\2 ... \\9 where the digit gives the order the region is\f0  \f1 encountered within the pattern.
\par 
\par }
420
Scribble420
OLink




Writing



FALSE
37
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 OLink\cf0\b0\f1\fs20 
\par 
\par OLink takes the\f0  \f1 output files from compilers and assemblers, and merges them\f0  \f1 together. This merging is necessary because often a single\f0  \f1 source file will either declare 'global' code or data that is\f0  \f1 accessible from other source files, or reference such global data from\f0  \f1 another file. Each output file from a compiler or assembler will\f0  \f1 have a list of these kinds of declarations, and the linker has two\f0  \f1 tasks: to combine the actual code and data from different files,\f0  \f1 and to resolve these global references between files.
\par 
\par \f0 The end result of linking in most systems is to generate an executable file, however olink generates a fully linked object file (or sometimes a partially linked file) and generation of the executable is deferred to link post processor.  The link post processor for Windows 32 bit programs is \cf2\strike DLPE.EXE\cf3\strike0\{linkID=540\}\cf0 .\f1 
\par 
\par When it comes to the actual data being combined, the compiler or\f0  \f1 assembler will organize the code and data into sections. Each\f0  \f1 section has a name, and this name is one of the criteria used in\f0  \f1 combining section data. For example a code section might contain\f0  \f1 the code fragments from the file, an initialized data section might\f0  \f1 hold initialized data, and an\f0  u\f1 ninitialized data section might reserve\f0  \f1 space for uninitialized data. In compiler output, other sections\f0  \f1 might occur for example to hold constant data, string data, or control\f0  \f1 information. An assembly program can directly control the\f0  \f1 segmentation of code and data into sections, and any number of\f0  \f1 arbitrary sections may appear according to the needs of the program.
\par 
\par It should be stressed that OLink does not generate actual ROM images or\f0  \f1 executable files; all it does is combine code and data, and resolve the\f0  \f1 global references. Another post-processing program \f0 such as DLHEX or DLPE \f1 is used to\f0  \f1 generate the ROM image or executable file, based on the output of \f0 OLink.\f1 
\par 
\par \f0 The output of \f1 OLink can beguided through use of a \cf2\strike specification\f0  f\f1 ile\cf3\strike0\{linkID=440\}\cf0\f0  \f1 and through command-line defines. The specification file\f0  \f1 indicates the ordering and grouping of sections, and gives default\f0  \f1 addresses and other attributes to the groupings. Command-line\f0  \f1 defines can be used to taylor the specifics of how the specification\f0  \f1 file is used; in OLink the command line defines generally act in terms\f0  \f1 of giving an absolute address to a variable which has been referenced\f0  \f1 elsewhere.
\par 
\par The general\f0  \f1 form of an OLink \cf2\strike Command Line\cf3\strike0\{linkID=430\}\cf0  is:
\par 
\par \f2 OLink [options] filename-list
\par 
\par \f1 Where filename-list gives a\f0  \f1 list of files to merge together.\f0   \f1 OLink natively understands\f0  \f1 files of the extension .o and .l, when\f0  these are \f1 generated by other tools in this package. Generally it will pass\f0  \f1 other\f0  \f1 files specified on the command line t\f0 o a post-processing program for \f1 further analysis. For example .res (windows resource) files\f0  \f1 are passed on to DLPE to help it build the executable.
\par 
\par Olink takes the files in\f0  \f1 filename-list, and produces an\f0  \f1 output file\f0  \f1 .rel extension.\f0   The .rel files have the same format as the object files generated by the compiler or assembler, but are partially or fully linked.\f1 
\par 
\par \cf2\strike Specification files\cf3\strike0\{linkID=440\}\cf0\f0  \f1 give a flexible method for specifying how to merge sections from the\f0  \f1 various input files. They can specify what code and data be\f0  \f1 combined\f0  \f1 together, in what order, and what the address of code and\f0  \f1 data should be. A specification file uses three basic constructs,\f0  \f1 and\f0  \f1 each construct can be further clarified with attributes. 
\par 
\par At the\f0  \f1 top level there can be one or more Partitions.\f0   \f1 Each\f0  \f1 partition \f0 may\f1  be relocated independently of other partitions. \f0  
\par 
\par \f1 A\f0  \f1 partition contains one or more\f0  \f1 Overlays.\f0   \f1 The overlays are\f0  \f1 independendent units of code or data, which are overlayed onto a common\f0  \f1 region of memory. The overlay mechanism can be used for example\f0  \f1 in systems that need to use bank-switching to extend the amount of\f0  \f1 memory available. 
\par 
\par An overlay contains Regions,\f0  \f1 which simply\f0  \f1 specify the names of sections that should be combined together.\f0   \f1 The regions can be actual section names, or expressions containing
\par section names. Normally a region would contain all sections\f0  \f1 matching the section name from\f0  \f1 all input files, but, a Region can be further clarified with a list of\f0  \f1 files that should be considered for inclusion. In this way\f0  \f1 different files with the same section can be combined into different\f0  \f1 overlays.
\par 
\par \cf2\strike Target configurations\cf3\strike0\{linkID=450\}\f0  \cf0\f1 specify the\f0  \f1 default mechanism for taking the linker output and creating a ROM image\f0  \f1 or executable file. Each target configuration specifies a linker
\par specification file, a list of default definitions, and the name of a\f0  \f1 post-processing program such as DLHex\f0  \f1 to run to create the final output\f0  \f1 file. The specification files used with default target\f0  \f1 configurations are generic in nature, and make\f0  \f1 certain assumptions about the program; however some of the identifiers\f0  \f1 in such specification filse may refer to definitions\f0  \f1 made\f0  \f1 elsewhere. Those definitions are generally part of the target\f0  \f1 configuration, and may be modified through command-line options to\f0  \f1 make minor changes to the configuration. 
\par 
\par For example, in WIN32\f0  \f1 DLLs, the \f0 target file specifies the \f1 base address of the DLL \f0 in terms of a linker define statement, which \f1 may be redefined via the command line\f0  /D define switch \f1 to not collide with other DLLs and thus improve load time.
\par 
\par }
430
Scribble430
Command Line




Writing



FALSE
96
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Command Line\cf0\b0\f1\fs20 
\par 
\par The general format of an OLink command\f0  \f1 line is:
\par 
\par \f2 OLink [options] file-list\f1 
\par 
\par where file-list is an arbitrary list of\f0  \f1 input files.
\par 
\par For example:
\par \f2 
\par OLink one.o two.o three.o\f1 
\par 
\par links several object\f0  \f1 files and makes an output file called one.rel.
\par 
\par By default\f0  \f1 OLink takes the name of the\f0  \f1 first file on the command line, and\f0  \f1 replaces it\f0 s extension\f1  with the extension .rel,to decide on an output file name.
\par 
\par The file list can have wildcards:
\par 
\par \f2 OLink /otest.rel *.o\f1 
\par 
\par links all the files in the curent directory, and the results are\f0  \f1 put in test.rel because the /o command line switch is specified.
\par 
\par 
\par Response files\f0  \f1 can be used as an alternate to specifying input on the command\f0  \f1 line. For example:
\par 
\par \f2 OLink @myresp.lst\f1 
\par 
\par will take command line options from\f0  'myresp.lst'.  \f1 This is useful for example when there are many\f0  \f1 .o o\f0 r \f1 .l files being\f0  \f1 merged and it is not convenient to specify them on the command line.
\par 
\par \f0 Following is a list of the command line switches OLink supports.\f1 
\par 
\par \pard\fi-700\li700\f0 -c case sensitivity
\par \f1 
\par \f0\tab\f1 By default OLink treats\f0  \f1 symbols as case-insensitive. If the -c+\f0  \f1 switch is specified on the command line, labels will be treated as case\f0  \f1 sensitive. This supports certain language compilers that do allow\f0  \f1 the user to type the same word in different case, and have it mean\f0  \f1 different things.
\par 
\par \b\f0 
\par -opath\b0  Specifying the outpuf tile name
\par \f1 
\par \f0\tab\f1 By default,\f0  \f1 OLink will take the first input\f0  \f1 file name, and replace \f0 it's\f1  extension\f0  \f1 with the\f0  \f1 extension .rel to create the\f0  \f1 name of the output file.\f0  \f1 However in some cases it is useful to be able to specify\f0  \f1 the output\f0  \f1 file name. The specified name can have its extension specified,\f0  \f1 or it\f0  \f1 can be typed without an extension to allow OLink to add the .rel\f0  \f1 extension.
\par 
\par \f2\tab OLink /r+ /omyfile test1.o test2.o test3.o cl.l
\par 
\par \f0\tab\f1 makes an object file called myfile.rel
\par 
\par \b\f0 -Lpath\b0  specify search path\f1 
\par \f0\tab\f1 By default,\f0  \f1 OLink will search for \f0 objecc and \f1 library\f0  \f1 files in the C Compiler library file\f0  \f1 path, and in the current directory. T\f0 his\f1  option may be used to\f0  \f1 specify additional directories to search for lib files:
\par 
\par \f2\tab OLink /L..\\mylibs test.o floating.l \f1 
\par 
\par \f0\tab\f1 will find floating.l either\f0  \f1 in the C compiler library directory, the\f0  \f1 current directory, or the directory ..\\mylibs.
\par 
\par \f2 -Ddefine=value\f1 
\par 
\par \f0\tab\f1 OLink allows definition of\f0  \f1 global variables on the command line, and gives them an\f0  \f1 absolute address. This facility can be used either to set the\f0  \f1 location of a function or data, or to provide a constant value to a\f0  \cf2\strike\f1 specification file\cf3\strike0\{linkID=440\}\f0  \cf0\f1 entry or \f0 even change a constant in \f1 user code.
\par 
\par \f0\tab\f1 For example
\par 
\par \f2\tab OLink /DRAM=0x80000000 myprog.o
\par 
\par \f0\tab\f1 Defines a global variable RAM which has the address 80000000 hex.\f0  \f1 This variable may be accessed externally from either the program code,\f0  \f1 or the specification file. It might be used for example to\f0  \f1 relocate code or data, to specify the address of some hardware, \f0 to set a a link-time constant into the program, \f1 and so\f0  \f1 forth.
\par 
\par \f0\tab\f1 Decimal values for addresses may be provided by omitting the\f0  \f1 0x prefix. 
\par 
\par \b\f0 /r+\b0  perform complete link\f1 
\par 
\par \f0\tab\f1 By default,\f0  \f1 OLink performs a partial\f0  \f1 link. In a partial link,\f0  \f1 some global\f0  \f1 definitions may remain unresolved, and libraries are ignored. The\f0  \f1 output file may be further linked with more object files or with the\f0  \f1 result of other partial links. However, a partial link cannot be\f0  \f1 used to generate a rom image or executable file, because not all the\f0  \f1 information required for such binaries has been generated; specifically\f0  \f1 there may be some addresses that haven't been defined yet.
\par 
\par \f0\tab\f1 Usually, a complete link happens automatically when the /T switch is\f0  \f1 used to specify a \cf2\strike target\f0  configuration\cf3\strike0\{linkID=450\}\cf0 .  \f1 However in some cases it is desirable to indicate to the\f0  \f1 linker that a complete link is desired without specifying a target\f0  \f1 configuration. Use /r+\f0  \f1 for this purpose.
\par 
\par \b\f0 -m\b0   generate map file\f1 
\par 
\par \f0\tab\f1 The map file name will\f0  \f1 be the same as the name of the .rel\f0  \f1 file, with the extension replaced\f0  \f1 by .map. The standard\f0  \f1 map file summarizes the partitions theprogram is contained in, and then lists publics in both alphabetic and\f0  \f1 numeric order. 
\par 
\par \f0\tab\f1 A more detailed map file may be obtained by using\f0  \f1 /mx. This gives a list of\f0  \f1 partitions, overlays, regions, and files that\f0  \f1 went into making up the program, in the order they were\f0  inc\f1 luded.\f0  \f1 t also includes details of the attributes used to place the sections.
\par 
\par \b\f0 -s\b0  - use specification file
\par 
\par \tab The \cf2\strike specification file\cf3\strike0\{linkID=440\}\cf0  gives the layout of the program as it will exist in memory or in an executable file.  Without a specification file, the linker will just order sections in the order it finds them, starting at address zero.  \f1 Sometimes a default\f0  \f1 specification\f0  \f1 file will be selected automatically\f0  when using \f1  the /T \f0  \cf2\strike\f1 target\f0  \f1 configuration\cf3\strike0\{linkID=450\}\cf0  switch. This specification file\f0  \f1 may be overridden with the /s\f0  \f1 switch; or if the /T switch is\f0  \f1 not\f0  \f1 specified the /s switch may be\f0  \f1 used to select a specification\f0  \f1 file. For example:
\par 
\par \f0\tab\f1 OLink\f0  \f1 /smyprog.spc myprog.o
\par 
\par \b\f0 -T target configuration\f1 
\par \b0\f0\tab\f1 The \cf2\strike target\f0  \f1 configuration\cf3\strike0\{linkID=450\}\cf0  switch specifies that a specific profile be used to\f0  \f1 build\f0  \f1 the target. The profile includes a specification file,\f0  \f1 default\f0  \f1 definitions, a post-processing program to run, and some other\f0  \f1 information. The link will be done with the givenparameters, and then the post-processing program will be run to\f0  \f1 generate the final ROM image or executable file.
\par 
\par \f0\tab\f1 For example:
\par 
\par \f0\tab\f2 OLink /T:M3 myprog.o\f1 
\par 
\par \f0\tab\f1 selects the specification profile that builds a Motorola Hex file with\f0  \f1 four-byte addresses.
\par 
\par \b\f0 -l\b0  link only
\par \tab Sometimes it is convenient to use the /T switch but it isn't necessary to go on to call the post processing program to generate a ROM image or executable.  This switch stops the linker after the .rel file is generated.\f1 
\par 
\par }
440
Scribble440
Specification Files




Writing



FALSE
265
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Specification Files\cf0\b0\f1\fs20 
\par 
\par A specification file indicates the combination and ordering of sections\f0  \f1 of code and data. The\f0  \f1 specification file consists of one or more \f0  \f1 Partitions, which are\f0  \f1 independent units of code and/or data. 
\par 
\par Each partition holds one\f0  \f1 or more Overlays.\f0  \f1 Overlays are units of code and/or data which\f0  \f1 may share the same location in memory; for example a processor with a\f0  \f1 small memory address can use bank switching to move different units of\f0  \f1 code in and out of the address range as necessary. These\f0  \f1 different units of code would however all share the same address, so\f0  \f1 the overlay mechanism gives a way of relocating multiple units of code\f0  \f1 to the same address.
\par 
\par Each overlay holds one or more Regions;\f0  \f1 a region is what specifies\f0  \f1 which sections get combined. For example the region code takes\f0  \f1 all sections named code from\f0  \f1 the input file and \f0 c\f1 oncatenates them\f0  \f1 together. Multiple regions can be concatenated one after another\f0  \f1 within an overlay. To support the overlay mechanism, each region\f0  \f1 may further be qualified with file names, so that you can specify that\f0  \f1 sections named code from one\f0  \f1 group of files go in one region, and\f0  \f1 sections named code from\f0  \f1 another group of files go in another\f0  \f1 region. These regions could be placed in different overlays to\f0  \f1 help with things like the bank-switch mechanism indicated above.
\par 
\par Partitions, overlays, and regions can be given attributes to specify\f0  \f1 things like an absolute address, a maximum size, alignment, and so\f0  \f1 forth.
\par 
\par You can also define labels within partitions and overlays similar to\f0  \f1 assignment statements in a high-level-language. Each label has an\f0  \f1 associated\f0  \f1 expression, which is calculated and then used as a value for the\f0  \f1 label. These values become globals to the linker, and are \f0 tr\f1 eated\f0  \f1 the\f0  \f1 same as the address obtained when declaring a global variable in the\f0  \f1 assembler and compiler. Code in the object files can referencethese labels as if they were externals. 
\par 
\par Further, expressions used\f0  \f1 in defining a label or attribute could use another label which is not\f0  \f1 defined in the specification file; this might be defined for examplesomewhere in the code, or in a command-line definition.\f0   \f1 It is\f0  \f1 especially useful to define such labels in a command line definition,\f0  \f1 as a way to customize the specification file without rewriting\f0  \f1 it. For example, if two peices of hardware share the same source\f0  \f1 code but are linked at different base addresses, one might write a\f0  \f1 single linker specification file, referencing the base address as a\f0  \f1 label. Then a linker command-line definition could be used to\f0  \f1 resolve the specific address the code is linked for.
\par 
\par The following will be used as an example:
\par 
\par \f2 partition
\par \{
\par \tab overlay \{
\par \tab\tab region \{\} data [align=4];
\par \tab\tab region \{\} bss.
\par \tab\} RAM [addr=0x0000, size=0x4000];
\par \} DATA;
\par 
\par partition
\par \{
\par \tab overlay \{
\par \tab\tab  region \{\} code;
\par \tab\tab  region \{\} const;
\par \tab\} ROM;
\par \} CODE [addr=0xf000, size=0x1000];
\par 
\par \f1 This\f0  \f1 defines two partitions, in this case one is for data nad one is for\f0  \f1 code. The first partition is named DATA and consists of two\f0  \f1 groups of sections.  First all sections named data are concatenated\f0  \f1 together, then all sections named bss\f0  \f1 follow after that. This partition is\f0  \f1 defined with attributes to start address 0, and extend for 16K.\f0   \f1 If the actual size of the partition is greater than 16K, an error will\f0  \f1 be generated. In this case the overlay is named RAM; this overlay\f0  \f1 name is what is visible to ROM-generation tools such as DLHEX.
\par 
\par The second partition is named CODE and also consists of two groups of\f0  \f1 sections; first all sections named codeare concatenated together,\f0  \f1 followed by all sections named const.\f0  \f1 This partition starts at\f0  \f1 address 0xf000, and extends for 4K. In this case the overlay name\f0  \f1 visible to DLHEX or other\f0  \f1 executable-generation tools is ROM.
\par 
\par In the first partition, an align=4\f0  \f1 attribute is declared on the data\f0  \f1 region. This means that each data section put into the region\f0  \f1 will be aligned\f0  \f1 on a four-byte boundary when it is loaded from its corresponding\f0  \f1 file. (Note: if\f0  \f1 assembly language code specifies a more stringent bound such as align =\f0  \f1 8, that will be used instead).
\par 
\par In the following:
\par 
\par \f2 partition
\par \{
\par \tab overlay \{
\par \tab\tab  region \{ bank1a.o bank1b.o bank1c.o \} code;
\par \tab\} BANK1;
\par 
\par \tab overlay \{
\par \tab\tab  region \{ bank2a.o bank2b.o bank2c.o \} code;
\par \tab\} BANK2;
\par 
\par \tab overlay \{
\par \tab\tab  region \{ bank3a.o bank3b.o bank3c.o \} code;
\par \tab\} BANK3;
\par \} CODE [addr = 0xe000, size = 0x1000];
\par \f1 
\par Three banks of code have been defined, each of which starts at address\f0  \f1 0xe000 and extends for 4K. Each region references sections named\f0  \f1 code, however, file names are\f0  \f1 specifically\f0  s\f1 pecified for each region,\f0   \f1 so that only code sections\f0  \f1 from specific files will be included while\f0  \f1 processing the region. For example in the overlay BANK1, only\f0  \f1 files bank1a.o,\f0  \f1 bank1b.o, and\f0  \f1 bank1c.o will contributed to the\f0  \f1 contents\f0  \f1 of the region.
\par 
\par Wildcards may be used in the file specification, so that the above\f0  \f1 could be written:
\par 
\par \f2 partition
\par \{
\par \tab overlay \{
\par 
\par \tab\tab  region \{ bank1*.o \} code;
\par \tab\} BANK1;
\par 
\par \tab overlay \{
\par 
\par \tab\tab  region \{ bank2*.o \} code;
\par \tab\} BANK2;
\par  
\par \tab overlay \{
\par 
\par \tab\tab  region \{ bank3*.o \} code;
\par \tab\} BANK3;
\par \} CODE [addr= 0xe000, size = 0x1000];
\par 
\par \f1 In the following:
\par 
\par \f2 partition
\par \{
\par \tab overlay \{
\par 
\par \tab\tab  RAMSTART=$
\par \tab\tab  region \{\} data [align=4];
\par \tab\tab  style="font-family: Courier New,Courier,monospace;">
\par 
\par \tab\tab  region \{\} bss;
\par \tab\tab   RAMEND=$
\par \tab  \} RAM [addr=0x0000, size=0x4000];
\par \} DATA;
\par 
\par \f1 The labels\f0  \f1 RAMSTART and RAMEND have been defined. The '$' in the expression\f0  \f1 indicates to use the address at the location the label is specified, so\f0  \f1 these definitions effectively define labels at the beginning and ending\f0  \f1 of the overlay. As indicated before these define global\f0  \f1 variables, so an x86 assembler program such as the following could be\f0  \f1 used to set all data in these regions to zero:
\par 
\par \f2 extern RAMSTART, RAMEND
\par mov edi, RAMSTART
\par mov ecx,RAMEND-RAMSTART
\par mov al, 0
\par cld
\par rep stosb
\par 
\par \f1 Expressions may be more complex, consisting of add, subtract, multiply,divide and parenthesis. As a simple example the above example can\f0  \f1 be rewritten to define a size:
\par 
\par \f2 partition
\par \{
\par \tab overlay \{
\par 
\par \tab\tab  RAMSTART=$
\par \tab\tab  region \{\} data [align=4];
\par \tab\tab  region \{\} bss;
\par \tab\tab   RAMSIZE = $-RAMSTART
\par \tab  \} RAM [addr=0x0000, size=0x4000];
\par \} DATA;
\par 
\par \f1 
\par 
\par Labels or\f0  \f1 expressions may be used in attributes, for example:
\par 
\par \f2 partition
\par \{
\par \tab overlay \{
\par 
\par \tab\tab  RAMSTART=$
\par \tab\tab  region \{\} data [align=4];
\par \tab\tab  region \{\} bss.
\par \tab\tab   RAMSIZE = $-RAMSTART
\par \tab  \} RAM [addr=RAMBASE, size=0x4000];
\par \} DATA;
\par 
\par \f1 Here the base\f0  \f1 address is defined in terms of a label RAMBASE. But RAMBASE is\f0  \f1 not defined anywhere in the specification file, so it has to be pulled\f0  \f1 from the linker's table of globals. In this case we might defin\f0 e it\f1  on the linker command line as follows:
\par 
\par \f2 OLink /DRAMBASE=0x7000 /smyspec.spc ...
\par 
\par \f1 Labels don't have to include '$' in the expression, although it is\f0  \f1 often useful. For example:
\par 
\par \f2 MYLABEL=0x44000+2000 
\par 
\par \f1 is valid.
\par 
\par Note that when using target configurations, the default specificationfiles use these types of declarations, but the target configuration\f0  \f1 gives default values to use. For example the default value for\f0  \f1 RAMBASE in a hex file is 0x10000, when used with the default linker\f0  \f1 specification file that is used for binary and hex file output.\f0  \f1 But such values can be overridden on the command line; if it is\f0  \f1 desirable to use the default specification file but RAMBASE is 0x8000\f0  \f1 for the specific hardware in question one might use OLink as follows:\f0  \f1 
\par 
\par \f2 OLink /T:M3 /DRAMBASE=0x8000 ...
\par 
\par \f0 P\f1 artitions, overlays, and regions can be attributed with one or more\f0  \f1 attributes. The attributes are comma delimited, and enclosed in\f0  \f1 braces. They occur after the name of the partition or overlay, or\f0  \f1 after the\f0  \f1 section specified by a region.
\par 
\par The \f0 possible attributes are listed in Table 1\f1 
\par 
\par \cf2\{html=\f0 <center>\cf0\f1 
\par <table border="1" cellpadding="2" cellspacing="2">
\par   <tbody>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">Attribute</td>
\par       <td style="vertical-align: top;">Meaning</td>
\par       <td style="vertical-align: top;">Default V\f0 a\f1 lue for Partitions</td>
\par       <td style="vertical-align: top;">Default Value for Overlays</td>
\par       <td style="vertical-align: top;">Default Value for Regions</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">ADDR</td>
\par       <td style="vertical-align: top;">Address</td>
\par       <td style="vertical-align: top;">0, or end of previous partition</td>
\par       <td style="vertical-align: top;">partition address</td>
\par       <td style="vertical-align: top;">overlay address or end of\f0  \f1 previous region</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">SIZE</td>
\par       <td style="vertical-align: top;">Absolute size</td>
\par       <td style="vertical-align: top;">unassigned</td>
\par       <td style="vertical-align: top;">partition size</td>
\par       <td style="vertical-align: top;">unassigned</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">MAXSIZE</td>
\par       <td style="vertical-align: top;">absolute size may vary up to\f0  \f1 this amount</td>
\par       <td style="vertical-align: top;">unassigned</td>
\par       <td style="vertical-align: top;">partition maxsize</td>
\par       <td style="vertical-align: top;">unassigned</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">ROUNDSIZE</td>
\par       <td style="vertical-align: top;">absolute size may vary, but wil\f0 l \f1 be rounded up to the next multiple of this</td>
\par       <td style="vertical-align: top;">1</td>
\par       <td style="vertical-align: top;">partition roundsize</td>
\par       <td style="vertical-align: top;">unassigned</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">FILL</td>
\par       <td style="vertical-align: top;">fill va\f0 l\f1 ue used when absolute\f0  \f1 size does not match size of data included in region</td>
\par       <td style="vertical-align: top;">0</td>
\par       <td style="vertical-align: top;">partition fill</td>
\par       <td style="vertical-align: top;">overlay fill</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">ALIGN</td>
\par       <td style="vertical-align: top;">minimum alignment of region</td>
\par       <td style="vertical-align: top;">1</td>
\par       <td style="vertical-align: top;">partition alignment</td>
\par       <td style="vertical-align: top;">overlay alignment</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">VIRTUAL</td>
\par       <td style="vertical-align: top;">base address for linking the\f0  \f1 region, when base address does not match the ADDR attribute</td>
\par       <td style="vertical-align: top;">unassigned</td>
\par       <td style="vertical-align: top;">partition virt\f0 u\f1 al attribute</td>
\par       <td style="vertical-align: top;">virtual address of overlay, or\f0  \f1 end of previous region</td>
\par     </tr>
\par \cf2\}
\par \{html=\cf0 
\par   </tbody>
\par </table>
\par \cf2\}
\par \cf0\f0 Table 1.  List of attributes\cf2\f1 
\par \{html=\f0 </center>
\par \}\cf0\f1 
\par 
\par 
\par Usually the\f0  \f1 region statement is used to specify a specific section name such as\f0  \f1 code\f0 :\f1 
\par 
\par \f2 region \{ \} code;\f1 
\par 
\par But sometimes it is useful to be able to combine multiple sections in a\f0  \f1 single region with the or\f0  \f1 operator
\par 
\par \f2 region \{\} code | const;\f1 
\par 
\par However this is\f0  \f1 different making two regions, one for code\f0  \f1 and one for const. The\f0  \f1 difference is that in this case code and const regions may be\f0  \f1 intermixed; whereas in the other case all the code sections would be\f0  \f1 combined together, separately from all the const regions.
\par 
\par Wildcards may be used in the region name:
\par 
\par \f2 region \{\} code*\f1 
\par 
\par matches the sections name code1, code2, code123, and so forth.
\par 
\par And for example
\par 
\par \f2 region \{\} * \f1  
\par 
\par matches ALL sections. There are two wildcard characters: * matches a sequence of characters,\f0  \f1 whereas ? matches a single\f0  \f1 character.
\par 
\par Other times you want to do a catch all which gets all sections except\f0  \f1 for a select section or group of sections.
\par \f2 
\par region \{\} *& !(code*)
\par 
\par \f1 This uses the and operator and\f0  \f1 the not operator to select all\f0  \f1 sections which do not start with the four letters 'code'.
\par 
\par A region can be named with any potentially complex expression\f0  \f1 involving section names and these operators, to match various\f0  \f1 combinations of sections.
\par 
\par }
450
Scribble450
Target Configurations




Writing



FALSE
208
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Target Configurations\cf0\b0\f1\fs20 
\par 
\par 
\par OLink has several default target configurations, that associate the\f0  \f1 various data needed for \f0 creating linker output for a particular type of output together\f1 . Each target configuration includes a linker\f0  \cf2\strike\f1 specification file\cf3\strike0\{linkID=440\}\cf0 ,\f0  \f1 default definitions for items used but not declared\f0  \f1 in the specification file, and a reference to a post-processing tool\f0  \f1 that will take an image linked against the specification file and\f0  \f1 generate some final binary image, such as a ROM image or an Operating\f0  \f1 System executable. 
\par 
\par Each target configuration is accessible via the /T linker switch.
\par 
\par For example:
\par 
\par \f2 OLink /T:BIN test.o
\par 
\par \f1 invokes the target configuration associated with the name BIN. In\f0  \f1 the case of BIN the file is linked into three partitions; code, data\f0  \f1 and stack using the specification file hex.spc;
\par and the results are\f0  \f1 dumped to a binary file using DLHex.
\par 
\par The remainder of this section will discuss the default target\f0  \f1 configurations\f0  rather than the mechanism for defining them.\f1 
\par 
\par There are several\f0  \f1 output file formats for generating a rom-based image. However,\f0  \f1 they all use a common specification file and post-processing\f0  \f1 tool. This section will briefly touch on the available output\f0  \f1 formats then touch on the specification file in more detail.
\par 
\par The available output formats in this mode are:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 /T:M1 Motorola srecord file format, 2 byte addresses
\par {\pntext\f3\'B7\tab}/T:M2 Motorola srecord file format, 3 byte addresses
\par {\pntext\f3\'B7\tab}/T:M3 Motorola srecord file format, 4 byte addresses
\par {\pntext\f3\'B7\tab}/T:I1 Intel hex file format, 16 bits
\par {\pntext\f3\'B7\tab}/T:I2 Intel hex file format, segmented
\par {\pntext\f3\'B7\tab}/T:I4 Intel hex file format, 32 bits
\par {\pntext\f3\'B7\tab}/T:BIN Binary file format
\par {\pntext\f3\'B7\tab}
\par \pard 
\par The default specification file for these output formats is hex.spc, and\f0  \f1 the default post-processing tool is dlhex.exe.
\par 
\par Hex.spc has 4\f0  \f1 independent overlays for code and data. Table 1 lists the\f0  \f1 overlays, the section names that are recognized in each overlay.\f0  \f1 It also lists an identifier that can be used with the /D command line\f0  \f1 switch\f0  \f1 to the linker to adjust base addresses, and the default base address\f0  \f1 for each overlay.
\par 
\par \cf3\{html=\cf0\f0 <center>\f1 
\par <table border="1" cellpadding="2"\f0  \f1  cellspacing="2">
\par   <tbody>
\par \cf3\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top; font-weight: bold;">Overlay</td>
\par       <td style="vertical-align: top; font-weight: bold;">Sections</td>
\par       <td style="vertical-align: top; font-weight: bold;">Base Address\f0  \f1 Identifier</td>
\par       <td style="vertical-align: top; font-weight: bold;">Base Address</td>
\par     </tr>
\par \cf3\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">RESET</td>
\par       <td style="vertical-align: top;">reset</td>
\par       <td style="vertical-align: top;">RESETBASE</td>
\par       <td style="vertical-align: top;">0x00000</td>
\par     </tr>
\par \cf3\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">ROM</td>
\par       <td style="vertical-align: top;">code, const, string</td>
\par       <td style="vertical-align: top;">CODEBASE</td>
\par       <td style="vertical-align: top;">0x00008</td>
\par     </tr>
\par \cf3\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">RAM</td>
\par       <td style="vertical-align: top;">data, bss</td>
\par       <td style="vertical-align: top;">RAMBASE</td>
\par       <td style="vertical-align: top;">0x10000</td>
\par     </tr>
\par \cf3\}
\par \{html=
\par \cf0     <tr>
\par       <td style="vertical-align: top;">STACK</td>
\par       <td style="vertical-align: top;">stack</td>
\par       <td style="vertical-align: top;">STACKBASE (size = STACKSIZE)</td>
\par       <td style="vertical-align: top;">0x20000 ( 0x400)</td>
\par     </tr>
\par \cf3\}
\par \{html=
\par \cf0   </tbody>
\par </table>
\par \cf3\}
\par \cf0 
\par Table 1 - Hex.spc details
\par \cf3\{html=\cf0\f0 </center>\cf3 
\par \}\f1 
\par \cf0 
\par 
\par Several types of WIN32 images may be\f0  \f1 generated. These include:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 /T:CON32 - console application
\par {\pntext\f3\'B7\tab}/T:GUI32 - windowing application
\par {\pntext\f3\'B7\tab}/T:DLL32 - dll application
\par \pard 
\par The default specification file for these output formats is pe.spc, and\f0  \f1 the default post-processing tool is dlpe.exe.\f0  \f1 PE.spc has two\f0  \f1 independent overlays for code and data. Table 2 lists the
\par overlays, and the section names that are recognized in each\f0  \f1 overlay. Table 3 lists the various values supported by \f0 p\f1 e.spc and \f0  \f1 dlpe.exe that may be adjusted on the\f0  \f1 linker command line.
\par 
\par 
\par \cf3\{html=\cf0\f0 <center>\cf3\f1 
\par \cf0 <table\f0  \f1 border="1" cellpadding="2"\f0  \f1  cellspacing="2">
\par   <tbody>
\par \cf3\f0\}\f1 
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top; font-weight: bold;">Overlay</td>
\par       <td style="vertical-align: top; font-weight: bold;">Sections</td>
\par     </tr>
\par \cf3\f0\}\f1 
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">.text</td>
\par       <td style="vertical-align: top;">code, const</td>
\par     </tr>
\par \cf3\f0\}\f1 
\par \{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">.data</td>
\par       <td style="vertical-align: top;">data, string, bss</td>
\par     </tr>
\par \cf3\f0\}\f1 
\par \{html=\cf0 
\par   </tbody>
\par </table>
\par \cf3\f0\}\f1 
\par \cf0 Table 2 - PE.SPC overlays
\par \cf3\{html=\cf0\f0 </center>\cf3 
\par \}\cf0\f1 
\par 
\par 
\par \cf3\{html=\cf0\f0 <center>\cf3 
\par \cf0\f1 <table border="1" cellpadding="2" cellspacing="2">
\par   <tbody>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">Definition</td>
\par       <td style="vertical-align: top;">Meaning</td>
\par       <td style="vertical-align: top;">Default</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">FILEALIGN</td>
\par       <td style="vertical-align: top;">Object Alignment within an\f0  \f1 executable file</td>
\par       <td style="vertical-align: top;">0x200</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">HEAP\f0 C\f1 OMMIT</td>
\par       <td style="vertical-align: top;">Amount of local heap to commit\f0  \f1 at program start</td>
\par       <td style="vertical-align: top;">0</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">HEAPSIZE</td>
\par       <td style="vertical-align: top;">Size of loc\f0 a\f1 l heap</td>
\par       <td style="vertical-align: top;">0x100000</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">IMAGEBASE</td>
\par       <td style="vertical-align: top;">Base address for the image (used\f0  \f1 to resolve DLL Address collisions)</td>
\par       <td style="vertical-align: top;">0x400000</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">OBJECTALIGN</td>
\par       <td style="vertical-align: top;">Object alignme\f0 d \f1 t in memory</td>
\par       <td style="vertical-align: top;">0x1000</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">STACKCOMMIT</td>
\par       <td style="vertical-align: top;">Amount of stack to commit at\f0  \f1 program start</td>
\par       <td style="vertical-align: top;">0x2000</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par     <tr>
\par       <td style="vertical-align: top;">STACKSIZE</td>
\par       <td style="vertical-align: top;">Size of stack for default thread</td>
\par       <td style="vertical-align: top;">0x100000</td>
\par     </tr>
\par \cf3\f0\}\cf0\f1 
\par \cf3\{html=\cf0 
\par   </tbody>
\par </table>
\par \cf3\f0\}\cf0\f1 
\par Table 3 - PE.SPC adjustable parameters
\par \cf3\{html=\cf0\f0 </center>\f1 
\par \cf3\f0\}\cf0\f1 
\par 
\par 
\par 
\par 
\par 
\par 
\par 
\par >
\par 
\par 
\par }
460
Scribble460
OMake




Writing



FALSE
11
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 OMake\cf0\b0\f1\fs20 
\par 
\par OMake currently has no\f0  \f1 documentation. OMake is however very similar to GNU make\f0  and the GNU make documentation is a good start.  \f1  \f0 B\f1 ut it\f0  \f1 should be stressed that OMake is not GNU make and there may be\f0  \f1 incompatibilities.
\par 
\par One known incompatibility is that OMake is case sensitive, even though it is being\f0  \f1 hosted on MSDOS/Windows.  \f0 This is a problem when specifying file names if the file name is not spelled in exactly the same case the OS spells it.  
\par 
\par \f1 This may be fixed in a future version.
\par 
\par }
470
Scribble470
OCPP




Writing



FALSE
83
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 OCPP\cf0\b0\f1\fs20 
\par 
\par OCPP is an\f0  \f1 extended version of the traditional C language preprocessor. The\f0  \f1 extensions include support for \f0 C11 and \f1 C99,  It is beyond the
\par scope of this document to discuss the format of input files used with\f0  \f1 the preprocessor. See a discussion of the C language for further
\par details of use with the C language\f0 .\f1 
\par 
\par Note that OCPP is not quite\f0  \f1 the same as the preprocessor built into a C\f0  \f1 compiler. The C compiler is able to maintain a slightly more\f0  \f1 detailed context about the preprocessed text. In rare cases loss\f0  \f1 of this information will cause a file preprocessed with OCPP to not be\f0  \f1 compilable with any C compiler.
\par 
\par The general form of an OCPP command line is:
\par 
\par \f2 OCPP [options] file 
\par 
\par \f1 Here the file\f0  \f1 is the file to preprocess. (multiple files may be specified onthe command line if you choose).
\par 
\par OCPP has no\f0  \f1 mechanism for specifying the output file name; instead it takes the
\par input file, stri\f0 p\f1 s the extension, and writes a file with a '.i'\f0  \f1 extension to indicate preprocessor output.
\par 
\par There are\f0  \f1 several command line options that control how the preprocessing is\f0  \f1 done. These include the ability to enable extensions, the ability\f0  \f1 to set a path for include files, and options to define and undefine\f0  \f1 preprocessor variables.
\par 
\par \f0 Following is a list of the command line switches OCPP supports.
\par \f1 
\par \pard\fi-700\li700\b\f0 -A\b0  strict mode\f1 
\par 
\par \f0\tab\f1 By default,\f0  \f1 OCPP will perform as a C89\f0  \f1 version preprocessor, which is slightly\f0  \f1 looser than the standard. It can be tightened to meet the
\par standard with the /A switch.
\par 
\par \b\f0 -1\b0  C11 mode
\par 
\par \tab Puts OCPP into the C11 parsing mode
\par 
\par -9 C99 mode
\par \tab Puts OCPP into the C99 parsing mdoe\f1 
\par 
\par \f0 -I set include file path\f1 
\par \f0\tab\f1 By default,\f0  \f1 OCPP will use the C language\f0  \f1 system include path to search for include\f0  \f1 files specified in the source file. If there are other include\f0  \f1 paths OCPP\f0  \f1 should search, th\f0 is\f1  switch\f0  \f1 can be specified to have it search\f0  \f1 them. For example by default the statement:
\par 
\par \f0\tab\f1 #include\f0  <\f1 windows.h\f0 >\f1 
\par \f0 
\par \tab\f1 will search in\f0  \f1 the C language system include directory to find windows.h.
\par \f0\tab\f1 Whereas:
\par 
\par \f0\tab\f1 OCPP\f0  \f1 /I.\\include test.c
\par 
\par \f0\tab\f1 will create a file test.i, which will additionally search\f0  \f1 the path.\\include\f0  \f1 for any include files specified in preprocessor directives.
\par 
\par 
\par \b /E[+\f0 -\f1 ]nn\b0  error control
\par 
\par \f0\tab\f1 nn is the maximum number of errors before the\f0   \f1 compile fails; if + is specified extended warnings will be shown that\f0  \f1 are normally disabled by default. \f0 If - is specified warnings will be turned off.  \f1 For example:
\par 
\par \f2\tab OCC /E+44 myfile.c\f1 
\par 
\par \f0\tab\f1 enables extended warnings and limits the number of\f0   \f1 errors to 44. By default only 25 errors will be shown and then\f0  \f1 the compiler will abort\f0  and
\par \f2 
\par \tab OCC /E- myfile.c\f0 
\par 
\par \tab compiles myfile.c without displaying any warnings.\f1 
\par 
\par 
\par 
\par \pard OCPP has two switches useful\f0  \f1 for defining \f0 preprocessor macros\f1 . The first\f0  \f1 switch /D defines a\f0  macro\f1 . The second switch /U\f0  \f1 causes OCPP to\f0  \f1 never allow the specified variable to be defined.
\par 
\par \pard\fi-700\li700\f0\tab\f1 For example
\par 
\par \f0\tab\f1 OCPP\f0  \f1 /DMYINT=4 test.c
\par \f0 
\par \tab\f1 defines the variable MYINT and gives it a value of 4. Whereas
\par 
\par \f0\tab\f1 OCPP /UMYINT\f0  \f1 test.c
\par 
\par \f0\tab\f1 globally undefines MYINT in such a way that it cannot be defined while\f0  \f1 preprocessing the file.
\par 
\par \f0\tab\f1 A \f0 macro\f1  doesn't have to be defined with a value:
\par 
\par \f0\tab\f1 OCPP /DWIN32\f0  \f1 test.c
\par 
\par \f0\tab\f1 might be used to specify preprocessing based on the program looking for\f0  \f1 the word WIN32 in #ifdef\f0  \f1 statements\f0 .\f1 
\par 
\par 
\par 
\par }
480
Scribble480
OImpLib




Writing



FALSE
51
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 OImpLib\cf0\b0\f1\fs20 
\par 
\par OImpLib is a\f0  \f1 WIN32 import librarian, suitable for various operations regarding the\f0  \f1 import sections of DLLs. It can take input from one of\f0  \f1 several sources, and place output in one of several destinations.In its most basic format one could use it to take a .DEF file or .DLL\f0  \f1 file and construct an import library for use with the toolchain, but it\f0  \f1 can also be used to create a .DEF file or extract things from a library.
\par 
\par The general format of an OImpLib\f0  \f1 command line is:
\par 
\par \f2 OImpLib [options] source dest
\par 
\par \f1 where source\f0  \f1 and dest specify files to use, and further, by parsing the extensions\f0  \f1 of source and dest OImpLib is\f0  \f1 able to act in one of several modes
\par 
\par Response files\f0  \f1 can be used as an alternate to specifying input on the command\f0  \f1 line. For example:
\par 
\par \f2 OImpLib test.l @myresp.lst\f1 
\par 
\par will take command line options from myresp.lst.\f0   \f1 In general it isn't necessary to use response files with OImpLib as the\f0  \f1 amount of input required is minimal.
\par 
\par \f0 Following is a list of command line switches OImpLib supports.
\par \f1 
\par \pard\fi-720\li720\b\f0 -c-\b0   case insensitive import library\f1 
\par \f0\tab\f1 OImpLib will allow the creation\f0  \f1 of case insensitive libraries with th\f0 is switch \f1 however, in general\f0  \f1 it isn't a good idea to make a\f0  \f1 case-insensitive import library, as WIN32 export records found in\f0  \f1 DLLs are case-sensitive\f0 .
\par \f1 
\par \pard 
\par OImpLib will perform different\f0  \f1 operations depending on what the file\f0  \f1 extensions of the input files are. The output file is specified\f0  \f1 first, followed by one or more input files. The output file may\f0  \f1 be one of the following:\f0  \f1 
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}a library file
\par {\pntext\f3\'B7\tab}an object file
\par {\pntext\f3\'B7\tab}a .DEF file
\par \pard 
\par When the output file is a library file, the input file can be a list of\f0  \f1 object files, .DEF files, and .DLL files. The object files will\f0  \f1 be placed in the library, whereas the export sections of .DEF and .DLL\f0  \f1 files will be converted to object files that hold import records, and\f0  \f1 then placed in the library.
\par 
\par When the output file is an object file, a single input file can be\f0  \f1 either a .DEF or .DLL file. The exports from the input file will\f0  \f1 be placed in the output file.
\par 
\par When the output file is a .DEF file, the input file can be either a\f0  \f1 .DLL file or an object file. The exports in the .DLL file will be\f0  \f1 written to the .DEF file, or the import records in the object file will\f0  \f1 be converted to export records and written to the .DEF file. For\f0  \f1 example:
\par 
\par \f0\tab\f2 OImpLib test.l kernel32.dll\f1 
\par \f0 
\par \tab\f1 will make an import library holding the export definitions from\f0  \f1 kernel32.dll
\par 
\par \f2\tab OImpLib test.ld mydll.def\f1 
\par 
\par \f0\tab\f1 will make an import library containing the export definitions from mydll.def.
\par 
\par \f0\tab\f1 On the other hand:
\par 
\par \f2\tab OImpLib user32.def user32.dll
\par 
\par \f0\tab\f1 will create a definition file from the export records in user32.dll
\par 
\par }
490
Scribble490
OLib




Writing



FALSE
55
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 OLib\cf0\b0\f1\fs20 
\par 
\par OLib is an\f0  \f1 object file librarian. It is used to combine a group of object\f0  \f1 files into a single file, to make it easier to operate on the group of\f0  \f1 files. It is capable of adding, removing, and\f0  \f1 extracting object files from a static library.
\par 
\par The general format of an OLib\f0  \f1 command line is:
\par \f2 
\par OLib [options] library object-file-list\f1 
\par 
\par where library\f0  \f1 specifies the library, and object-file-list\f0  \f1 specifies the list of files\f0  \f1 to operate on.
\par 
\par For example:
\par 
\par \f2 OLib test.l + obj1.o obj2.o obj3.o obj4.o obj5.o\f1 
\par 
\par adds several object\f0  \f1 files to the library test.l.
\par 
\par The object file list can have wildcards:
\par 
\par \f2 OLib test.l + *.o\f1 
\par 
\par adds all the object files from the current directory.
\par 
\par \f0 
\par \f1 Response files\f0  \f1 can be used as an alternate to specifying input on the command\f0  \f1 line. For example:
\par 
\par \f2 OLib test.l @myresp.lst\f1 
\par 
\par will take command line options from myresp.lst.\f0   \f1 Response files might be used for example if a library is to be made out\f0  \f1 of dozens of object files, and they won't all fit on the OS command
\par line.
\par 
\par \f0 Following is a list of the command line switches OLib supports.\f1 
\par 
\par \pard\fi-720\li720\b\f0 /c-\b0  case insensitivity
\par 
\par \tab By default OLib makes case-sensitive libraries, but this switch will allow creation of a case-insensitive library.  \f1 In general\f0  \f1 you don't need to make a library case\f0  \f1 insensitive, as the linker will handle case insensitivity based on\f0  \f1 command line switches even if the library is case-sensitive.
\par 
\par \pard 
\par In a previous examples the '+' symbol was used to indicate that the\f0  \f1 following files should be added to the library. '+' doesn't have\f0  \f1 to be used in this case because the default is to add files to the\f0  \f1 library. But there are two other command line modifiers that can\f0  \f1 be used to extract files from the library and remove files from the\f0  \f1 library. These are '*' for extract and '-' for remove. Note
\par that '-' is also used for command line switches; to prevent it from\f0  \f1 being ambiguous it must be present with spaces on either side when\f0  \f1 used. The '+' and '-' and '*' can be mixed on the\f0  \f1 command line;\f0  \f1 files after one of these modifies will be processed according to that\f0  \f1 modifier until another modifier is encountered. For example:
\par 
\par \f2 OLib test.l * obj1.o\f1 
\par 
\par extracts obj1.o from the library and places it in the current\f0  \f1 directory, and 
\par 
\par \f2 OLib Test.l - obj2.o \f1 
\par 
\par removes obj2.o from the library and destroys it. As a more\f0  \f1 complex example:
\par 
\par \f2 OLib test.l + add1.o add2.o - rem1.o rem2.o * ext1.o
\par 
\par \f1 adds the files add1.o and add2.o, removes the files rem1.o and rem2.o,\f0  \f1 and extracts the file ext1.o. The order of the modifiers in this\f0  \f1 example is arbitrary, and modifiers can occur more than once on the\f0  \f1 command line or in the response file.
\par 
\par }
500
Scribble500
ORC




Writing



FALSE
52
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 ORC\cf0\b0\f1\fs20 
\par 
\par ORC is a windows resource\f0  \f1 compiler. It handles compilation of a\f0  \f1 file containing standard windows resources into a .RES file.\f0   \f1 The .RES file can be given to the linker or to DLPE for use in \f0 a\f1 dding\f0  \f1 resources to a windows executable file. The specification for the\f0  \f1 format of the input file is mostly beyond the\f0  \f1 scope of this document other than to say that the ORC program has a C
\par language preprocessor. This makes it possible to define\f0  \f1 symbolic constants.
\par 
\par The general format of an ORC\f0  \f1 command line is:
\par \f2 
\par ORC [options] rcfile.rc\f1 
\par 
\par where rcfile.rc is the resource file to compile.
\par 
\par \f0 At present ORC will not directly modify an EXE file but its output must be passed to the linker or post processing program.
\par \f1 
\par Response files\f0  \f1 can be used as an alternate to specifying input on the command\f0  \f1 line. For example:
\par 
\par \f2 ORC @myresp.lst \f1 
\par 
\par will take command line options from myresp.lst.\f0  \f1 In general it isn't necessary to use response files with\f0  \f1 ORC as the\f0  \f1 amount of input required is minimal.
\par 
\par \f0 Following is a list of command line switches ORC supports.\f1 
\par 
\par \pard\fi-720\li720\b\f0 /ipath\b0  set include file path\f1 
\par 
\par \f0\tab\f1 By default, ORC will use the C\f0  \f1 language header include path as an\f0  \f1 include path to search for files specified in preprocessor INCLUDE\f0  \f1 directives. For example the statement:
\par 
\par \f0\tab\f1 #include\f0  <\f1 windows.h\f0 >
\par \f1 
\par \f0\tab\f1 will result in windows.h being found in the compiler include directory,\f0  \f1 and included in the file. If there are other paths that should be\f0  \f1 searched, they may be specified on the command line with the /i\f0  \f1 switch. For example:
\par 
\par \f0\tab\f2 ORC /i.\\include test.rc 
\par 
\par \f0\tab\f1 Searches in the directory .\\include\f0  \f1 as well as in the C language\f0  \f1 include directory.
\par 
\par \f0\tab\f1 The ORC preprocessor defines\f0  \f1 the preprocessor symbol\f0  \f1 RC_INVOKED\f0  \f1 to allow include files to specify sections that won't be evaluated by\f0  \f1 ORC. For example the\f0  \f1 windows headers use this to prevent RC\f0  \f1 compilers from trying to parse structure definitions written in the C\f0  \f1 language. This way instructions to the RC compiler can be\f0  \f1 intermixed with instructions to the C compiler without causing ORC to\f0  \f1 process things it isn't capable of processing.
\par 
\par \b\f0 /Ddefine=value\b0 
\par 
\par \tab Defines preprocessor macros.  \f1 For example\f0 :\f1 
\par 
\par \f0\tab\f2 ORC /DMYINT=4 test.c\f1 
\par 
\par \f0\tab\f1 defines the \f0 macro\f1  MYINT and gives it a value of 4.
\par 
\par \f0\tab\f1 A \f0 macro\f1  doesn't have to be defined with a value:
\par 
\par \f2\tab ORC /DWIN32 test.c\f1 
\par 
\par \f0\tab\f1 might be used to specify preprocessing based on the program looking for\f0  \f1 the word WIN32 in #ifdef statements.
\par \pard 
\par }
510
Scribble510
DLHex




Writing



FALSE
120
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 DLHex\cf0\b0\f1\fs20 
\par 
\par DLHex is a \f0 linker \f1 postprocessor for\f0  \f1 obtaining hex and binary files of the type used in\f0  \f1 generatring rom-based images for embedded systems. It can be used\f0  \f1 indirectly as part\f0  \f1 of the link\f0  \f1 process, if the default linker settings are sufficient. In many\f0  \f1 cases though configuring the output for an embedded system will require\f0  \f1 a customized linker specification file. The linker documentation\f0   \f1 discusses this in more detail. If a customized specification file\f0  \f1 is used, it may be necessary to call DLHex\f0  \f1 directly to obtain an output\f0  \f1 file.
\par 
\par DLHex can\f0  \f1 produce output in one of several formats. These include 
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 Motorola\f0  \f1 S19 files
\par {\pntext\f3\'B7\tab}Intel Hex files
\par {\pntext\f3\'B7\tab}pure binary output format
\par \pard 
\par The\f0  \f1 S19 and Hex file formats are suitable for use with an EEProm burner or\f0  \f1 other device that can accept them; the binary format is available to\f0  \f1 make postprocessing the output for other\f0  \f1 types of requirements\f0  \f1 easier. In rare cases the binary format may be used directly,\f0  \f1 e.g. if a device has a file system and a suitable loader is written to\f0  \f1 load it into memory.
\par 
\par 
\par The general form of a DLHex\f0  \f1 command line is:
\par 
\par \f2 dlhex [options] relfile\f1 
\par 
\par Here the\f0  \f1 rel file is the linker\f0  \f1 generated file that holds the \f0 completely \f1 linked\f0  \f1 code.\f0  \f1 There are several command line options that control the output.\f0  \f1 These include options for specifying what parts of the input file to\f0  \f1 process, how to format the output, and optionally a file name to use\f0  \f1 for the output file.
\par 
\par The linker has a default linker specification file, which is used if\f0  \f1 the linker /T switch is used\f0  \f1 to run dlhex to create an outputfile. The script is as follows:
\par 
\par \f2 partition \{
\par \tab  overlay \{
\par \tab\tab  region \{\} reset [ size = RESETSIZE];
\par \tab  \} RESET;
\par \} pt0 [addr = RESETBASE];
\par partition \{
\par \tab  overlay \{
\par \tab\tab  region \{\} code [ align = 2];
\par \tab\tab  region \{\} const [ align = 4];
\par \tab\tab  region \{\} string [ align = 2];
\par \tab  \} ROM;
\par \} pt1 [addr=CODEBASE];
\par 
\par partition \{
\par \tab  overlay \{
\par \tab\tab  RAMDATA = $;
\par \tab\tab  region \{\} data [ align = 4];
\par \tab\tab  region \{\} bss [ align = 4];
\par \tab  \} RAM ;
\par \} pt2 [addr=RAMBASE];
\par 
\par partition \{
\par \tab  overlay \{
\par \tab\tab  region \{\} stack[size = $400];
\par \tab\tab  STACKPOINTER = $;
\par \tab  \} STACK;
\par \} pt3 [addr = STACKBASE];
\par \f1 
\par From the above you can see it has four\f0  \f1 overlay sections, which are named 'RESET','ROM', 'RAM' and\f0  \f1 'STACK'.\f0   \f1 In this case you may not want to extract the stack into an output file,
\par since an embedded system might initialize it in a loop. It may or\f0  \f1 may not be useful to extract the RAM section either, depending on\f0  \f1 whether the design of the embedded system specifies initialized\f0  \f1 data. Assuming it is useful, the /c\f0  \f1 switch can be used to extract\f0  \f1 the RESET, ROM and RAM files into a single .HEX file like this:
\par 
\par \f2 dlhex /cRESET,ROM,RAM /mM1 test.rel\f1 
\par 
\par or it can be used to extract the ROM and RAM sections into two files by\f0  \f1 running it twice like this:
\par 
\par \f2 dlhex /cROM /mM1 /orom.S19 test.rel
\par dlhex /cRAM /mM1 /oram.S19 test.rel
\par 
\par \f1 Note that the\f0  \f1 above discussions assumes use of default linker files; it is acceptable\f0  \f1 to use a customized linker specification file and name overlays
\par anything desirable.
\par 
\par 
\par \f0 Following is a list of command line switchs DLHex supports.
\par \f1 
\par \pard\fi-720\li720\b\f0 /c\b0  - specify what overlays to use\f1 
\par 
\par \f0\tab\f1 The input file\f0  \f1 is a linker generated .rel file. Encoded in the input file is the\f0  \f1 program text, separated into the overlays indicated in the linker\f0  \f1 specification script. Th\f0 is \f1 command line option is used to\f0  \f1 specify which of these overlays will be placed in the output\f0  \f1 file. Following it overlay names are specified, separated by a\f0  \f1 comma. For example:
\par 
\par \f2\tab dlhex /mM1 /cCODE;DATA test.rel
\par 
\par \f0\tab\f1 pulls the two\f0  \f1 overlays CODE and DATA from the test.rel file, and places the contents\f0  \f1 in a Motorola S19 file. By default, if no \f0 /\f1 c switch is specified,\f0  \f1 all overlays will be pulled from the .rel file in the order specified.
\par 
\par \b\f0 /oname\b0  - specify output file\f1 
\par \f0\tab\f1 By default,\f0  \f1 DLhex will take the input file name,\f0  \f1 and replace the extension with an\f0  \f1 extension indicating what type of output is being used. These\f0  \f1 extensions are as follows:
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-720\li720 BIN - a binary output file
\par {\pntext\f3\'B7\tab}S19 - a Motorola S19 file
\par {\pntext\f3\'B7\tab}HEX - an Intel Hex file
\par \pard\fi-720\li720   
\par \f0\tab\f1 However in some cases it is useful to be able to specify the output\f0  \f1 file name\f0  with this switch\f1 . The specified name can have its extension specified,or it can be typed without an extension to allow DLhex to add one of\f0  \f1 the default extensions. For example:
\par 
\par \f2\tab dlhex /mM1 /omyfile.dat test.rel\f1 
\par 
\par \f0\tab creates\f1  a Motorola S19 file and stores it in myfile.dat.
\par 
\par \f0\tab\f1 Whereas
\par \f2 
\par \tab dlhex /mM1 /omyfile test.rel\f1 
\par \f0 
\par \tab creates \f1  a Motorola S19 file and stores it in\f0  m\f1 yfile.S19.
\par 
\par \b\f0 /mxxx\b0  -- specify output file format\f1 
\par \f0  \tab\f1 DLHex supports several types of\f0  \f1 Motorola S19 output files, several\f0  \f1 types of Intel Hex output files, and a binary output file forma\f0 t\f1 . This switch\f0  \f1 can be followed by one of the following specifiers:
\par 
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-720\li720  M1 - Motorola S19 files with two byte address fields 
\par {\pntext\f3\'B7\tab} M2 - Motorola S19 files with three byte address fields
\par {\pntext\f3\'B7\tab} M3 - Motorola S19 files with four-byte address fields
\par {\pntext\f3\'B7\tab} I1 - 16 bit Intel hex file. Can be segmented or\f0  \f1 not depending on the input.
\par {\pntext\f3\'B7\tab} I2 - 16 bit Intel hex file. Starts with a\f0  \f1 segmentation record.
\par {\pntext\f3\'B7\tab} I4 - 32 bit Intel hex file.
\par {\pntext\f3\'B7\tab} B - Binary output format
\par \pard\fi-720\li720 
\par \f0\tab\f1 For practical purposes the I1 and I2 formats are the same, except the\f0  \f1 first record of an I2 file is guaranteed to be a segmentation record.
\par 
\par \f0\tab\f1 The default output format if no /m switch is specified is the binary\f0  \f1 format.
\par 
\par \b\f0 /p:xx\b0  - pad\f1 
\par \f0\tab\f1 By default DLHex does not pad\f0  \f1 output files. In the case of\f0  \f1 Motorola S19 and Intel Hex files there are address bytes in the output\f0  \f1 file, which means padding may be applied \f0  \f1 xternally if necessary, e.g.by an EPROM programmer. In the case of the binary format, no\f0  \f1 address bytes exist, and without padding the input files are copied\f0  \f1 directly to the binary output file one \f0 s\f1 ection at a time, without\f0  \f1 regard for the fact that it may be useful to align subsequent sections\f0  \f1 at the appropriate place relative to the first section. In other\f0  \f1 words, the default for the binary format is to create a file that has\f0  \f1 sections offset from each other based on their actual size, rather than\f0  \f1 based on the addressing information the linker has provided.
\par 
\par \f0\tab This switch may be used to specify that padding is required between sections.  the 'xx' value is a hexadecimal value used as the padding byte.\f1 
\par \f0\tab\f1 For example:
\par 
\par \f2\tab dlhex /p:FF test.rel
\par 
\par \pard\f1 
\par }
520
Scribble520
DLLE




Writing



FALSE
9
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 DLLE\cf0\b0\f1\fs20 
\par 
\par DLLE is the utility to make MSDOS\f0  \f1 executables, that aren't win32 compatible. Generally this means\f0  \f1 it creates LE or LX style executables, but there are other options as\f0  \f1 well.
\par 
\par DLLE is currently not documented.
\par 
\par }
530
Scribble530
DLMZ




Writing



FALSE
43
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 DLMZ\cf0\b0\f1\fs20 
\par 
\par 
\par DLMZ is the postprocessor used\f0  \f1 to create DOS 16-bit executables.There\f0  \f1 is quite a bit of linker defaults built around it; normally it will be\f0  \f1 called by the linker in response to use of a linker /T switch that\f0  \f1 specifies an MSDOS output format.. It would be rare for a user to\f0  \f1 need to call it directly.
\par 
\par The general form of a DLMZ\f0  \f1 command line is:
\par 
\par \f2 dlmz[options] relffile
\par 
\par \f1 Here the\f0  \f1 relfile is the linker\f0  \f1 generated file that holds the \f0 completely \f1 linked\f0  \f1 code. For example:
\par 
\par \f2 dlmz test.rel
\par 
\par \f1 makes a console application called\f0  \f1 test.exe\f0  \f1 from the linked code in\f0  \f1 test.rel.
\par 
\par \f0 The following is a list of the command line switches that DLMZ supports.\f1 
\par \pard\fi-720\li720\f0 
\par /oname - specify output name\f1 
\par \f0\tab\f1 By default, \f0 D\f1 LMZ will take the input file name,\f0  \f1 and replace the extension with the\f0  \f1 extension .EXE.
\par 
\par \f0\tab\f1 However in some cases it is useful to be able to specify\f0  \f1 the output\f0  \f1 file name\f0  with this switch\f1 . The specified name can have its extension specified,\f0  \f1 or it\f0  \f1 can be typed without an extension to allow dlhex to add one of the\f0  \f1 default extensions. For example:
\par 
\par \f0\tab\f1 dlmz /mREAL\f0  \f1 /omyfile test.rel
\par 
\par makes a segmented executable called myfile.exe
\par 
\par \f0 /mxxx - specify output type\f1 
\par \f0\tab\f1 DLMZ supports two types of MSDOS executables with the /m switch, as\f0  \f1 follows:
\par 
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-720\li720 TINY - a tiny-mode program  
\par {\pntext\f3\'B7\tab}REAL - a segmented program
\par \pard\fi-720\li720   
\par 
\par \f0\tab\f1 The default output format if no /m\f0  \f1 switch is specified is REAL.
\par 
\par \pard\f0 Note that t\f1 his toolchain\f0  \f1 isn't compatible with the normal MSDOS build process. Normally,MSDOS programs would have sections that also included something called\f0  \f1 a class name; the linker would take both the class name and the\f0  \f1 section name into account when determining how to create an output\f0  \f1 file. Class names aren't supported by this toolchain, instead it\f0  \f1 is preferred to write a linker specification file to specify how the\f0  \f1 sections should be combined. A generic linker specification file\f0  \f1 exists for each of the supported modes, however, since sections can be\f0  \f1 named and placed arbitrarily, this specification file would not work\f0  \f1 for all programs. It may be necessary to augment an MSDOS program\f0  \f1 with its own specific linker specification file. 
\par 
\par In tiny mode, it is customary to instruct most MSDOS assemblers to set\f0  \f1 a code origin of 100h. However, the linker specification file fortiny mode automatically sets this origin. Therefore it does not\f0  \f1 have to be present for this toolchain to generate tiny mode\f0  \f1 files. Such files still must start with a code sequence, however.
\par 
\par }
540
Scribble540
DLPE




Writing



FALSE
49
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}{\f3\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 DLPE\cf0\b0\f1\fs20 
\par 
\par DLPE is the postprocessor used\f0  \f1 to create Win32 executables. There\f0  \f1 is quite a bit of linker defaults built around it; normally it will be\f0  \f1 called by the linker in response to use of a linker /T switch that\f0  \f1 specifies a WIN32 output format.. It would be rare for a user to\f0  \f1 need to call it directly.
\par 
\par The general form of a DLPE\f0  \f1 command line is:
\par 
\par \f2 dlpe [options] relfile [resourcefile]\f1 
\par 
\par Here the\f0  \f1 relfile is the linker\f0  \f1 generated file that holds the linked\f0  \f1 code, and the optional resourcefile\f0  \f1 can be used to add resources to an\f0  \f1 executable.
\par 
\par Resource files can be specified on the command line and DLPE will build\f0  \f1 a resource section for the exectuble. For example:
\par 
\par \f2 dlpe test.rel test.res
\par \f1 
\par makes a console application called\f0  \f1 test.exe\f0  \f1 from the linked code in\f0  \f1 test.rel,\f0  \f1 using resources as specified in test.res.
\par 
\par \f0 Following is a list of command line switches DLPE supports.\f1 
\par 
\par \pard\fi-720\li720\b\f0 /oname \b0 - specify output name
\par \f1 
\par \f0\tab\f1 By default, DLPE will take the input file name,\f0  \f1 and replace the extension with the\f0  \f1 extension .EXE to form an output file name.\f0  \f1 However in some cases it is useful to be able to specify\f0  \f1 the output\f0  \f1 file name. The specified name can have its extension specified,\f0  \f1 or it\f0  \f1 can be typed without an extension to allow dlhex to add one of the\f0  \f1 default extensions. The output file name is specified with t\f0 his\f1  switch. For example:
\par 
\par \f0\tab\f1 dlpe /mGUI\f0  \f1 /omyfile test.rel
\par 
\par \f0\tab\f1 makes a windowing executable called\f0  \f1 myfile.exe.
\par 
\par \b\f0 /mxxx\b0  specify the output file format\f1 
\par \f0\tab\f1 DLPE supports several types of\f0  \f1 WIN32 Executables with the /m\f0  \f1 switch. the switch is followed by one of the folowing:
\par 
\par 
\par \pard{\pntext\f3\'B7\tab}{\*\pn\pnlvlblt\pnf3\pnindent0{\pntxtb\'B7}}\fi-720\li720 CON - a console application
\par {\pntext\f3\'B7\tab}GUI - a windowing application
\par {\pntext\f3\'B7\tab}DLL - a DLL
\par \pard\fi-720\li720   
\par 
\par \f0\tab\f1 The default output format if no /m\f0  \f1 switch is specified is the console\f0  \f1 format.
\par 
\par \b\f0 /sname\b0  set stub file\f1 
\par 
\par \f0\tab\f1 By\f0  \f1 default DLPE will add an MSDOS\f0  \f1 stub file which simply says that the\f0  \f1 program requires WIN32, if someone happens to run it on MSDOS.\f0   \f1 However, with the /s switch a\f0  \f1 specific stub can be specified.\f0   \f1 This might be useful for example with certain MSDOS DPMI extenders that\f0  \f1 mimic the WIN32 API for console mode programs. Th\f0 is switches adds a custom stub.  \f1  For example:
\par 
\par \f0\tab\f1 dlpe\f0  \f1 /smystub.exe test.rel
\par 
\par \f0\tab creates\f1  an output file test.exe\f0  \f1 which has the 16-bit program mystub.exe\f0  \f1 as its MSDOS stub.
\par 
\par 
\par }
0
0
0
66
1 General Topics
2 Welcome to Command Line Tools Help=Scribble10
2 Copyright=Scribble20
2 Tools Setup=Scribble15
1 OCC Compiler
2 OCC=Scribble30
2 Command Line=Scribble40
2 Output Control=Scribble50
2 Error Control=Scribble60
2 List File Control=Scribble70
2 Preprocessor File Control=Scribble80
2 Compilation Modes=Scribble90
2 Defining Macros=Scribble100
2 Specifying Include Paths=Scribble110
2 Translating Trigraphs=Scribble120
2 Code Generation Parameters=Scribble130
2 Optimizer Parameters=Scribble140
2 #Pragma statements=Scribble150
2 Compiler Extensions=Scribble160
1 OAsm Assembler
2 OAsm=Scribble170
2 Command Line=Scribble180
2 Directives
3 Directives=Scribble190
3 Data Definition Directives=Scribble199
3 Data Reservation Directives=Scribble210
3 Label Directives=Scribble220
3 Section Directives=Scribble230
3 EQU Directive=Scribble240
3 INCBIN Directive=Scribble250
3 TIMES Directive=Scribble260
3 Psuedo-structure Directives=Scribble270
2 Labels=Scribble280
2 Numbers and Expressions=Scribble290
2 Preprocessor
3 Preprocessor Directives=Scribble300
3 C-Style Preprocessor directives=Scribble310
3 Basic Extensions to C Preprocessor=Scribble320
3 Conditional Processing
4 Conditional Processing=Scribble330
4 Text Comparison Conditionals=Scribble340
4 Token Classification Conditionals=Scribble350
2 Context-Related Extensions=Scribble360
2 Multi-Line Macro Extensions=Scribble370
2 Repeat Block Extensions=Scribble380
1 OGrep Regular Expression Matcher
2 OGrep=Scribble390
2 Command Line=Scribble400
2 Regular Expressions=Scribble410
1 OLink Linker
2 OLink=Scribble420
2 Command Line=Scribble430
2 Specification Files=Scribble440
2 Target Configurations=Scribble450
1 OMake
2 OMake=Scribble460
1 Miscellaneous Utilities
2 OCPP=Scribble470
2 OImpLib=Scribble480
2 OLib=Scribble490
2 ORC=Scribble500
1 Link Post-Processors
2 DLHex=Scribble510
2 DLLE=Scribble520
2 DLMZ=Scribble530
2 DLPE=Scribble540
7
*InternetLink
16711680
Courier New
0
10
1
....
0
0
0
0
0
0
*ParagraphTitle
-16777208
Arial
0
11
1
B...
0
0
0
0
0
0
*PopupLink
-16777208
Arial
0
8
1
....
0
0
0
0
0
0
*PopupTopicTitle
16711680
Arial
0
10
1
B...
0
0
0
0
0
0
*TopicText
-16777208
Arial
0
10
1
....
0
0
0
0
0
0
*TopicTitle
16711680
Arial
0
16
1
B...
0
0
0
0
0
0
<new macro>
-16777208
Arial
0
10
0
....
0
0
0
0
0
0
