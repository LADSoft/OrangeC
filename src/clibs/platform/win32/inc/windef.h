#ifndef _WINDEF_H
#define _WINDEF_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Basic Type Definitions */

#ifdef __cplusplus
extern "C" {
#endif

#include <minwindef.h>

#ifndef WINVER
#define WINVER  0x0500
#endif /* WINVER */

#ifndef NT_INCLUDED
#include <winnt.h>
#endif /* NT_INCLUDED */

DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HHOOK);

#ifdef WINABLE
DECLARE_HANDLE(HEVENT);
#endif /* WINABLE */

#ifdef STRICT
typedef void *HGDIOBJ;
#else /* !STRICT */
DECLARE_HANDLE(HGDIOBJ);
#endif /* !STRICT */

DECLARE_HANDLE(HACCEL);
DECLARE_HANDLE(HBITMAP);
DECLARE_HANDLE(HBRUSH);
#if(WINVER >= 0x0400)
DECLARE_HANDLE(HCOLORSPACE);
#endif /* WINVER >= 0x0400 */
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HGLRC);
DECLARE_HANDLE(HDESK);
DECLARE_HANDLE(HENHMETAFILE);
DECLARE_HANDLE(HFONT);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HPALETTE);
DECLARE_HANDLE(HPEN);

#if(WINVER >= 0x0400)
DECLARE_HANDLE(HWINEVENTHOOK);
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
DECLARE_HANDLE(HMONITOR);
DECLARE_HANDLE(HUMPD);
#endif /* WINVER >= 0x0500 */

typedef HICON HCURSOR;      /* HICONs & HCURSORs are polymorphic */

typedef DWORD COLORREF;
typedef DWORD *LPCOLORREF;

#define HFILE_ERROR  ((HFILE)-1)

typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT,  *PRECT, *NPRECT, *LPRECT;
typedef const RECT *LPCRECT;

typedef struct _RECTL {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECTL, *PRECTL, *LPRECTL;
typedef const RECTL *LPCRECTL;

typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT,  *PPOINT, *NPPOINT, *LPPOINT;

typedef struct _POINTL {
    LONG x;
    LONG y;
} POINTL, *PPOINTL;

typedef struct tagSIZE {
    LONG cx;
    LONG cy;
} SIZE,  *PSIZE, *LPSIZE;

typedef SIZE SIZEL;
typedef SIZE *PSIZEL, *LPSIZEL;

typedef struct tagPOINTS {
    SHORT   x;
    SHORT   y;
} POINTS, *PPOINTS, *LPPOINTS;

#define DM_UPDATE  1
#define DM_COPY    2
#define DM_PROMPT  4
#define DM_MODIFY  8

#define DM_IN_BUFFER    DM_MODIFY
#define DM_IN_PROMPT    DM_PROMPT
#define DM_OUT_BUFFER   DM_COPY
#define DM_OUT_DEFAULT  DM_UPDATE

#define DC_FIELDS  1
#define DC_PAPERS  2
#define DC_PAPERSIZE  3
#define DC_MINEXTENT  4
#define DC_MAXEXTENT  5
#define DC_BINS  6
#define DC_DUPLEX  7
#define DC_SIZE  8
#define DC_EXTRA  9
#define DC_VERSION  10
#define DC_DRIVER  11
#define DC_BINNAMES  12
#define DC_ENUMRESOLUTIONS  13
#define DC_FILEDEPENDENCIES  14
#define DC_TRUETYPE  15
#define DC_PAPERNAMES  16
#define DC_ORIENTATION  17
#define DC_COPIES  18

#ifdef __cplusplus
}
#endif

#define _DPI_AWARENESS_CONTEXTS_

DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);

typedef enum DPI_AWARENESS {
    DPI_AWARENESS_INVALID = -1,
    DPI_AWARENESS_UNAWARE = 0,
    DPI_AWARENESS_SYSTEM_AWARE = 1,
    DPI_AWARENESS_PER_MONITOR_AWARE = 2
} DPI_AWARENESS;

#define DPI_AWARENESS_CONTEXT_UNAWARE            ((DPI_AWARENESS_CONTEXT)-1)
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE       ((DPI_AWARENESS_CONTEXT)-2)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE  ((DPI_AWARENESS_CONTEXT)-3)

#endif /* _WINDEF_H */
