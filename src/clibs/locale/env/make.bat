cc386 /c env.c
valx /32 /SY env.obj,env.bin,env,,
pkzip env.zip env.bin
extrzip
copy env.h ..