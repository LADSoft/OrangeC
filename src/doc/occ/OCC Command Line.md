****


##OCC Command Line

 
 OCC has a variety of command line parameters, most of which aren't needed to just compile a file.  It will also allow you to specify multiple input files.  The current default for OCC is to generate executable files.  While processing the command line, OCC may encounter a command to process command line arguments from a file.
 
 The general format of the command line is as follows:
 
 OCC \[parameters\]  list of files
 
 The list of files can be a list of one or more C language files.  C++ language files are supported as well.  If you don't specify an extension on the command line it will default to .C; it will detect a .CPP extension and activate C++ mode as required.

The list of files can also include wild cards.
 
 OCC will accept response files with a list of command line options.  To use a response file, prefix its name with '@':
 
 OCC \[parameters\] @resp.cc
 
 There are a variety of parameters that can be set.  Help is available for the following:
 
     [Output Control](OCC%20Output%20Control.html)
     [Error Control](OCC%20Error%20Control.html)
     [List File Control](OCC%20List%20File%20Control.html)
     [Preprocessor File Control](OCC%20Preprocessor%20File%20Control.html)
     [Compilation Modes](OCC%20Compilation%20Modes.html)
     [Defining Macros](OCC%20Defining%20Macros.html)
     [Specifying Include Paths](OCC%20Specifying%20Include%20Paths.html)
     [Translating Trigraphs](OCC%20Translating%20Trigraphs.html)
     [Code Generation Parameters](OCC%20Code%20Generation%20Parameters.html)
     [Optimizer Parameters](OCC%20Optimizer%20Parameters.html)
 
 