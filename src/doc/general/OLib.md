


#OLib

 
 **OLib** is an object file librarian.  It is used to combine a group of object files into a single file, to make it easier to operate on the group of files.  It is capable of adding, removing, and extracting object files from a static library.


##Command Line Options

 
 The general format of an **OLib** command line is:
 
> OLib \[options\] library object-file-list
 
 where _library _specifies the library, and _object-file-list_ specifies the list of files to operate on.
 
 For example:
 
> OLib test.l + obj1.o obj2.o obj3.o obj4.o obj5.o
 
 adds several object files to the library **test.l**.
 
 The object file list can have wildcards:
 
> OLib test.l + \*.o
 
 adds all the object files from the current directory.


###Response Files

 Response files can be used as an alternate to specifying input on the command line.  For example:
 
> OLib test.l @myresp.lst
 
 will take command line options from **myresp.lst**.  Response files might be used for example if a library is to be made out of dozens of object files, and they won't all fit on the OS command line.


###Case Insensitivity
 

 
 **OLib** will allow the creation of case insensitive libraries with the **/c-** switch.  In general you don't need to make a library case insensitive, as the linker will handle case insensitivity based on command line switches even if the library is case-sensitive.


###Operating Modes
 

 In a previous examples the '+' symbol was used to indicate that the following files should be added to the library.  '+' doesn't have to be used in this case because the default is to add files to the library.  But there are two other command line modifiers that can be used to extract files from the library and remove files from the library.  These are '\*' for extract and '-' for remove.  Note that '-' is also used for command line switches; to prevent it from being ambiguous it must be present with spaces on either side when used.  The '+' and '-' and '\*' can be mixed on the command line; files after one of these modifies will be processed according to that modifier until another modifier is encountered.  For example:
 
> OLib test.l \* obj1.o
 
 extracts obj1.o from the library and places it in the current directory, and 
 
> OLib Test.l - obj2.o 
 
 removes obj2.o from the library and destroys it.  As a more complex example:
 
> OLib test.l + add1.o add2.o - rem1.o rem2.o \* ext1.o
 
 adds the files add1.o and add2.o, removes the files rem1.o and rem2.o, and extracts the file ext1.o.  The order of the modifiers in this example is arbitrary, and modifiers can occur more than once on the command line or in the response file.
 
 
 