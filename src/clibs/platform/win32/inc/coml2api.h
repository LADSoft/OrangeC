#ifndef _COML2API_H
#define _COML2API_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* #include <apiset.h> */
#include <apisetcconv.h>

/* ApiSet api-ms-win-core-com-l2 */

#include <combaseapi.h>
#include <objidl.h>
#include <propidlbase.h>

#define CWCSTORAGENAME  32

#define STGM_DIRECT             0x00000000L
#define STGM_TRANSACTED         0x00010000L
#define STGM_SIMPLE             0x08000000L

#define STGM_READ               0x00000000L
#define STGM_WRITE              0x00000001L
#define STGM_READWRITE          0x00000002L

#define STGM_SHARE_DENY_NONE    0x00000040L
#define STGM_SHARE_DENY_READ    0x00000030L
#define STGM_SHARE_DENY_WRITE   0x00000020L
#define STGM_SHARE_EXCLUSIVE    0x00000010L

#define STGM_PRIORITY           0x00040000L
#define STGM_DELETEONRELEASE    0x04000000L

#if (WINVER >= 400)
#define STGM_NOSCRATCH          0x00100000L
#endif /* WINVER */

#define STGM_CREATE             0x00001000L
#define STGM_CONVERT            0x00020000L
#define STGM_FAILIFTHERE        0x00000000L

#define STGM_NOSNAPSHOT         0x00200000L

#if (_WIN32_WINNT >= 0x0500)
#define STGM_DIRECT_SWMR        0x00400000L
#endif /* _WIN32_WINNT >= 0x0500 */

typedef DWORD STGFMT;

#define STGFMT_STORAGE   0
#define STGFMT_NATIVE    1
#define STGFMT_FILE      3
#define STGFMT_ANY       4
#define STGFMT_DOCFILE   5

#define STGFMT_DOCUMENT  0

WINOLEAPI StgCreateDocfile(const WCHAR *, DWORD, DWORD, IStorage **);
WINOLEAPI StgCreateDocfileOnILockBytes(ILockBytes *, DWORD, DWORD, IStorage **);
WINOLEAPI StgOpenStorage(const WCHAR *, IStorage *, DWORD, SNB, DWORD, IStorage **);
WINOLEAPI StgOpenStorageOnILockBytes(ILockBytes *, IStorage *, DWORD, SNB, DWORD, IStorage **);
WINOLEAPI StgIsStorageFile(const WCHAR *);
WINOLEAPI StgIsStorageILockBytes(ILockBytes *);
WINOLEAPI  StgSetTimes(const WCHAR *, const FILETIME *, const FILETIME *, const FILETIME *);

#if _WIN32_WINNT == 0x500
#define STGOPTIONS_VERSION  1
#elif _WIN32_WINNT > 0x500
#define STGOPTIONS_VERSION  2
#else
#define STGOPTIONS_VERSION  0
#endif

typedef struct tagSTGOPTIONS {
    USHORT usVersion;
    USHORT reserved;
    ULONG ulSectorSize;
#if STGOPTIONS_VERSION >= 2
    const WCHAR *pwcsTemplateFile;
#endif /* STGOPTIONS_VERSION >= 2 */
} STGOPTIONS;

WINOLEAPI StgCreateStorageEx(const WCHAR *, DWORD, DWORD, DWORD, STGOPTIONS *, PSECURITY_DESCRIPTOR, REFIID, void **);
WINOLEAPI StgOpenStorageEx(const WCHAR *, DWORD, DWORD, DWORD, STGOPTIONS *, PSECURITY_DESCRIPTOR, REFIID, void **);

#ifndef _STGCREATEPROPSTG_DEFINED_

#define CCH_MAX_PROPSTG_NAME  31

WINOLEAPI StgCreatePropStg(IUnknown *, REFFMTID, const CLSID *, DWORD, DWORD, IPropertyStorage **);
WINOLEAPI StgOpenPropStg(IUnknown *, REFFMTID, DWORD, DWORD, IPropertyStorage **);
WINOLEAPI StgCreatePropSetStg(IStorage *, DWORD, IPropertySetStorage **);
WINOLEAPI FmtIdToPropStgName(const FMTID *, LPOLESTR);
WINOLEAPI PropStgNameToFmtId(const LPOLESTR, FMTID *);

#endif /* _STGCREATEPROPSTG_DEFINED_ */

WINOLEAPI ReadClassStg(LPSTORAGE, CLSID *);
WINOLEAPI WriteClassStg(LPSTORAGE, REFCLSID);
WINOLEAPI ReadClassStm(LPSTREAM, CLSID *);
WINOLEAPI WriteClassStm(LPSTREAM, REFCLSID);
WINOLEAPI GetHGlobalFromILockBytes(LPLOCKBYTES, HGLOBAL *);
WINOLEAPI CreateILockBytesOnHGlobal(HGLOBAL, BOOL, LPLOCKBYTES *);

WINOLEAPI GetConvertStg(LPSTORAGE);

#endif /* _COML2API_H */
