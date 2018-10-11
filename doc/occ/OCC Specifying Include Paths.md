## OCC Specifying Include Paths

### /Ipath    specify include path.  
 
 If your file uses headers that aren't in the directory you are running CC386 from, you will have to tell it what directory to look in.  You can have multiple search directories by using a semicolon between the directory names.  If there are multiple files with the same name in different directories being searched, CC386 will use the first instance of the file it finds by searching the path in order.  For example:
 
     OCC /I..\include;..\source;c:\libraries\include myfile.c
 
 Will search the paths ..\\include , ..\\source, and c:\\libraries\\include in that order.  Note that you generally don't have to specify a path to the OCC compiler header files such as stdio.h, as that will be taken care of automatically.

### /zpath    specify system include path

  This command will include this directory before ALL other directories, giving it priority.
  Please don't use this unless you're using it to include a new entire header system