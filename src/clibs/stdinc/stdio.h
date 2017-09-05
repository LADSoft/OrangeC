/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*  stdio.h

    Definitions for stream input/output.

*/

#ifndef __STDIO_H
#define __STDIO_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifndef __STDARG_H
#include <stdarg.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

#pragma pack(4)
typedef struct  __file__{
        short           token;          /* Used for validity checking */
        unsigned short        flags;          /* File status flags          */
        unsigned char   hold;           /* Ungetc char if no buffer   */
        int             fd;             /* File descriptor            */
        int             level;          /* fill/empty level of buffer */
        int             bsize;          /* Buffer size                */
        unsigned char   *buffer;   /* Data transfer buffer       */
        unsigned char   *curp;     /* Current active pointer     */
        struct __file2{
            char            *name;           /* filename */
            enum { __or_unspecified,
               __or_narrow, 
               __or_wide } orient;       /* stream orientation */
            int mbstate[2];                 /* space for mbstate_t structure */
        } *extended;
}       FILE;                           /* This is the FILE object    */
#pragma pack()


#ifdef __cplusplus
}
}
#endif

#define stdin   __stdin
#define stdout  __stdout
#define stderr  __stderr
#define stdaux  __stdaux /* these two not supported now */
#define stdprn  __stdprn

#if defined(__MSIL__)
#if defined(__MANAGED__)
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stdin ;
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stdout ;
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stderr ;
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stdaux ;
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stdprn ;
#else
extern __STD_NS_QUALIFIER void * __stdin ;
extern __STD_NS_QUALIFIER void * __stdout ;
extern __STD_NS_QUALIFIER void * __stderr ;
extern __STD_NS_QUALIFIER void * __stdaux ;
extern __STD_NS_QUALIFIER void * __stdprn ;
#endif
#elif defined(__MSVCRT_DLL) || defined(__CRTDLL_DLL)
extern __STD_NS_QUALIFIER FILE * __stdin ;
extern __STD_NS_QUALIFIER FILE * __stdout ;
extern __STD_NS_QUALIFIER FILE * __stderr ;
extern __STD_NS_QUALIFIER FILE * __stdaux ;
extern __STD_NS_QUALIFIER FILE * __stdprn ;
#elif defined(__LSCRTL_DLL)
extern __STD_NS_QUALIFIER FILE _IMPORT * __stdin ;
extern __STD_NS_QUALIFIER FILE _IMPORT * __stdout ;
extern __STD_NS_QUALIFIER FILE _IMPORT * __stderr ;
extern __STD_NS_QUALIFIER FILE _IMPORT * __stdaux ;
extern __STD_NS_QUALIFIER FILE _IMPORT * __stdprn ;
#else
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stdin ;
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stdout ;
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stderr ;
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stdaux ;
extern __STD_NS_QUALIFIER FILE _RTL_DATA * __stdprn ;
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

typedef long fpos_t;

#define FILTOK	0x444c
#define STACKPAD 512

/*  "flags" bits definitions
*/
#define _F_RDWR 0x0003                  /* Read/write flag       */
#define _F_READ 0x0001                  /* Read only file        */
#define _F_WRIT 0x0002                  /* Write only file       */
#define _F_BUF  0x0004                  /* Malloc'ed Buffer data */
#define _F_LBUF 0x0008                  /* line-buffered file    */
#define _F_ERR  0x0010                  /* Error indicator       */
#define _F_EOF  0x0020                  /* End of file indicator */
#define _F_BIN  0x0040                  /* Binary file indicator */
#define _F_IN   0x0080                  /* Data is incoming      */
#define _F_OUT  0x0100                  /* Data is outgoing      */
#define _F_TERM 0x0200                  /* File is a terminal    */
#define _F_APPEND 0x400			/* Need to ignore seeks  */
#define _F_BUFFEREDSTRING 0x800       /* it is not an actual file */
#define _F_XEOF 0x1000				    /* EOF is pending based on CTRL-Z in buffered input */
#define _F_VBUF 0x2000                  /* true if setvbut allowed */
#define _F_UNGETC 0x4000				/* character was cached with ungetc */
/* End-of-file constant definition
*/
#define EOF (-1)            /* End of file indicator */

/* Default buffer size use by "setbuf" function
*/
#define BUFSIZ  512         /* Buffer size for stdio */

/* Size of an arry large enough to hold a temporary file name string
*/
#define L_ctermid   5       /* CON: plus null byte */
#define P_tmpdir    ""      /* temporary directory */
#define L_tmpnam    13      /* tmpnam buffer size */

/* Constants to be used as 3rd argument for "fseek" function
*/
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

/* Number of unique file names that shall be generated by "tmpnam" function
*/
#define TMP_MAX     32767

#define _NFILE_ 40 /* SHOULD BE SAME AS IN IO.H */

/* Definition of the control structure for streams
*/

/* Number of files that can be open simultaneously
*/
#define FOPEN_MAX (_NFILE_ - 2) /* (_NFILE_ - stdaux & stdprn) */

#define FILENAME_MAX 265

/* Standard I/O predefined streams
*/

extern  unsigned     _RTL_DATA _nfile;
FILE *   _RTL_FUNC _IMPORT __getStream(int stream) ;

#define _IOFBF  1
#define _IOLBF  2
#define _IONBF  4


void      _RTL_FUNC _IMPORT clearerr(FILE *__stream);
int       _RTL_FUNC _IMPORT fclose(FILE *__stream);
int       _RTL_FUNC _IMPORT fflush(FILE *__stream);
int       _RTL_FUNC _IMPORT fgetc(FILE *__stream);
int       _RTL_FUNC _IMPORT fgetpos(FILE *__stream, fpos_t *__pos);
char   *  _RTL_FUNC _IMPORT fgets(char *__s, int __n, FILE *__stream);
int 	  _RTL_FUNC _IMPORT fileno(FILE *);
int 	  _RTL_FUNC _IMPORT _fileno(FILE *);
FILE   *  _RTL_FUNC _IMPORT fopen(const char *__path, const char *__mode);
int       _RTL_FUNC _IMPORT fprintf(FILE *restrict __stream, const char *restrict __format, ...);
int       _RTL_FUNC _IMPORT fputc(int __c, FILE *__stream);
int       _RTL_FUNC _IMPORT fputs(const char *__s, FILE *__stream);
size_t    _RTL_FUNC _IMPORT fread(void *__ptr, size_t __size, size_t __n,
                     FILE *__stream);
FILE   *  _RTL_FUNC _IMPORT freopen(const char *__path, const char *__mode,
                            FILE *restrict __stream);
int       _RTL_FUNC _IMPORT fscanf(FILE *restrict __stream, const char *restrict __format, ...);
int       _RTL_FUNC _IMPORT fseek(FILE *__stream, long __offset, int __whence);
int       _RTL_FUNC _IMPORT fsetpos(FILE *__stream, const fpos_t *__pos);
long      _RTL_FUNC _IMPORT ftell(FILE *__stream);
size_t    _RTL_FUNC _IMPORT fwrite(const void *__ptr, size_t __size, size_t __n,
                      FILE *__stream);
char   *  _RTL_FUNC _IMPORT gets(char *__s);
void      _RTL_FUNC _IMPORT perror(const char *__s);
int       _RTL_FUNC _IMPORT printf(const char *restrict __format, ...);
int       _RTL_FUNC _IMPORT puts(const char *__s);
int       _RTL_FUNC _IMPORT remove(const char *__path);
int       _RTL_FUNC _IMPORT rename(const char *__oldname,const char *__newname);
void      _RTL_FUNC _IMPORT rewind(FILE *__stream);
int       _RTL_FUNC _IMPORT scanf(const char *__format, ...);
void      _RTL_FUNC _IMPORT setbuf(FILE *restrict __stream, char *restrict __buf);
int       _RTL_FUNC _IMPORT setvbuf(FILE *restrict __stream, char *restrict __buf,
                                   int __type, size_t __size);
int       _RTL_FUNC _IMPORT snprintf(char *restrict __buffer, size_t n, 
                                const char *restrict __format, ...);
int       _RTL_FUNC _IMPORT _snprintf(char *restrict __buffer, size_t n, 
                                const char *restrict __format, ...);
int       _RTL_FUNC _IMPORT sprintf(char *restrict __buffer, const char *restrict __format, ...);
int       _RTL_FUNC _IMPORT sscanf(const char *restrict __buffer,
                                  const char *restrict __format, ...);
char   *  _RTL_FUNC _IMPORT strerror(int __errnum);
char    * _RTL_FUNC _IMPORT tempnam(char *__dir, char *__prefix);
char    * _RTL_FUNC _IMPORT _tempnam(char *__dir, char *__prefix);
FILE   *  _RTL_FUNC _IMPORT tmpfile(void);
char   *  _RTL_FUNC _IMPORT tmpnam(char *__s);
int       _RTL_FUNC _IMPORT ungetc(int __c, FILE *__stream);
int       _RTL_FUNC _IMPORT vfprintf(FILE *restrict __stream, const char *restrict __format,
                                    va_list __arglist);
int       _RTL_FUNC _IMPORT vfscanf(FILE *restrict __stream, const char *restrict __format,
                                   va_list __arglist);
int       _RTL_FUNC _IMPORT vprintf(const char *restrict __format, va_list __arglist);
int       _RTL_FUNC _IMPORT vscanf(const char *restrict __format, va_list __arglist);
int       _RTL_FUNC _IMPORT vsnprintf(char *restrict __buffer, size_t __n,
                                    const char *restrict __format,
                                    va_list __arglist);
int       _RTL_FUNC _IMPORT _vsnprintf(char *restrict __buffer, size_t __n,
                                    const char *restrict __format,
                                    va_list __arglist);
int       _RTL_FUNC _IMPORT vsprintf(char *__buffer, const char *__format,
                                    va_list __arglist);
int       _RTL_FUNC _IMPORT vsscanf(const char *__buffer, const char *__format,
                                   va_list __arglist);
int       _RTL_FUNC _IMPORT unlink(const char *__path);
int       _RTL_FUNC _IMPORT _unlink(const char *__path);
int       _RTL_FUNC _IMPORT getc(FILE *__fp);

int       _RTL_FUNC _IMPORT getchar(void);
int       _RTL_FUNC _IMPORT putchar(const int __c);

int       _RTL_FUNC _IMPORT putc(const int __c, FILE *__fp);
int       _RTL_FUNC _IMPORT feof(FILE *__fp);
int       _RTL_FUNC _IMPORT ferror(FILE *__fp);

int       _RTL_FUNC _IMPORT fcloseall(void);
int       _RTL_FUNC _IMPORT _fcloseall(void);
FILE    * _RTL_FUNC _IMPORT fdopen(int __handle, const char *restrict __type);
FILE    * _RTL_FUNC _IMPORT _fdopen(int __handle, const char *restrict __type);
int       _RTL_FUNC _IMPORT fgetchar(void);
int       _RTL_FUNC _IMPORT _fgetchar(void);
int       _RTL_FUNC _IMPORT flushall(void);
int       _RTL_FUNC _IMPORT _flushall(void);
int       _RTL_FUNC _IMPORT fputchar(int __c);
int       _RTL_FUNC _IMPORT _fputchar(int __c);
FILE    * _RTL_FUNC _IMPORT _fsopen (const char *__path, const char *__mode,
                  int __shflag);
int       _RTL_FUNC _IMPORT getw(FILE *__stream);
int       _RTL_FUNC _IMPORT _getw(FILE *__stream);
int       _RTL_FUNC _IMPORT putw(int __w, FILE *__stream);
int       _RTL_FUNC _IMPORT _putw(int __w, FILE *__stream);
int       _RTL_FUNC _IMPORT rmtmp(void);
int		  _RTL_FUNC _IMPORT _rmtmp(void);
char    * _RTL_FUNC _IMPORT _strerror(const char *__s);

#if !defined(__CRTDLL_DLL) && !defined(__MSVCRT_DLL) && !defined(__MSIL__)
#define fileno(f)       ((f)->fd)
#define _fileno(f)		((f)->fd)
#endif
#if defined(__MSIL__)
#define fileno(f) _fileno(f)
#endif 
int       _RTL_FUNC _IMPORT _fgetc(FILE *__stream);           /* used by getc() macro */
int       _RTL_FUNC _IMPORT _fputc(int __c, FILE *__stream); /* used by putc() macro */

#if 0
/*
int _RTL_FUNC _IMPORT _filbuf(FILE *);
int _RTL_FUNC _IMPORT _flsbuf(int, FILE *);
FILE * _RTL_FUNC _IMPORT _fsopen(const char *, const char *, int);
int _RTL_FUNC _IMPORT _getmaxstdio(void);
int _RTL_FUNC _IMPORT _setmaxstdio(int);
*/
#endif

#ifdef __cplusplus
};
};
#endif

/*  The following macros provide for common functions */

#if !defined(__CRTDLL_DLL) && !defined(__MSVCRT_DLL) && !defined(__MSIL__) && !defined(__LSCRTL_DLL)
#define ferror(f)   ((f)->flags & _F_ERR)
#define feof(f)     ((f)->flags & _F_EOF)
#endif
/*
 * the following four macros are somewhat problematic as they will
 * ignore line buffering...
 */
#define getc(f) fgetc(f)
#define putc(c,f) fputc((c),f)
#define getchar()  getc(stdin)
#define putchar(c) putc((c), stdout)

#define ungetc(c,f) ungetc((c),f)   /* traditionally a macro */

#pragma pack()

#endif  /* __STDIO_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__STDIO_H_USING_LIST)
#define __STDIO_H_USING_LIST
    using __STD_NS_QUALIFIER fileno;
    using __STD_NS_QUALIFIER _fileno;
    using __STD_NS_QUALIFIER __getStream;
    using __STD_NS_QUALIFIER clearerr;
    using __STD_NS_QUALIFIER fclose;
    using __STD_NS_QUALIFIER fflush;
    using __STD_NS_QUALIFIER fgetc;
    using __STD_NS_QUALIFIER fgetpos;
    using __STD_NS_QUALIFIER fgets;
    using __STD_NS_QUALIFIER fopen;
    using __STD_NS_QUALIFIER fprintf;
    using __STD_NS_QUALIFIER fputc;
    using __STD_NS_QUALIFIER fputs;
    using __STD_NS_QUALIFIER fread;
    using __STD_NS_QUALIFIER freopen;
    using __STD_NS_QUALIFIER fscanf;
    using __STD_NS_QUALIFIER fseek;
    using __STD_NS_QUALIFIER fsetpos;
    using __STD_NS_QUALIFIER ftell;
    using __STD_NS_QUALIFIER fwrite;
    using __STD_NS_QUALIFIER gets;
    using __STD_NS_QUALIFIER perror;
    using __STD_NS_QUALIFIER printf;
    using __STD_NS_QUALIFIER puts;
    using __STD_NS_QUALIFIER remove;
    using __STD_NS_QUALIFIER rename;
    using __STD_NS_QUALIFIER rewind;
    using __STD_NS_QUALIFIER scanf;
    using __STD_NS_QUALIFIER setbuf;
    using __STD_NS_QUALIFIER setvbuf;
    using __STD_NS_QUALIFIER snprintf;
    using __STD_NS_QUALIFIER _snprintf;
    using __STD_NS_QUALIFIER sprintf;
    using __STD_NS_QUALIFIER sscanf;
    using __STD_NS_QUALIFIER strerror;
    using __STD_NS_QUALIFIER tmpfile;
    using __STD_NS_QUALIFIER tmpnam;
    using __STD_NS_QUALIFIER ungetc;
    using __STD_NS_QUALIFIER vfprintf;
    using __STD_NS_QUALIFIER vfscanf;
    using __STD_NS_QUALIFIER vprintf;
    using __STD_NS_QUALIFIER vscanf;
    using __STD_NS_QUALIFIER vsnprintf;
    using __STD_NS_QUALIFIER _vsnprintf;
    using __STD_NS_QUALIFIER vsprintf;
    using __STD_NS_QUALIFIER vsscanf;
    using __STD_NS_QUALIFIER unlink;
    using __STD_NS_QUALIFIER getc;
    using __STD_NS_QUALIFIER getchar;
    using __STD_NS_QUALIFIER putchar;
    using __STD_NS_QUALIFIER putc;
    using __STD_NS_QUALIFIER feof;
    using __STD_NS_QUALIFIER ferror;
    using __STD_NS_QUALIFIER fcloseall;
    using __STD_NS_QUALIFIER _fcloseall;
    using __STD_NS_QUALIFIER fdopen;
    using __STD_NS_QUALIFIER _fdopen;
    using __STD_NS_QUALIFIER fgetchar;
    using __STD_NS_QUALIFIER _fgetchar;
    using __STD_NS_QUALIFIER flushall;
    using __STD_NS_QUALIFIER _flushall;
    using __STD_NS_QUALIFIER fputchar;
    using __STD_NS_QUALIFIER _fputchar;
    using __STD_NS_QUALIFIER _fsopen;
    using __STD_NS_QUALIFIER getw;
    using __STD_NS_QUALIFIER putw;
    using __STD_NS_QUALIFIER rmtmp;
    using __STD_NS_QUALIFIER _strerror;
    using __STD_NS_QUALIFIER _fgetc;
    using __STD_NS_QUALIFIER _fputc;
    using __STD_NS_QUALIFIER FILE;
    using __STD_NS_QUALIFIER fpos_t;
#ifndef _USING_NFILE
#define _USING_NFILE
    using __STD_NS_QUALIFIER _nfile ;
#endif
    using __STD_NS_QUALIFIER tempnam ;
    using __STD_NS_QUALIFIER _tempnam;
#endif /* __USING_CNAME__ */

