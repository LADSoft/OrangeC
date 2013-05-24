OrangeC
=======

OrangeC Compiler And Tool Chain

This is a tool chain that is heading toward being retargetable, but for now it only targs x86 32 bit.  The compiler in the tool chain is an optimizing compiler using some techniques out of the literature.

I've only compiled this on windows but it is mostly OS agnostic.

For windows make a directory ORANGEC off the root and put the src directory in it.

you can build it with the makefiles but will need the free BCC55 release to do that.  The make *may* fail the first time as it won't copy the orc.exe file to a directory on the path before it is used.

you can also build this with visual studio express, I've only tried it with the 2010 version however.

after building it you can use MAKE DISTRIBUTE to build the release package.  Be careful though until I fix it MAKE DISTRIBUTE is going to delete this readme file.  I will try to get it fixed within a day or two.



