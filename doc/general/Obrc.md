## OBRC

**OBRC** is used for compiling browse information, for the 'jump list' at the top of the editor window and for the Browse functionality (F12).   In general it is only used as a background tool so it isn't documented.  But be aware that it requires compiler support...   a compiler switch is used to cause the compiler to generate .CBR files, which are kind of object file with browse information for the compiled file.  

**OBRC** generates SQLITE3 databases, with a file extension ".obr".