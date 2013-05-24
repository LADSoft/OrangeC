OrangeC
=======

OrangeC Compiler And Tool Chain

This is a tool chain that is heading toward being retargetable, but for now it only targets x86 32 bit.  
The compiler in the tool chain is an optimizing compiler using some techniques out of the literature.

License for code that I authored is Berkely style.

I've only compiled this on windows but it is mostly OS agnostic.

For windows make a directory ORANGEC off the root and put the src directory in it.

you can build it with the makefiles but will need the free BCC55 release to do that.  
The make *may* fail the first time as it won't copy the orc.exe file to a directory on the path before it is used.

you can also build this with visual studio express, I've only tried it with the 2010 version however.

after building it you can use MAKE DISTRIBUTE to build the release package.  
Be careful though until I fix it MAKE DISTRIBUTE is going to delete this readme file.  
I will try to get it fixed within a day or two.



The Orange C compiler is going to be a retargetable optimizing compiler and toolchain.  Work over the last few years has resulted in an optimizing compiler than generates working WIN32 programs.  The optimizations in the compiler follow some of the standard optimizations, and also perform some optimizations from the literature.

Orange C is released under a Berkely-style license.

This compiler comes in an install package with an IDE suitable for developing WIN32 programs.  Help files are included; the source format for the help files is HELPSCRIBBLE's HSC format.

My primary development environment for many years has been Borland C 5.5, and I still support that.  Recently, I have started including build files for Visual Studio Express.  There is currently no support for compiling under other operating systems although most of the sources are OS-agnostic, with the exception of the make program which has heavy reliance on win32.

The compiler uses a text-based output format, a variant of the IEEE-695 OMF.   I've preferred this since it is easier to debug…  at one point I did try turning it into a binary format but once I got far enough to prototype it there didn't seem to be enough gain to want to complete the work.

The make program is eerily similar to GNU make, however, with me never having the pleasure of actually using GNU make I don't know how close I got.  The code was developed independently of GNU make, and without ever looking at the sources for GNU make.

At the moment there is only partial support for retargeting the compiler and toolchain.  The toolchain introduces an extra phase to the usual compile-assemble-link process in the form of a post-link step called a downloader.  Essentially the downloader takes the output of the linker and turns it into an OS-friendly executable file for whatever OS is being targeted.  There are presently downloaders for WINDOWS 32 bit PE files, several DOS 32-bit file formats, and a downloader that generates HEX files for burning to eprom.

The linker supports specification files to allow customization of the link and download processes; these files basically select a downloader based on command line switches and then guide the link process in generating an image that can be processed by the downloader.  Meta-data may be added to the linker output using this mechanism as well, for example it is used in setting up default values for the objects in the PE file.

There is some support for retargeting the assembler;  the instruction set may be completely customized in an architecture description file, which makes a set of tables to guide the translation from text to binary code.   This is used to retarget the assembler code generation, and will eventually also be used in the compiler.  There is some other data such as for assembler directives which will also eventually be placed into the architecture description….  and long term there is a goal of placing directives for translating the intermediate code into assembly language statements there as well.

There has been some thought as to eventually making this an x64 compiler, however, that would take a bit of effort as it wasn't well-supported while developing the code.  Mostly, there are a lot of place that long-long values need to be passed around in the tools, where only ints are being passed around.

To reach the retarget ability goal there is some compiler and assembler work, however, that is currently deferred while work is being done to turn this into a c++ compiler.

See the file BUILD.TXT for instructions on how to build the project.
