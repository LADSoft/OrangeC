The Orange Toolchain is designed for DOS/WIN32.  It uses win32 binaries along
with Japheth's WIN32 emulator for DOS to get the MSDOS functionality.

The project comes with a complete set of tools that should be familiar to those
who work regularly with command line tools.  There is some documentation, however
it is incomplete, especially the MAKE section has not been written.

Set up is simple; extract the EXE file to the place of your choice, then
set environment variables like this:

set PATH=<your path>\orangec\bin;%PATH%
set ORANGEC=<your path>\orangec

This needs to be done either for MSDOS or WIN32

Additionally, under MSDOS you may find it convenient to run HXLDR32.EXE at some point
to allow files the toolchain generates to run.  There are ways to generate code
that does not require this (for example see the HXDOS documentation or use OCL to switch
to a different DOS extender).

dlindauer@acm.org <David Lindauer>

April 11, 2010
