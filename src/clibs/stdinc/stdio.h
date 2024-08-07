/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

/*  stdio.h

    Definitions for stream input/output.

*/

#ifndef __STDIO_H
#define __STDIO_H

#pragma pack(1)

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifndef __STDARG_H
#    include <stdarg.h>
#endif

#ifndef RC_INVOKED
#pragma pack(4)
typedef struct __file__
{
    short token;           /* Used for validity checking */
    unsigned short flags;  /* File status flags          */
    unsigned char hold;    /* Ungetc char if no buffer   */
    int fd;                /* File descriptor            */
    int level;             /* fill/empty level of buffer */
    int bsize;             /* Buffer size                */
    unsigned char* buffer; /* Data transfer buffer       */
    unsigned char* curp;   /* Current active pointer     */
    struct __file2
    {
        enum
        {
            __or_unspecified,
            __or_narrow,
            __or_wide
        } orient;       /* stream orientation */
        int mbstate[2]; /* space for mbstate_t structure */
        unsigned flags2;
        union
        {
            wchar_t* name; /* filename */
            void* dynamicBuffer[2];
        };
        int lock; // this is actually an atomic type 
    } * extended;
} FILE; /* This is the FILE object    */
#pragma pack()

#if defined(__MSIL__)
#    if defined(__MANAGED__)
extern FILE _RTL_DATA* __stdin;
extern FILE _RTL_DATA* __stdout;
extern FILE _RTL_DATA* __stderr;
extern FILE _RTL_DATA* __stdaux;
extern FILE _RTL_DATA* __stdprn;
#    else
extern void* __stdin;
extern void* __stdout;
extern void* __stderr;
extern void* __stdaux;
extern void* __stdprn;
#    endif
#elif defined(__MSVCRT_DLL) || defined(__CRTDLL_DLL)
extern FILE* __stdin;
extern FILE* __stdout;
extern FILE* __stderr;
extern FILE* __stdaux;
extern FILE* __stdprn;
#elif defined(__LSCRTL_DLL)
extern FILE _IMPORT* __stdin;
extern FILE _IMPORT* __stdout;
extern FILE _IMPORT* __stderr;
extern FILE _IMPORT* __stdaux;
extern FILE _IMPORT* __stdprn;
#else
extern FILE _RTL_DATA* __stdin;
extern FILE _RTL_DATA* __stdout;
extern FILE _RTL_DATA* __stderr;
extern FILE _RTL_DATA* __stdaux;
extern FILE _RTL_DATA* __stdprn;
#endif
#endif

#define stdin __stdin
#define stdout __stdout
#define stderr __stderr
#define stdaux __stdaux /* these two not supported now */
#define stdprn __stdprn

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED
    typedef long fpos_t;
#ifndef _OFFT_DEFINED
#define _OFFT_DEFINED
    typedef long off_t;
#endif
#endif

#define FILTOK 0x444c
#define STACKPAD 512

/*  "flags" bits definitions
 */
#define _F_RDWR 0x0003          /* Read/write flag       */
#define _F_READ 0x0001          /* Read only file        */
#define _F_WRIT 0x0002          /* Write only file       */
#define _F_BUF 0x0004           /* Malloc'ed Buffer data */
#define _F_LBUF 0x0008          /* line-buffered file    */
#define _F_ERR 0x0010           /* Error indicator       */
#define _F_EOF 0x0020           /* End of file indicator */
#define _F_BIN 0x0040           /* Binary file indicator */
#define _F_IN 0x0080            /* Data is incoming      */
#define _F_OUT 0x0100           /* Data is outgoing      */
#define _F_TERM 0x0200          /* File is a terminal    */
#define _F_APPEND 0x400         /* Need to ignore seek */
#define _F_BUFFEREDSTRING 0x800 /* it is not an actual file */
#define _F_XEOF 0x1000          /* EOF is pending based on CTRL-Z in buffered input */
#define _F_VBUF 0x2000          /* true if setvbut allowed */
#define _F_UNGETC 0x4000        /* character was cached with ungetc */

#define _F2_DYNAMICBUFFER 1
#define _F2_WCHAR 2
/* End-of-file constant definition
 */
#define EOF (-1) /* End of file indicator */

/* Default buffer size use by "setbuf" function
 */
#define BUFSIZ 512 /* Buffer size for stdio */

/* Size of an arry large enough to hold a temporary file name string
 */
#define L_ctermid 5 /* CON: plus null byte */
#define P_tmpdir "" /* temporary directory */
#define L_tmpnam 13 /* tmpnam buffer size */

/* Constants to be used as 3rd argument for "fseek" function
 */
#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0

/* Number of unique file names that shall be generated by "tmpnam" function
 */
#define TMP_MAX 32767

#define _NFILE_ 40 /* SHOULD BE SAME AS IN IO.H */

/* Definition of the control structure for streams
 */

/* Number of files that can be open simultaneously
 */
#define FOPEN_MAX (_NFILE_ - 2) /* (_NFILE_ - stdaux & stdprn) */

#define FILENAME_MAX 265

    /* Standard I/O predefined streams
     */
#ifndef _NFILE_EXT
#ifndef RC_INVOKED
#    define _NFILE_EXT
    extern unsigned _RTL_DATA _nfile;
    FILE* _RTL_FUNC _IMPORT __getStream(int stream);
#endif
#endif
#define _IOFBF 1
#define _IOLBF 2
#define _IONBF 4

#ifndef RC_INVOKED
    void _RTL_FUNC _IMPORT clearerr(FILE* __stream);
    int _RTL_FUNC _IMPORT fclose(FILE* __stream);
    int _RTL_FUNC _IMPORT fflush(FILE* __stream);
    int _RTL_FUNC _IMPORT fgetc(FILE* __stream);
    int _RTL_FUNC _IMPORT fgetpos(FILE* __stream, fpos_t* __pos);
    char* ZSTR _RTL_FUNC _IMPORT fgets(char* ZSTR __s, int __n, FILE* __stream);
    int _RTL_FUNC _IMPORT fileno(FILE*);
    int _RTL_FUNC _IMPORT _fileno(FILE*);
    FILE* _RTL_FUNC _IMPORT fopen(const char* ZSTR __path, const char* ZSTR __mode);
    FILE* _RTL_FUNC _IMPORT fmemopen(void* buf, size_t size, const char* mode);
    FILE* _RTL_FUNC _IMPORT open_memstream(char** ptr, size_t* sizeloc);
    int _RTL_FUNC _IMPORT fprintf(FILE* restrict __stream, const char* ZSTR restrict __format, ...);
    int _RTL_FUNC _IMPORT fputc(int __c, FILE* __stream);
    int _RTL_FUNC _IMPORT fputs(const char* ZSTR __s, FILE* __stream);
    size_t _RTL_FUNC _IMPORT fread(void* __ptr, size_t __size, size_t __n, FILE* __stream);
    FILE* _RTL_FUNC _IMPORT freopen(const char* ZSTR __path, const char* ZSTR __mode, FILE* restrict __stream);
    int _RTL_FUNC _IMPORT fscanf(FILE* restrict __stream, const char* ZSTR restrict __format, ...);
    int _RTL_FUNC _IMPORT fseek(FILE* __stream, long __offset, int __whence);
    int _RTL_FUNC _IMPORT fseeko(FILE* __stream, off_t __offset, int __whence);
    int _RTL_FUNC _IMPORT fsetpos(FILE* __stream, const fpos_t* __pos);
    int _RTL_FUNC _IMPORT _fseek64(FILE *, long long, int);  /* 14-06-29 */
    long _RTL_FUNC _IMPORT ftell(FILE* __stream);
    off_t _RTL_FUNC _IMPORT ftello(FILE* __stream);
    long long _RTL_FUNC _IMPORT _ftell64(FILE *);  /* 14-06-29 */
    size_t _RTL_FUNC _IMPORT fwrite(const void* __ptr, size_t __size, size_t __n, FILE* __stream);
    FILE* _RTL_FUNC popen(const char* ZSTR name, const char* ZSTR restrict mode);
    FILE* _RTL_FUNC _popen(const char* ZSTR name, const char* ZSTR restrict mode);
    int _RTL_FUNC _IMPORT _pclose(FILE *);
    char* ZSTR _RTL_FUNC _IMPORT gets(char* ZSTR __s);
    void _RTL_FUNC _IMPORT perror(const char* ZSTR __s);
    int _RTL_FUNC _IMPORT printf(const char* ZSTR restrict __format, ...);
    int _RTL_FUNC _IMPORT puts(const char* ZSTR __s);
    int _RTL_FUNC _IMPORT remove(const char* ZSTR __path);
    int _RTL_FUNC _IMPORT rename(const char* ZSTR __oldname, const char* ZSTR __newname);
    void _RTL_FUNC _IMPORT rewind(FILE* __stream);
    int _RTL_FUNC _IMPORT scanf(const char* ZSTR __format, ...);
    void _RTL_FUNC _IMPORT setbuf(FILE* restrict __stream, char* ZSTR restrict __buf);
    int _RTL_FUNC _IMPORT setvbuf(FILE* restrict __stream, char* ZSTR restrict __buf, int __type, size_t __size);
    int _RTL_FUNC _IMPORT snprintf(char* ZSTR restrict __buffer, size_t n, const char* ZSTR restrict __format, ...);
    int _RTL_FUNC _IMPORT _snprintf(char* ZSTR restrict __buffer, size_t n, const char* ZSTR restrict __format, ...);
    int _RTL_FUNC _IMPORT sprintf(char* ZSTR restrict __buffer, const char* ZSTR restrict __format, ...);
    int _RTL_FUNC _IMPORT asprintf(char** ZSTR restrict __buffer, const char* ZSTR restrict __format, ...);
    int _RTL_FUNC _IMPORT sscanf(const char* ZSTR restrict __buffer, const char* ZSTR restrict __format, ...);
    char* ZSTR _RTL_FUNC _IMPORT strerror(int __errnum);
    errno_t _RTL_FUNC _IMPORT strerror_s(char* ZSTR buf, size_t size, errno_t __errnum);
    errno_t _RTL_FUNC _IMPORT _strerror_s(char* ZSTR buf, size_t size, const char* ZSTR msg);
    char* ZSTR _RTL_FUNC _IMPORT tempnam(char* ZSTR __dir, char* ZSTR __prefix);
    char* ZSTR _RTL_FUNC _IMPORT _tempnam(char* ZSTR __dir, char* ZSTR __prefix);
    FILE* _RTL_FUNC _IMPORT tmpfile(void);
    char* ZSTR _RTL_FUNC _IMPORT tmpnam(char* ZSTR __s);
    int _RTL_FUNC _IMPORT ungetc(int __c, FILE* __stream);
    int _RTL_FUNC _IMPORT vfprintf(FILE* restrict __stream, const char* ZSTR restrict __format, va_list __arglist);
    int _RTL_FUNC _IMPORT vfscanf(FILE* restrict __stream, const char* ZSTR restrict __format, va_list __arglist);
    int _RTL_FUNC _IMPORT vprintf(const char* ZSTR restrict __format, va_list __arglist);
    int _RTL_FUNC _IMPORT vscanf(const char* ZSTR restrict __format, va_list __arglist);
    int _RTL_FUNC _IMPORT vsnprintf(char* ZSTR restrict __buffer, size_t __n, const char* ZSTR restrict __format,
                                    va_list __arglist);
    int _RTL_FUNC _IMPORT _vsnprintf(char* ZSTR restrict __buffer, size_t __n, const char* ZSTR restrict __format,
                                     va_list __arglist);
    int _RTL_FUNC _IMPORT vsprintf(char* ZSTR __buffer, const char* ZSTR __format, va_list __arglist);
    int _RTL_FUNC _IMPORT vasprintf(char** ZSTR __buffer, const char* ZSTR __format, va_list __arglist);
    int _RTL_FUNC _IMPORT vsscanf(const char* ZSTR __buffer, const char* ZSTR __format, va_list __arglist);
    int _RTL_FUNC _IMPORT unlink(const char* ZSTR __path);
    int _RTL_FUNC _IMPORT _unlink(const char* ZSTR __path);
    int _RTL_FUNC _IMPORT getc(FILE* __fp);

    int _RTL_FUNC _IMPORT getchar(void);
    int _RTL_FUNC _IMPORT putchar(const int __c);

    int _RTL_FUNC _IMPORT putc(const int __c, FILE* __fp);
    int _RTL_FUNC _IMPORT feof(FILE* __fp);
    int _RTL_FUNC _IMPORT ferror(FILE* __fp);

    int _RTL_FUNC _IMPORT fcloseall(void);
    int _RTL_FUNC _IMPORT _fcloseall(void);
    FILE* _RTL_FUNC _IMPORT fdopen(int __handle, const char* ZSTR restrict __type);
    FILE* _RTL_FUNC _IMPORT _fdopen(int __handle, const char* ZSTR restrict __type);
    int _RTL_FUNC _IMPORT fgetchar(void);
    int _RTL_FUNC _IMPORT _fgetchar(void);
    int _RTL_FUNC _IMPORT flushall(void);
    int _RTL_FUNC _IMPORT _flushall(void);
    int _RTL_FUNC _IMPORT fputchar(int __c);
    int _RTL_FUNC _IMPORT _fputchar(int __c);
    FILE* _RTL_FUNC _IMPORT _fsopen(const char* ZSTR __path, const char* ZSTR __mode, int __shflag);
    int _RTL_FUNC _IMPORT getw(FILE* __stream);
    int _RTL_FUNC _IMPORT _getw(FILE* __stream);
    int _RTL_FUNC _IMPORT putw(int __w, FILE* __stream);
    int _RTL_FUNC _IMPORT _putw(int __w, FILE* __stream);
    int _RTL_FUNC _IMPORT rmtmp(void);
    int _RTL_FUNC _IMPORT _rmtmp(void);
    char* ZSTR _RTL_FUNC _IMPORT _strerror(const char* ZSTR __s);

    FILE * _RTL_FUNC _IMPORT _wfopen(const wchar_t * restrict, const wchar_t * restrict);
    FILE * _RTL_FUNC _IMPORT _wfsopen(const wchar_t * restrict, const wchar_t * restrict, int __shflag);
    FILE * _RTL_FUNC _IMPORT _wfreopen(const wchar_t * restrict, const wchar_t * restrict, FILE * restrict);
    FILE * _RTL_FUNC _IMPORT _wfdopen(int, const wchar_t *);
    int _RTL_FUNC _IMPORT _wremove(const wchar_t *);
    int _RTL_FUNC _IMPORT _wrename(const wchar_t *, const wchar_t *);


    void _RTL_FUNC _IMPORT flockfile(FILE* filehandle);
    int _RTL_FUNC _IMPORT ftrylockfile(FILE* filehandle);
    void _RTL_FUNC _IMPORT funlockfile(FILE* filehandle);

    int _RTL_FUNC _IMPORT getc_unlocked(FILE* stream);
    int _RTL_FUNC _IMPORT getchar_unlocked(void);
    int _RTL_FUNC _IMPORT putc_unlocked(int c, FILE* stream);
    int _RTL_FUNC _IMPORT putchar_unlocked(int c);

    void _RTL_FUNC _IMPORT clearerr_unlocked(FILE* stream);
    int _RTL_FUNC _IMPORT feof_unlocked(FILE* stream);
    int _RTL_FUNC _IMPORT ferror_unlocked(FILE* stream);
    int _RTL_FUNC _IMPORT fileno_unlocked(FILE* stream);
    int _RTL_FUNC _IMPORT fflush_unlocked(FILE* stream);
    int _RTL_FUNC _IMPORT fgetc_unlocked(FILE* stream);
    int _RTL_FUNC _IMPORT fputc_unlocked(int c, FILE* stream);
    size_t _RTL_FUNC _IMPORT fread_unlocked(void* ptr, size_t size, size_t n, FILE* stream);
    size_t _RTL_FUNC _IMPORT fwrite_unlocked(const void* ptr, size_t size, size_t n, FILE* stream);

    char* _RTL_FUNC _IMPORT fgets_unlocked(char* s, int n, FILE* stream);
    int _RTL_FUNC _IMPORT fputs_unlocked(const char* s, FILE* stream);
#endif
#if !defined(__CRTDLL_DLL) && !defined(__MSVCRT_DLL) && !defined(__MSIL__)
#    define fileno(f) ((f)->fd)
#    define _fileno(f) ((f)->fd)
#endif
#if defined(__MSIL__)
#    define fileno(f) _fileno(f)
#endif
#ifndef RC_INVOKED
    int _RTL_FUNC _IMPORT _fgetc(FILE* __stream);          /* used by getc() macro */
    int _RTL_FUNC _IMPORT _fputc(int __c, FILE* __stream); /* used by putc() macro */
#endif
#ifdef __cplusplus
};
#endif

    /*  The following macros provide for common functions */

#if !defined(__CRTDLL_DLL) && !defined(__MSVCRT_DLL) && !defined(__MSIL__) && !defined(__LSCRTL_DLL) && !defined(__cplusplus)
#    define ferror(f) ((f)->flags & _F_ERR)
#    define feof(f) ((f)->flags & _F_EOF)
#endif

#ifndef __cplusplus
#define getc(f) fgetc(f)
#define putc(c, f) fputc((c), f)
#endif

#pragma pack()

#endif /* __STDIO_H */
