## OCC Warning Control
  
### /E\[+\]nn    error control
 
nn is the maximum number of errors before the compile fails; if + is specified extended warnings will be shown that are normally disabled by default.  For example:
 
     OCC /E+44 myfile.c
 
enables extended warnings and limits the number of errors to 44.  By default only 25 errors will be shown and then the compiler will abort.

### /wxxx    warning control

The /w switch controls the warning output.   Note that by default or as a result of the /E switch some warnings may be automatically disabled.

There are a variety of options.

     OCC /w myfile.c

disables all warnings.

     OCC /wx myfile.c

or

     OCC /werror myfile.c

turn all warnings into errors.

     OCC /wd123 myfile.c

disables warning #123.

     OCC /w123 myfile.c

enables warning #123.

     OCC /wo123 myfile.c

only displays the warning once per source file.

     OCC /We123 myfile.c

turns warning #123 into an error.
