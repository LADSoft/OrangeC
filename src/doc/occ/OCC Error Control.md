## OCC Error Control

 
 This section deals with compiler parameters related to error messages.
 
 +e     put the compiler errors in a file.  
 
     For each file processed, OCC will create a file with the same name as the original source with the extension.  '.err'.  The contents of this file will be a listing of any errors or warnings which occurred during the compile.  For example:
 
 
     OCC +e myfile.c
 
     results in myfile.err
 
 +Q    Quiet mode
 
     Don't display errors or warnings on the console.  Generally this is used in conjunction with the +e switch.  For example:
 
     OCC +e +Q myfile.c
 
     puts the errors in a file, without displaying them on the console.
 
 /w-xxx    disable warnings
 
     you use this once for each warning you want to disable, xxx is a three character warning code.  These are presently undocumented, except for /W-all which disables all warnings.  For example:
 
     OCC /w-all myfile.c
 
     disables all warnings
 
 /E\[+\]nn    error control
 
     nn is the maximum number of errors before the compile fails; if + is specified extended warnings will be shown that are normally disabled by default.  For example:
 
     OCC /E+44 myfile.c
 
     enables extended warnings and limits the number of errors to 44.  By default only 25 errors will be shown and then the compiler will abort.
 
 
 