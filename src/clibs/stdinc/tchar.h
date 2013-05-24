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
/*
 *      C/C++ Run Time Library - Version 7.0
 *
 *      Copyright (c) 1995, 1996 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __TCHAR_H
#define __TCHAR_H

#ifdef  __cplusplus
extern "C" {
#endif

/* warning... there isn't a full unicode version of the lib... */
#define _tWinMain       WinMain
#define _targv          _argv
#define _tenviron       _environ
#define _tfullpath      _fullpath
#define _tmakepath      _makepath
#define _tpopen         _popen
#define _tsplitpath     _splitpath
#define _tstrdate       _strdate
#define _tstrtime       _strtime
#define _taccess        access
#define _tasctime       asctime
#define _tchdir         chdir
#define _tchmod         chmod
#define _tcreat         creat
#define _tctime         ctime
#define _texecl         execl
#define _texecle        execle
#define _texeclp        execlp
#define _texeclpe       execlpe
#define _texecv         execv
#define _texecve        execve
#define _texecvp        execvp
#define _texecvpe       execvpe
#define _tfdopen        fdopen
#define _tfindfirst     findfirst
#define _tfindnext      findnext
#define _tfopen         fopen
#define _tfreopen       freopen
#define _tfsopen        fsopen
#define _tgetcwd        getcwd
#define _tgetenv        getenv
#define _tmain          main
#define _tmkdir         mkdir
#define _tmktemp        mktemp
#define _topen          open
#define _tperror        perror
#define _tputenv        putenv
#define _tremove        remove
#define _trename        rename
#define _trmdir         rmdir
#define _tsetlocale     setlocale
#define _tsopen         sopen
#define _tspawnl        spawnl
#define _tspawnle       spawnle
#define _tspawnlp       spawnlp
#define _tspawnlpe      spawnlpe
#define _tspawnv        spawnv
#define _tspawnve       spawnve
#define _tspawnvp       spawnvp
#define _tspawnvpe      spawnvpe
#define _tstat          stat
#define _tsystem        system
#define _ttempnam       tempnam
#define _ttmpnam        tmpnam
#define _ungettc        ungetc
#define _tunlink        unlink
#define _tutime         utime

#define strinc(a)       ((a)+1)
#define strdec(a, b)    ((b)-1)
#define strnextc(a)     ((unsigned int) *(a))
#define strninc(a, b)   ((a)+(b))
#define strncnt(a, b)   ((strlen(a)>b) ? b : strlen(a))
#define strspnp(a, b)  ((*((a)+strspn(a,b))) ? ((a)+strspn(a,b)) : NULL)
#define wcsncnt(a, b)   ((wcslen(a)>b) ? b : wcslen(a))
#define wcsspnp(a, b)  ((*((a)+wcsspn(a,b))) ? ((a)+wcsspn(a,b)) : NULL)

#define _tcsdec         strdec
#define _tcsinc         strinc
#define _tcsnextc       strnextc
#define _tcsnbcnt       strncnt

#define _tclen(a)       (1)
#define _tccpy(a,b)     ((*(a))=(*(b)))
#define _tccmp(a,b)     ((*(a))-(*(b)))

#define _istlegal(a)    (1)
#define _istlead(a)     (0)
#define _istleadbyte(a) (0)

#if defined(_UNICODE)

#define _ttoi           wtoi
#define _ttol           wtol
#define _fgettc         fgetwc
#define _fgettchar      fgetwchar
#define _fgetts         fgetws
#define _ftprintf       fwprintf
#define _fputtc         fputwc
#define _fputtchar      fputwchar
#define _fputts         fputws
#define _ftscanf        fwscanf
#define _gettc          getwc
#define _gettchar       getwchar
#define _getts          getws
#define _istalnum       iswalnum
#define _istalpha       iswalpha
#define _istascii       iswascii
#define _istcntrl       iswcntrl
#define _istdigit       iswdigit
#define _istgraph       iswgraph
#define _istlower       iswlower
#define _istprint       iswprint
#define _istpunct       iswpunct
#define _istspace       iswspace
#define _istupper       iswupper
#define _istxdigit      iswxdigit
#define _itot           itow
#define _ltot           ltow
#define _tprintf        wprintf
#define _puttc          putwc
#define _puttchar       putwchar
#define _putts          putws
#define _tscanf         wscanf
#define _stprintf       wsprintf
#define _stscanf        wsscanf
#define _tcscat         wcscat
#define _tcschr         wcschr
#define _tcscmp         wcscmp
#define _tcscoll        wcscoll
#define _tcscpy         wcscpy
#define _tcscspn        wcscspn
#define _tcsdup         wcsdup
#define _tcsftime       wcsftime
#define _tcsicmp        wcsicmp
#define _tcslen         wcslen
#define _tcslwr         wcslwr
#define _tcsncmp        wcsncmp
#define _tcsncoll       wcsncoll
#define _tcsncpy        wcsncpy
#define _tcsnicmp       wcsnicmp
#define _tcsninc        wcsninc
#define _tcspbrk        wcspbrk
#define _tcsrchr        wcsrchr
#define _tcsrev         wcsrev
#define _tcsset         wcsset
#define _tcsspn         wcsspn
#define _tcsspnp        wcsspnp
#define _tcsstr         wcsstr
#define _tcstod         wcstod
#define _tcstok         wcstok
#define _tcstol         wcstol
#define _tcstoul        wcstoul
#define _tcsupr         wcsupr
#define _tcsxfrm        wcsxfrm
#define _totlower       towlower
#define _totupper       towupper
#define _ultot          ultow
#define _vftprintf      vfwprintf
#define _vtprintf       vwprintf
#define _vstprintf      vswprintf

#ifndef __TCHAR_DEFINED
typedef wchar_t         _TCHAR;
typedef wchar_t         _TSCHAR;
typedef wchar_t         _TUCHAR;
typedef wchar_t         _TXCHAR;
typedef unsigned __int64  _TINT;
#define __TCHAR_DEFINED
#endif

#define _TEOF           WEOF
#define __T(x)          L ## x

#else

#define _ttoi           atoi
#define _ttol           atol
#define _fgettc         fgetc
#define _fgettchar      fgetchar
#define _fgetts         fgets
#define _ftprintf       fprintf
#define _fputtc         fputc
#define _fputtchar      fputchar
#define _fputts         fputs
#define _ftscanf        fscanf
#define _gettc          getc
#define _gettchar       getchar
#define _getts          gets
#define _istalnum       isalnum
#define _istalpha       isalpha
#define _istascii       isascii
#define _istcntrl       iscntrl
#define _istdigit       isdigit
#define _istgraph       isgraph
#define _istlower       islower
#define _istprint       isprint
#define _istpunct       ispunct
#define _istspace       isspace
#define _istupper       isupper
#define _istxdigit      isxdigit
#define _itot           itoa
#define _ltot           ltoa
#define _tprintf        printf
#define _puttc          putc
#define _puttchar       putchar
#define _putts          puts
#define _tscanf         scanf
#define _stprintf       sprintf
#define _stscanf        sscanf
#define _tcscat         strcat
#define _tcschr         strchr
#define _tcscmp         strcmp
#define _tcscoll        strcoll
#define _tcscpy         strcpy
#define _tcscspn        strcspn
#define _tcsdup         strdup
#define _tcsftime       strftime
#define _tcsicmp        stricmp
#define _tcslen         strlen
#define _tcslwr         strlwr
#define _tcsncmp        strncmp
#define _tcsncoll       strncoll
#define _tcsncpy        strncpy
#define _tcsnicmp       strnicmp
#define _tcspbrk        strpbrk
#define _tcsrchr        strrchr
#define _tcsrev         strrev
#define _tcsset         strset
#define _tcsspn         strspn
#define _tcsspnp        strspnp
#define _tcsstr         strstr
#define _tcstod         strtod
#define _tcstok         strtok
#define _tcstol         strtol
#define _tcstoul        strtoul
#define _tcsupr         strupr
#define _tcsxfrm        strxfrm
#define _totlower       tolower
#define _totupper       toupper
#define _ultot          ultoa
#define _vftprintf      vfprintf
#define _vtprintf       vprintf
#define _vstprintf      vsprintf

#ifndef __TCHAR_DEFINED
typedef char            _TCHAR;
typedef signed char     _TSCHAR;
typedef unsigned char   _TUCHAR;
typedef char            _TXCHAR;
typedef int             _TINT;
#define __TCHAR_DEFINED
#endif

#define _TEOF           EOF
#define __T(x)          x

#endif 	/* _UNICODE */

#define _TEXT(x)        __T(x)
#define _T(x)           __T(x)

#ifdef __cplusplus
}
#endif

#endif	/* __TCHAR_H */
