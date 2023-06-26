OrangeC
=======

OrangeC Compiler And Tool Chain

Project Build Status:
[![Build status Appveyor](https://ci.appveyor.com/api/projects/status/4ts7bsgas67osyht?svg=true)](https://ci.appveyor.com/project/LADSoft/orangec)
[![Build status Github Actions](https://github.com/LADSoft/OrangeC/actions/workflows/github_actions_build.yml/badge.svg)](https://github.com/LADSoft/OrangeC/actions/workflows/github_actions_build.yml)


Documentation status: 
[![Documentation Status](https://readthedocs.org/projects/orangec/badge/?version=latest)](http://orangec.readthedocs.io/en/latest/Tools/)

Coverity scan status:
[![Coverity Scan Build Status](https://scan.coverity.com/projects/15633/badge.svg)](https://scan.coverity.com/projects/ladsoft-orangec)

Orange C is currently a Win32 C++ compiler.   Long term it is going to be retargetable.


The Orange C package also includes a C compiler for .Net, which compiles to MSIL/CIL.   It is an offshoot of Orange C that is retargeted for .NET.   Documentation may be found here: [OCCIL documentation](doc/occil.md)

Source Code for Orange C is released under the GNU General Public License version 3.

Current languages targeted by Orange C are C++14 and C11.

The tools in this package that aren't specific to this toolchain are also available in standalone repositories.   This includes the [ORC resource compiler](https://www.github.com/LADSoft/ORC),[omake makefile utility](https://www.github.com/LADSoft/OMAKE), and a [C++ library to generate and parse .NET assemblies](https://www.github.com/LADSoft/DotNetPELib).

This compiler comes in an install package with an IDE suitable for developing WIN32 programs.  Help files are included; the source format for the help files is HELPSCRIBBLE's HSC format.

The compiler can be built with various C++11 compilers, however, by default it is configured to use the Visual Studio 2017 community edition.   A solution exists which will build all files in the project, or you can use the project's omake program to build it from the command line (you can also use MSBUILD if so inclined)

The compiler uses a text-based output format, a variant of the IEEE-695 OMF.   I've preferred this since it is easier to debug…  at one point I did try turning it into a binary format but once I got far enough to prototype it there didn't seem to be enough gain to complete the work.  And I really like text files a lot lol!

The make program is eerily similar to GNU make, however, with me never having the pleasure of actually using GNU make I don't know how close I got.  The code was developed independently of GNU make, and without ever looking at the sources for GNU make.

At the moment there is only partial support for retargeting the compiler and toolchain.  The toolchain introduces an extra phase to the usual compile-assemble-link process in the form of a post-link step called a downloader.  Essentially the downloader takes the output of the linker and turns it into an OS-friendly executable file for whatever OS is being targeted.  There are presently downloaders for WINDOWS 32 bit PE files, several DOS 32-bit file formats, and a downloader that generates HEX files for burning to EPROM.

The linker supports specification files to allow customization of the link and download processes; these files basically select a downloader based on command line switches and then guide the link process in generating an image that can be processed by the downloader.  Meta-data may be added to the linker output using this mechanism as well, for example it is used in setting up default values for the objects in the PE file.

There is some support for retargeting the assembler;  the instruction set may be completely customized in an architecture description file, which makes a set of tables to guide the translation from text to binary code.   This is used to retarget the assembler code generation, and will eventually also be used in the compiler.  There is some other data such as for assembler directives which will also eventually be placed into the architecture description….  And long term there is a goal of placing directives for translating the intermediate code into assembly language statements there as well.

There has been some thought as to eventually making this an x64 compiler, however, that would take a bit of effort as it wasn't well-supported while developing the code.  Mostly, there are a lot of place that long-long values need to be passed around in the tools, where only int values are being passed around.

The [Appveyor CI](https://ci.appveyor.com/project/LADSoft/orangec) project for this repository builds a setup file after each checkin.   It uses `omake fullbuild` to do this. See the file [`build.md`](build.md) for instructions on how to build the project.

The [Read The Docs](http://orangec.readthedocs.io/en/latest/Tools/) project for this repository verifies the documentation after each checkin.

For the project's history see the file [`HISTORY.md`](HISTORY.md).
