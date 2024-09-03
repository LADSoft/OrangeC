#ifndef _LZEXPAND_H
#define _LZEXPAND_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Data decompression library definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define LZERROR_BADINHANDLE  (-1)
#define LZERROR_BADOUTHANDLE  (-2)
#define LZERROR_READ  (-3)
#define LZERROR_WRITE  (-4)
#define LZERROR_GLOBALLOC  (-5)
#define LZERROR_GLOBLOCK  (-6)
#define LZERROR_BADVALUE  (-7)
#define LZERROR_UNKNOWNALG  (-8)

INT WINAPI LZStart(void);
void WINAPI LZDone(void);
LONG WINAPI CopyLZFile(INT,INT);
LONG WINAPI LZCopy(INT,INT);
INT WINAPI LZInit(INT);
INT WINAPI GetExpandedNameA(LPSTR,LPSTR);
INT WINAPI GetExpandedNameW(LPWSTR,LPWSTR);
INT WINAPI LZOpenFileA(LPSTR,LPOFSTRUCT,WORD);
INT WINAPI LZOpenFileW(LPWSTR,LPOFSTRUCT,WORD);
LONG WINAPI LZSeek(INT,LONG,INT);
INT WINAPI LZRead(INT,LPSTR,INT);
void APIENTRY LZClose(INT);

#ifdef UNICODE
#define GetExpandedName GetExpandedNameW
#define LZOpenFile LZOpenFileW
#else
#define GetExpandedName GetExpandedNameA
#define LZOpenFile LZOpenFileA
#endif

#ifdef __cplusplus
}
#endif

#endif /* _LZEXPAND_H */
