## OCCPR

**OCCPR** is the code completion compiler.   It is basically the C compiler, instrumented to output information into an SQLITE3 database.   While it can be used as a standalone compiler, the IDE generally configures it through a pipe.  It uses the information in the database for various things, such as name colorization, structure and function code completion, and colorization of unused areas in #defines.

The database has the file extension ".ods".
