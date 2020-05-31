szip, sunzip: (c) 1997-1999 Michael Schindler, szip@compressconsult.com
http://www.compressconsult.com/szip/

(R),SM:  szip and the data compression logo are "geschuetztes Markenzeichen"
   of Michael Schindler
US and other patents pending. 

The program szip performs data compression/decompression, the current
version is 1.12a.

There were several previous incompatible versions of szip;
version 1.00 to 1.04 use one format, version 1.05X another one.
version 1.10 was available as alpha test only but had a bug in encoding.
If you have szip files from that versions you need to keep the old
decompression program around; if you lost it you can still download
them from the website.
1.11 and 1.12 differ only in internals (IO macros instead of function
calls, more memory allocations) from this version.

Usage:
szip [options] [inputfile [outputfile]]

option              meaning                 default
-d                  decompress
-b<blocksize>       blocksize in 100kB      -b17
-o<order>           order of context        -o6
-r<recordsize>      recordsize              -r1
-i                  incremental coding (differences to previous value)
-v<level>           turn on messages        -v0
options may be grouped like -b14o10r3

if outputfile is omitted output is written to standardoutput.
if inputfile is omitted too input is read from standardinput.

I recommend using .sz for szipped files and .tar.sz or .tsz
for szipped tarfiles. Future versions will produce these extensions.


option effects:

decompress: tells the program to decompress; default operation
    mode is compression. If present all other options except
    v are ignored.
blocksize: larger blocks usually give better compression, but
    if your system gets into paging it will be slow. No effect
    on speed if enough memory is available. 1-41 possible.
order: higher order gives better compression (and increased time).
    3-255 possible. There is special code for order 4; this will give
    a faster (even faster than order 3) compression.
    order 0 makes a BWT transform (unlimited order)
    Decompression of -o0 is fastest, so use it for distribution.
    fast compression but larger: -o4
    fast decompression and probably smaller: -o0
    Some files compress better with a small order like 3 or 4.
recordsize: tells what size (in bytes) the elementary datatype is.
    getting this one right will improve compression.
    24-bit graphics: use -r3
    2-channel 8-bit audio: use -r2
    4-byte words: use -r4
    1-byte chars: use -r1 (default)
    the recordsize need not be in sync with the real record; if you
    have a 4-byte header on an -r3 file still choose -r3.
    1-127 possible
incremental: use differences to the last value (after recordsize
    reordering) instead of the actual value. Good for sounds.
verbosity level: output progress messages.


OPERATING SYSTEMS SUPPORTED:
The code is plain C; please check out the webpage for available
compilations.
It can be compiled for other platforms upon request, please ask.
The produced files are platform independent, and all versions 1.1x
produce this format.


COPYING:
This Program can be freely distributed as unchanged executeable, as
long as this file accompanies them unchanged. The program itself may
not be sold, however you may collect fees for copying, distribution or
bundled items. It MUST be clear to your customer that he can get the
same free of charge from other sources; mentioning the website
http://www.compressconsult.com/szip/ and "Freeware" will fulfill this
requirement.


CAVEATS:
DOS/Windows does NOT support binary pipes, so SPECIFY BOTH FILES.

Depending on the compilation and the stdio C library there may be
limitations of compressable filesizes; on 32-bit computers this limit
is often 2 or 4GB. If you see this problem and you can recompile C-code
please get in touch with me.


The intention is mainly demonstration; I do not consider them production
versions.

This free program comes with ABSOLUTELY NO WARRANTY OF ANY KIND.


Ask me about compression for your data; see http://www.compressconsult.com
for more information.

Szip is a free program of 
> d a t a < / / / /
compression consulting
