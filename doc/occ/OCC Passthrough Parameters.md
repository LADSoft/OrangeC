## OCC Passthrough Parameters

Passthrough parameters control the operation of programs that OCC runs as part of its operation.

There are three programs that can be run, oasm, orc, and olink.


### /px-yyy    pass a switch through
 
This parameter allows passing another parameter.  Here 'x' selects the program to pass through to, and 'yyy' is the switch to pass through.   'x' is 'a' to pass through to the assembler, 'l' to pass through to the linker, and 'r' to pass through to the resource compiler.

For example:

    occ /pl-mx test.cpp

passes the -mx switch when calling the linker.   This switch generates a map file.


### /lxxx      include a library

This switch strips any extension from 'xxx' and adds '.l'.   Then when the linker is called, the resulting library will be passed.


### /Lxxx      set library path

This switch adds to the library path.   The library path is passed to the linker so that it can search for libraries.