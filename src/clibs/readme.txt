This is sources for a 32-bit C run-time library.  You will need to 
modify some things if you plan to use it as a 16-bit library.  This
library is useful for DOS DPMI and WIN32 programs.

This program should be unpacked with pkunzip -d to set the proper 
directory structure

see license.txt for licensing information

To build this library, use the following make files:

DLL.MAK  - lscrtl.dll
win.MAK  - win32 version of the library, import library, and msvcrt/crtdll stubs
dos.MAK  - dos version of the library

the low level functions that need to be defined are
prototyped in stdinc\libp.h.  You only need to create
_ll_ functions, and only the ones that you actually want to
use.  If you use file I/O (e.g. stdin, stdout) you also have
to define some structures to describe stdin and stdout and stderr.

in the MSDOS directory you will find sample implementation of
these _ll functions, the target is DPMI running under MSDOS.
You can use this as a template if you want.

My website is:

http://members.tripod.com/~ladsoft/cc386.htm

Companion files to this file which should be found on my website
are:

cwdl375e.zip    - WIN32 version of compiler executables
ccdl375e.zip    - DOS version of compiler executables
cxdl375e.zip    - combined executables, minimal installation
cwdl375s.zip    - compiler and tools sources

The following people have contributed their sources to this
package:

Kirill Joss         - Watcom support
David Lindauer      - Debug/386
David Lindauer      - run-time libraries
Scott Christley	    - parts of the WIN32 header files


you can contact the author of the package at:

David Lindauer
mail: camille@bluegrass.net
web site: http://members.tripod.com/~ladsoft
simtel download location: ftp://ftp.simtel.net//pub/simtelnet/msdos/c/ccdl186l.zip