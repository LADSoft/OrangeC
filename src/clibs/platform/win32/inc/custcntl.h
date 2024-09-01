#ifndef _CUSTCNTL_H
#define _CUSTCNTL_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Custom Control Library definitions */

#define CCHCCCLASS  32
#define CCHCCDESC  32
#define CCHCCTEXT  256

#define CCF_NOTEXT  0x00000001

typedef struct tagCCSTYLEA {
    DWORD flStyle;
    DWORD flExtStyle;
    CHAR szText[CCHCCTEXT];
    LANGID lgid;
    WORD wReserved1;
} CCSTYLEA, *LPCCSTYLEA;

typedef struct tagCCSTYLEW {
    DWORD flStyle;
    DWORD flExtStyle;
    WCHAR szText[CCHCCTEXT];
    LANGID lgid;
    WORD wReserved1;
} CCSTYLEW, *LPCCSTYLEW;

typedef BOOL (CALLBACK *LPFNCCSTYLEA)(HWND, LPCCSTYLEA);
typedef BOOL (CALLBACK *LPFNCCSTYLEW)(HWND, LPCCSTYLEW);

typedef INT (CALLBACK *LPFNCCSIZETOTEXTA)(DWORD, DWORD, HFONT, LPSTR);
typedef INT (CALLBACK *LPFNCCSIZETOTEXTW)(DWORD, DWORD, HFONT, LPWSTR);

typedef struct tagCCSTYLEFLAGA {
    DWORD flStyle;
    DWORD flStyleMask;
    LPSTR pszStyle;
} CCSTYLEFLAGA, *LPCCSTYLEFLAGA;

typedef struct tagCCSTYLEFLAGW {
    DWORD flStyle;
    DWORD flStyleMask;
    LPWSTR pszStyle;
} CCSTYLEFLAGW, *LPCCSTYLEFLAGW;

typedef struct tagCCINFOA {
    CHAR szClass[CCHCCCLASS];
    DWORD flOptions;
    CHAR szDesc[CCHCCDESC];
    UINT cxDefault;
    UINT cyDefault;
    DWORD flStyleDefault;
    DWORD flExtStyleDefault;
    DWORD flCtrlTypeMask;
    CHAR szTextDefault[CCHCCTEXT];
    INT cStyleFlags;
    LPCCSTYLEFLAGA aStyleFlags;
    LPFNCCSTYLEA lpfnStyle;
    LPFNCCSIZETOTEXTA lpfnSizeToText;
    DWORD dwReserved1;
    DWORD dwReserved2;
} CCINFOA, *LPCCINFOA;

typedef struct tagCCINFOW {
    WCHAR szClass[CCHCCCLASS];
    DWORD flOptions;
    WCHAR szDesc[CCHCCDESC];
    UINT cxDefault;
    UINT cyDefault;
    DWORD flStyleDefault;
    DWORD flExtStyleDefault;
    DWORD flCtrlTypeMask;
    INT cStyleFlags;
    LPCCSTYLEFLAGW aStyleFlags;
    WCHAR szTextDefault[CCHCCTEXT];
    LPFNCCSTYLEW lpfnStyle;
    LPFNCCSIZETOTEXTW lpfnSizeToText;
    DWORD dwReserved1;
    DWORD dwReserved2;
} CCINFOW, *LPCCINFOW;

typedef UINT (CALLBACK *LPFNCCINFOA)(LPCCINFOA);
typedef UINT (CALLBACK *LPFNCCINFOW)(LPCCINFOW);

#ifdef UNICODE
#define CCSTYLE CCSTYLEW
#define LPCCSTYLE LPCCSTYLEW
#define LPFNCCSTYLE LPFNCCSTYLEW
#define LPFNCCSIZETOTEXT LPFNCCSIZETOTEXTW
#define CCSTYLEFLAG CCSTYLEFLAGW
#define LPCCSTYLEFLAG LPCCSTYLEFLAGW
#define CCINFO CCINFOW
#define LPCCINFO LPCCINFOW
#define LPFNCCINFO LPFNCCINFOW
#else /* !UNICODE */
#define CCSTYLE CCSTYLEA
#define LPCCSTYLE LPCCSTYLEA
#define LPFNCCSTYLE LPFNCCSTYLEA
#define LPFNCCSIZETOTEXT LPFNCCSIZETOTEXTA
#define CCSTYLEFLAG CCSTYLEFLAGA
#define LPCCSTYLEFLAG LPCCSTYLEFLAGA
#define CCINFO CCINFOA
#define LPCCINFO LPCCINFOA
#define LPFNCCINFO LPFNCCINFOA
#endif /* !UNICODE */

#endif /* _CUSTCNTL_H */
