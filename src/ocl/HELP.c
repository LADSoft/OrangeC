#include "afx.h"

static char usage[] = /* Usage */ 
"OCL    Version " STRING_VERSION " (c) Kirill Joss, email: jossk@bk.ru\n"
"Usage: OCL.EXE [options] files\n"
" (first option is default)         @fname    specify response file\n"
" /A[-|+]   enable extensions       /?,/h,/H  this help\n"
" /Dxxx     define something        /Ennn     max number of errors\n"
" /Ipath    specify include path    /Lpath    specify .LIB & .OBJ dir\n"
" /Oxxx     optimize parameters     /Cxxx     codegen parameters\n"
//" /q[-|+]   quiet mode              /X[-|+]   enable C++ exception handling\n"
" /K[-|+]   keep response file      /k[-|+]   keep generate files\n"
" /e[+|-]   dump errors to file     /m[-|+]   generate .MAP file\n"
" /l[-|+]   dump listing file       /i[-|+]   dump preprocessed file\n"
" /w-xxx    disable a warning       /n[-|+]   no default .LIB & .OBJ\n"
" /e=name   set name of .EXE file   /v[-|+]   debug info\n"
" /a        generate .ASM file      /c        generate .OBJ file\n"
" /9        C99 compile mode\n\n"
//" /NASM,/TASM,/TASMX,/TASM32,/MASM,/ML,/WASM,/LASM,/LASM32  use /assembler\n"
//" /VALX,/TLINK,/LINK,/WLINK                                 use /linker\n"
" /PMODE,/PMODEW,/DOS4G,/WDOSX,/DOS32A,/ZRDX,/CAUSEWAY,/HX,/WHX,/D3X use Extender\n"
" OR\n"
" /W32CON,/W32GUI,/W32DLL                                   gen Win32 App\n\n"
"Compile '" __DATE__ " " __TIME__ "'";


void help(void) {
    puts( usage );
    exit(0);
}
