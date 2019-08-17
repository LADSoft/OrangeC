set COMMAVERSION=%APPVEYOR_BUILD_VERSION:.=,%
echo #define STRING_VERSION "%APPVEYOR_BUILD_VERSION%" > c:\orangec\src\version.h
echo #define PRODUCT_VERSION %COMMAVERSION% >> c:\orangec\src\version.h
echo #define CLIB_VERSION %COMMAVERSION% >> c:\orangec\src\version.h
echo #define PRODUCT_STRING_VERSION STRING_VERSION >> c:\orangec\src\version.h
echo #define PRODUCT_NAME "Orange C Compiler Package" >> c:\orangec\src\version.h
echo #define CLIB_STRING_VERSION STRING_VERSION >> c:\orangec\src\version.h
echo #define COPYRIGHT "Copyright (C) LADSoft 2006-2019" >> c:\orangec\src\version.h
echo #define LIBRARY_COPYRIGHT "Copyright (C) LADSoft 1994-2019" >> c:\orangec\src\version.h
