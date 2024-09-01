#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__  500
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__  100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif

#ifndef _SHTYPES_H
#define _SHTYPES_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#include "wtypes.h"

#ifdef __cplusplus
extern "C"{
#endif 


#include <pshpack1.h>
typedef struct _SHITEMID {
    USHORT cb;
    BYTE abID[1];
} SHITEMID;
#include <poppack.h>

#if defined(_M_IX86) || defined(_M_ARM)
#define __unaligned
#endif /* __unaligned */

typedef SHITEMID __unaligned *LPSHITEMID;
typedef const SHITEMID __unaligned *LPCSHITEMID;

#include <pshpack1.h>
typedef struct _ITEMIDLIST {
    SHITEMID mkid;
} ITEMIDLIST;

#if defined(__cplusplus) && defined(STRICT_TYPED_ITEMIDS)
typedef struct _ITEMIDLIST_RELATIVE : ITEMIDLIST {} ITEMIDLIST_RELATIVE;
typedef struct _ITEMID_CHILD : ITEMIDLIST_RELATIVE {} ITEMID_CHILD;
typedef struct _ITEMIDLIST_ABSOLUTE : ITEMIDLIST_RELATIVE {} ITEMIDLIST_ABSOLUTE;
#else /* !(defined(__cplusplus) && defined(STRICT_TYPED_ITEMIDS)) */
typedef ITEMIDLIST ITEMIDLIST_RELATIVE;
typedef ITEMIDLIST ITEMID_CHILD;
typedef ITEMIDLIST ITEMIDLIST_ABSOLUTE;
#endif /* !(defined(__cplusplus) && defined(STRICT_TYPED_ITEMIDS)) */

#include <poppack.h>

typedef BYTE_BLOB *wirePIDL;
typedef ITEMIDLIST __unaligned *LPITEMIDLIST;
typedef const ITEMIDLIST __unaligned *LPCITEMIDLIST;

#if defined(STRICT_TYPED_ITEMIDS) && defined(__cplusplus)
typedef ITEMIDLIST_ABSOLUTE *PIDLIST_ABSOLUTE;
typedef const ITEMIDLIST_ABSOLUTE *PCIDLIST_ABSOLUTE;
typedef const ITEMIDLIST_ABSOLUTE __unaligned *PCUIDLIST_ABSOLUTE;
typedef ITEMIDLIST_RELATIVE *PIDLIST_RELATIVE;
typedef const ITEMIDLIST_RELATIVE *PCIDLIST_RELATIVE;
typedef ITEMIDLIST_RELATIVE __unaligned *PUIDLIST_RELATIVE;
typedef const ITEMIDLIST_RELATIVE __unaligned *PCUIDLIST_RELATIVE;
typedef ITEMID_CHILD *PITEMID_CHILD;
typedef const ITEMID_CHILD *PCITEMID_CHILD;
typedef ITEMID_CHILD __unaligned *PUITEMID_CHILD;
typedef const ITEMID_CHILD __unaligned *PCUITEMID_CHILD;
typedef const PCUITEMID_CHILD *PCUITEMID_CHILD_ARRAY;
typedef const PCUIDLIST_RELATIVE *PCUIDLIST_RELATIVE_ARRAY;
typedef const PCIDLIST_ABSOLUTE *PCIDLIST_ABSOLUTE_ARRAY;
typedef const PCUIDLIST_ABSOLUTE *PCUIDLIST_ABSOLUTE_ARRAY;
#else /* !(defined(STRICT_TYPED_ITEMIDS) && defined(__cplusplus)) */
#define PIDLIST_ABSOLUTE  LPITEMIDLIST
#define PCIDLIST_ABSOLUTE  LPCITEMIDLIST
#define PCUIDLIST_ABSOLUTE  LPCITEMIDLIST
#define PIDLIST_RELATIVE  LPITEMIDLIST
#define PCIDLIST_RELATIVE  LPCITEMIDLIST
#define PUIDLIST_RELATIVE  LPITEMIDLIST
#define PCUIDLIST_RELATIVE  LPCITEMIDLIST
#define PITEMID_CHILD  LPITEMIDLIST
#define PCITEMID_CHILD  LPCITEMIDLIST
#define PUITEMID_CHILD  LPITEMIDLIST
#define PCUITEMID_CHILD  LPCITEMIDLIST
#define PCUITEMID_CHILD_ARRAY   LPCITEMIDLIST *
#define PCUIDLIST_RELATIVE_ARRAY  LPCITEMIDLIST *
#define PCIDLIST_ABSOLUTE_ARRAY  LPCITEMIDLIST *
#define PCUIDLIST_ABSOLUTE_ARRAY  LPCITEMIDLIST *
#endif /* defined(STRICT_TYPED_ITEMIDS) && defined(__cplusplus) */

typedef enum tagSTRRET_TYPE {
    STRRET_WSTR = 0,
    STRRET_OFFSET = 0x1,
    STRRET_CSTR = 0x2
} STRRET_TYPE;

#include <pshpack8.h>
typedef struct _STRRET {
    UINT uType;
    union {
        LPWSTR pOleStr;
        UINT uOffset;
        char cStr[260];
    } DUMMYUNIONNAME;
} STRRET;

#include <poppack.h>

typedef STRRET *LPSTRRET;

#include <pshpack1.h>
typedef struct _SHELLDETAILS {
    int fmt;
    int cxChar;
    STRRET str;
} SHELLDETAILS, *LPSHELLDETAILS;
#include <poppack.h>

#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
typedef enum tagPERCEIVED {
    PERCEIVED_TYPE_FIRST    = -3,
    PERCEIVED_TYPE_CUSTOM   = -3,
    PERCEIVED_TYPE_UNSPECIFIED  = -2,
    PERCEIVED_TYPE_FOLDER   = -1,
    PERCEIVED_TYPE_UNKNOWN  = 0,
    PERCEIVED_TYPE_TEXT = 1,
    PERCEIVED_TYPE_IMAGE    = 2,
    PERCEIVED_TYPE_AUDIO    = 3,
    PERCEIVED_TYPE_VIDEO    = 4,
    PERCEIVED_TYPE_COMPRESSED   = 5,
    PERCEIVED_TYPE_DOCUMENT = 6,
    PERCEIVED_TYPE_SYSTEM   = 7,
    PERCEIVED_TYPE_APPLICATION  = 8,
    PERCEIVED_TYPE_GAMEMEDIA    = 9,
    PERCEIVED_TYPE_CONTACTS = 10,
    PERCEIVED_TYPE_LAST = 10
} PERCEIVED;

#define PERCEIVEDFLAG_UNDEFINED  0x0000
#define PERCEIVEDFLAG_SOFTCODED  0x0001
#define PERCEIVEDFLAG_HARDCODED  0x0002
#define PERCEIVEDFLAG_NATIVESUPPORT  0x0004
#define PERCEIVEDFLAG_GDIPLUS  0x0010
#define PERCEIVEDFLAG_WMSDK  0x0020
#define PERCEIVEDFLAG_ZIPFOLDER  0x0040
typedef DWORD PERCEIVEDFLAG;
#endif  /* _WIN32_IE_IE60SP2 */

#if (NTDDI_VERSION >= NTDDI_VISTA)
typedef struct _COMDLG_FILTERSPEC {
    LPCWSTR pszName;
    LPCWSTR pszSpec;
} COMDLG_FILTERSPEC;
#endif  /* NTDDI_VERSION >= NTDDI_VISTA */

typedef struct tagMACHINE_ID {
    char szName[16];
} MACHINE_ID;

typedef struct tagDOMAIN_RELATIVE_OBJECTID {
    GUID guidVolume;
    GUID guidObject;
} DOMAIN_RELATIVE_OBJECTID;

typedef GUID KNOWNFOLDERID;

#ifdef __cplusplus
#define REFKNOWNFOLDERID const KNOWNFOLDERID &
#else
#define REFKNOWNFOLDERID const KNOWNFOLDERID * /*__MIDL_CONST*/
#endif /* __cplusplus */
typedef DWORD KF_REDIRECT_FLAGS;
typedef GUID FOLDERTYPEID;

#ifdef __cplusplus
#define REFFOLDERTYPEID const FOLDERTYPEID &
#else
#define REFFOLDERTYPEID const FOLDERTYPEID * /*__MIDL_CONST*/
#endif /* __cplusplus */
typedef GUID TASKOWNERID;

#ifdef __cplusplus
#define REFTASKOWNERID const TASKOWNERID &
#else
#define REFTASKOWNERID const TASKOWNERID * /*__MIDL_CONST*/
#endif /* __cplusplus */
typedef GUID ELEMENTID;

#ifdef __cplusplus
#define REFELEMENTID const ELEMENTID &
#else
#define REFELEMENTID const ELEMENTID * /*__MIDL_CONST*/
#endif /* __cplusplus */

#ifndef LF_FACESIZE
typedef struct tagLOGFONTA {
    LONG lfHeight;
    LONG lfWidth;
    LONG lfEscapement;
    LONG lfOrientation;
    LONG lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    CHAR lfFaceName[32];
} LOGFONTA;

typedef struct tagLOGFONTW {
    LONG lfHeight;
    LONG lfWidth;
    LONG lfEscapement;
    LONG lfOrientation;
    LONG lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    WCHAR lfFaceName[32];
} LOGFONTW;

typedef LOGFONTA LOGFONT;
#endif /* LF_FACESIZE */

typedef enum tagSHCOLSTATE {
    SHCOLSTATE_TYPE_STR = 0x1,
    SHCOLSTATE_TYPE_INT = 0x2,
    SHCOLSTATE_TYPE_DATE    = 0x3,
    SHCOLSTATE_TYPEMASK = 0xf,
    SHCOLSTATE_ONBYDEFAULT  = 0x10,
    SHCOLSTATE_SLOW = 0x20,
    SHCOLSTATE_EXTENDED = 0x40,
    SHCOLSTATE_SECONDARYUI  = 0x80,
    SHCOLSTATE_HIDDEN   = 0x100,
    SHCOLSTATE_PREFER_VARCMP    = 0x200,
    SHCOLSTATE_PREFER_FMTCMP    = 0x400,
    SHCOLSTATE_NOSORTBYFOLDERNESS   = 0x800,
    SHCOLSTATE_VIEWONLY = 0x10000,
    SHCOLSTATE_BATCHREAD    = 0x20000,
    SHCOLSTATE_NO_GROUPBY   = 0x40000,
    SHCOLSTATE_FIXED_WIDTH  = 0x1000,
    SHCOLSTATE_NODPISCALE   = 0x2000,
    SHCOLSTATE_FIXED_RATIO  = 0x4000,
    SHCOLSTATE_DISPLAYMASK  = 0xf000
} SHCOLSTATE;

typedef DWORD SHCOLSTATEF;

typedef PROPERTYKEY SHCOLUMNID;

typedef const SHCOLUMNID *LPCSHCOLUMNID;

typedef enum DEVICE_SCALE_FACTOR {
    SCALE_100_PERCENT = 100,
    SCALE_120_PERCENT = 120,
    SCALE_140_PERCENT = 140,
    SCALE_150_PERCENT = 150,
    SCALE_160_PERCENT = 160,
    SCALE_180_PERCENT = 180,
    SCALE_225_PERCENT = 225
} DEVICE_SCALE_FACTOR;

extern RPC_IF_HANDLE __MIDL_itf_shtypes_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shtypes_0000_0000_v0_0_s_ifspec;


#ifdef __cplusplus
}
#endif

#endif /* _SHTYPES_H */
