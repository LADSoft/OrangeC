#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef _WTYPES_H
#define _WTYPES_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#include "wtypesbase.h"

#ifdef __cplusplus
extern "C"{
#endif 


extern RPC_IF_HANDLE __MIDL_itf_wtypes_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wtypes_0000_0000_v0_0_s_ifspec;

#ifndef __IWinTypes_INTERFACE_DEFINED__
#define __IWinTypes_INTERFACE_DEFINED__

typedef struct tagRemHGLOBAL {
    LONG fNullHGlobal;
    ULONG cbData;
    byte data[1];
} RemHGLOBAL;

typedef struct tagRemHMETAFILEPICT {
    LONG mm;
    LONG xExt;
    LONG yExt;
    ULONG cbData;
    byte data[1];
} RemHMETAFILEPICT;

typedef struct tagRemHENHMETAFILE {
    ULONG cbData;
    byte data[1];
} RemHENHMETAFILE;

typedef struct tagRemHBITMAP {
    ULONG cbData;
    byte data[1];
} RemHBITMAP;

typedef struct tagRemHPALETTE {
    ULONG cbData;
    byte data[1];
} RemHPALETTE;

typedef struct tagRemBRUSH {
    ULONG cbData;
    byte data[1];
} RemHBRUSH;

#ifndef _PALETTEENTRY_DEFINED
#define _PALETTEENTRY_DEFINED
typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} PALETTEENTRY, *PPALETTEENTRY, *LPPALETTEENTRY;
#endif /* _PALETTEENTRY_DEFINED */

#ifndef _LOGPALETTE_DEFINED
#define _LOGPALETTE_DEFINED
typedef struct tagLOGPALETTE {
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE, *PLOGPALETTE, *LPLOGPALETTE;
#endif /* _LOGPALETTE_DEFINED */

#ifndef _WINDEF_H
typedef const RECTL *LPCRECTL;

typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT, *LPRECT;
typedef const RECT *LPCRECT;

#endif /* _WINDEF_H */

#ifndef _ROTFLAGS_DEFINED
#define _ROTFLAGS_DEFINED
#define ROTFLAGS_REGISTRATIONKEEPSALIVE  0x1
#define ROTFLAGS_ALLOWANYCLIENT  0x2
#endif /* _ROTFLAGS_DEFINED */

#ifndef _ROT_COMPARE_MAX_DEFINED
#define _ROT_COMPARE_MAX_DEFINED
#define ROT_COMPARE_MAX  2048
#endif /* _ROT_COMPARE_MAX_DEFINED */

typedef enum tagDVASPECT {
    DVASPECT_CONTENT = 1,
    DVASPECT_THUMBNAIL = 2,
    DVASPECT_ICON = 4,
    DVASPECT_DOCPRINT = 8
} DVASPECT;

typedef enum tagSTGC {
    STGC_DEFAULT = 0,
    STGC_OVERWRITE = 1,
    STGC_ONLYIFCURRENT = 2,
    STGC_DANGEROUSLYCOMMITMERELYTODISKCACHE = 4,
    STGC_CONSOLIDATE = 8
} STGC;

typedef enum tagSTGMOVE {
    STGMOVE_MOVE = 0,
    STGMOVE_COPY = 1,
    STGMOVE_SHALLOWCOPY = 2
} STGMOVE;

typedef enum tagSTATFLAG {
    STATFLAG_DEFAULT = 0,
    STATFLAG_NONAME = 1,
    STATFLAG_NOOPEN = 2
} STATFLAG;

typedef void *HCONTEXT;

#ifndef _LCID_DEFINED
#define _LCID_DEFINED
typedef DWORD LCID;
#endif /* _LCID_DEFINED */

#ifndef _LANGID_DEFINED
#define _LANGID_DEFINED
typedef USHORT LANGID;
#endif /* _LANGID_DEFINED */

#define WDT_INPROC_CALL ( 0x48746457 )
#define WDT_REMOTE_CALL ( 0x52746457 )
#define WDT_INPROC64_CALL ( 0x50746457 )

typedef struct _userCLIPFORMAT {
    LONG fContext;
    union __MIDL_IWinTypes_0001 {
        DWORD dwValue;
        wchar_t *pwszName;
    } u;
} userCLIPFORMAT;

typedef userCLIPFORMAT *wireCLIPFORMAT;

typedef WORD CLIPFORMAT;

typedef struct _GDI_NONREMOTE {
    LONG fContext;
    union __MIDL_IWinTypes_0002 {
        LONG hInproc;
        DWORD_BLOB *hRemote;
    } u;
} GDI_NONREMOTE;

typedef struct _userHGLOBAL {
    LONG fContext;
    union __MIDL_IWinTypes_0003 {
        LONG hInproc;
        FLAGGED_BYTE_BLOB *hRemote;
        __int64 hInproc64;
    } u;
} userHGLOBAL;

typedef userHGLOBAL *wireHGLOBAL;

typedef struct _userHMETAFILE {
    LONG fContext;
    union __MIDL_IWinTypes_0004 {
        LONG hInproc;
        BYTE_BLOB *hRemote;
        __int64 hInproc64;
    } u;
} userHMETAFILE;

typedef struct _remoteMETAFILEPICT {
    LONG mm;
    LONG xExt;
    LONG yExt;
    userHMETAFILE *hMF;
} remoteMETAFILEPICT;

typedef struct _userHMETAFILEPICT {
    LONG fContext;
    union __MIDL_IWinTypes_0005 {
        LONG hInproc;
        remoteMETAFILEPICT *hRemote;
        __int64 hInproc64;
    } u;
} userHMETAFILEPICT;

typedef struct _userHENHMETAFILE {
    LONG fContext;
    union __MIDL_IWinTypes_0006 {
        LONG hInproc;
        BYTE_BLOB *hRemote;
        __int64 hInproc64;
    } u;
} userHENHMETAFILE;

typedef struct _userBITMAP {
    LONG bmType;
    LONG bmWidth;
    LONG bmHeight;
    LONG bmWidthBytes;
    WORD bmPlanes;
    WORD bmBitsPixel;
    ULONG cbSize;
    byte pBuffer[1];
} userBITMAP;

typedef struct _userHBITMAP {
    LONG fContext;
    union __MIDL_IWinTypes_0007 {
        LONG hInproc;
        userBITMAP *hRemote;
        __int64 hInproc64;
    } u;
} userHBITMAP;

typedef struct _userHPALETTE {
    LONG fContext;
    union __MIDL_IWinTypes_0008 {
        LONG hInproc;
        LOGPALETTE *hRemote;
        __int64 hInproc64;
    } u;
} userHPALETTE;

typedef struct _RemotableHandle {
    LONG fContext;
    union __MIDL_IWinTypes_0009 {
        LONG hInproc;
        LONG hRemote;
    } u;
} RemotableHandle;

typedef RemotableHandle *wireHWND;
typedef RemotableHandle *wireHMENU;
typedef RemotableHandle *wireHACCEL;
typedef RemotableHandle *wireHBRUSH;
typedef RemotableHandle *wireHFONT;
typedef RemotableHandle *wireHDC;
typedef RemotableHandle *wireHICON;
typedef RemotableHandle *wireHRGN;
typedef RemotableHandle *wireHMONITOR;

#ifndef _TEXTMETRIC_DEFINED
#define _TEXTMETRIC_DEFINED
typedef struct tagTEXTMETRICW {
    LONG tmHeight;
    LONG tmAscent;
    LONG tmDescent;
    LONG tmInternalLeading;
    LONG tmExternalLeading;
    LONG tmAveCharWidth;
    LONG tmMaxCharWidth;
    LONG tmWeight;
    LONG tmOverhang;
    LONG tmDigitizedAspectX;
    LONG tmDigitizedAspectY;
    WCHAR tmFirstChar;
    WCHAR tmLastChar;
    WCHAR tmDefaultChar;
    WCHAR tmBreakChar;
    BYTE tmItalic;
    BYTE tmUnderlined;
    BYTE tmStruckOut;
    BYTE tmPitchAndFamily;
    BYTE tmCharSet;
} TEXTMETRICW, *PTEXTMETRICW, *LPTEXTMETRICW;
#endif /* _TEXTMETRIC_DEFINED */

typedef userHBITMAP *wireHBITMAP;
typedef userHPALETTE *wireHPALETTE;
typedef userHENHMETAFILE *wireHENHMETAFILE;
typedef userHMETAFILE *wireHMETAFILE;
typedef userHMETAFILEPICT *wireHMETAFILEPICT;
typedef void *HMETAFILEPICT;

extern RPC_IF_HANDLE IWinTypes_v0_1_c_ifspec;
extern RPC_IF_HANDLE IWinTypes_v0_1_s_ifspec;
#endif /* __IWinTypes_INTERFACE_DEFINED__ */

typedef double DATE;

#ifndef _tagCY_DEFINED
#define _tagCY_DEFINED
#define _CY_DEFINED
typedef union tagCY {
    struct {
        ULONG Lo;
        LONG      Hi;
    } DUMMYSTRUCTNAME;
    LONGLONG int64;
} CY;
#endif /* _tagCY_DEFINED */
typedef CY *LPCY;

typedef struct tagDEC {
    USHORT wReserved;
    union {
        struct {
            BYTE scale;
            BYTE sign;
        } DUMMYSTRUCTNAME;
        USHORT signscale;
    } DUMMYUNIONNAME;
    ULONG Hi32;
    union {
        struct {
            ULONG Lo32;
            ULONG Mid32;
        } DUMMYSTRUCTNAME2;
        ULONGLONG Lo64;
    } DUMMYUNIONNAME2;
} DECIMAL;

#define DECIMAL_NEG  ((BYTE)0x80)
#define DECIMAL_SETZERO(dec)  {(dec).Lo64 = 0; (dec).Hi32 = 0; (dec).signscale = 0;}

typedef DECIMAL *LPDECIMAL;

typedef FLAGGED_WORD_BLOB *wireBSTR;

typedef OLECHAR *BSTR;
typedef BSTR *LPBSTR;

typedef short VARIANT_BOOL;

#define _VARIANT_BOOL  VARIANT_BOOL

#ifndef _tagBSTRBLOB_DEFINED
#define _tagBSTRBLOB_DEFINED
typedef struct tagBSTRBLOB {
    ULONG cbSize;
    BYTE *pData;
} BSTRBLOB, *LPBSTRBLOB;
#endif /* _tagBSTRBLOB_DEFINED */

#define VARIANT_TRUE  ((VARIANT_BOOL)-1)
#define VARIANT_FALSE  ((VARIANT_BOOL)0)

typedef struct tagCLIPDATA {
    ULONG cbSize;
    LONG ulClipFmt;
    BYTE *pClipData;
} CLIPDATA;

#define CBPCLIPDATA(clipdata)  ((clipdata).cbSize - sizeof((clipdata).ulClipFmt))

typedef unsigned short VARTYPE;

enum VARENUM {
    VT_EMPTY = 0,
    VT_NULL = 1,
    VT_I2 = 2,
    VT_I4 = 3,
    VT_R4 = 4,
    VT_R8 = 5,
    VT_CY = 6,
    VT_DATE = 7,
    VT_BSTR = 8,
    VT_DISPATCH = 9,
    VT_ERROR = 10,
    VT_BOOL = 11,
    VT_VARIANT = 12,
    VT_UNKNOWN = 13,
    VT_DECIMAL = 14,
    VT_I1 = 16,
    VT_UI1 = 17,
    VT_UI2 = 18,
    VT_UI4 = 19,
    VT_I8 = 20,
    VT_UI8 = 21,
    VT_INT = 22,
    VT_UINT = 23,
    VT_VOID = 24,
    VT_HRESULT = 25,
    VT_PTR = 26,
    VT_SAFEARRAY = 27,
    VT_CARRAY = 28,
    VT_USERDEFINED = 29,
    VT_LPSTR = 30,
    VT_LPWSTR = 31,
    VT_RECORD = 36,
    VT_INT_PTR = 37,
    VT_UINT_PTR = 38,
    VT_FILETIME = 64,
    VT_BLOB = 65,
    VT_STREAM = 66,
    VT_STORAGE = 67,
    VT_STREAMED_OBJECT = 68,
    VT_STORED_OBJECT = 69,
    VT_BLOB_OBJECT = 70,
    VT_CF = 71,
    VT_CLSID = 72,
    VT_VERSIONED_STREAM = 73,
    VT_BSTR_BLOB = 0xfff,
    VT_VECTOR = 0x1000,
    VT_ARRAY = 0x2000,
    VT_BYREF = 0x4000,
    VT_RESERVED = 0x8000,
    VT_ILLEGAL = 0xffff,
    VT_ILLEGALMASKED = 0xfff,
    VT_TYPEMASK = 0xfff
};

typedef ULONG PROPID;

#ifndef PROPERTYKEY_DEFINED
#define PROPERTYKEY_DEFINED
typedef struct _tagpropertykey {
    GUID fmtid;
    DWORD pid;
} PROPERTYKEY;
#endif /* PROPERTYKEY_DEFINED */

typedef struct tagCSPLATFORM {
    DWORD dwPlatformId;
    DWORD dwVersionHi;
    DWORD dwVersionLo;
    DWORD dwProcessorArch;
} CSPLATFORM;

typedef struct tagQUERYCONTEXT {
    DWORD dwContext;
    CSPLATFORM Platform;
    LCID Locale;
    DWORD dwVersionHi;
    DWORD dwVersionLo;
} QUERYCONTEXT;

typedef enum tagTYSPEC {
    TYSPEC_CLSID = 0,
    TYSPEC_FILEEXT = (TYSPEC_CLSID + 1),
    TYSPEC_MIMETYPE = (TYSPEC_FILEEXT + 1),
    TYSPEC_FILENAME = (TYSPEC_MIMETYPE + 1),
    TYSPEC_PROGID = (TYSPEC_FILENAME + 1),
    TYSPEC_PACKAGENAME = (TYSPEC_PROGID + 1),
    TYSPEC_OBJECTID = (TYSPEC_PACKAGENAME + 1)
} TYSPEC;

typedef struct __MIDL___MIDL_itf_wtypes_0000_0001_0001 {
    DWORD tyspec;
    union __MIDL___MIDL_itf_wtypes_0000_0001_0005 {
        CLSID clsid;
        LPOLESTR pFileExt;
        LPOLESTR pMimeType;
        LPOLESTR pProgId;
        LPOLESTR pFileName;
        struct {
            LPOLESTR pPackageName;
            GUID PolicyId;
        } ByName;
        struct {
            GUID ObjectId;
            GUID PolicyId;
        } ByObjectId;
    } tagged_union;
} uCLSSPEC;

extern RPC_IF_HANDLE __MIDL_itf_wtypes_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wtypes_0000_0001_v0_0_s_ifspec;


#ifdef __cplusplus
}
#endif

#endif /* _WTYPES_H */
