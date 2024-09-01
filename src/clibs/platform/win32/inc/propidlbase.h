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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef _PROPIDLBASE_H
#define _PROPIDLBASE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IPropertyStorage_FWD_DEFINED__
#define __IPropertyStorage_FWD_DEFINED__
typedef interface IPropertyStorage IPropertyStorage;
#endif /* __IPropertyStorage_FWD_DEFINED__ */

#ifndef __IPropertySetStorage_FWD_DEFINED__
#define __IPropertySetStorage_FWD_DEFINED__
typedef interface IPropertySetStorage IPropertySetStorage;
#endif /* __IPropertySetStorage_FWD_DEFINED__ */

#ifndef __IEnumSTATPROPSTG_FWD_DEFINED__
#define __IEnumSTATPROPSTG_FWD_DEFINED__
typedef interface IEnumSTATPROPSTG IEnumSTATPROPSTG;
#endif /* __IEnumSTATPROPSTG_FWD_DEFINED__ */

#ifndef __IEnumSTATPROPSETSTG_FWD_DEFINED__
#define __IEnumSTATPROPSETSTG_FWD_DEFINED__
typedef interface IEnumSTATPROPSETSTG IEnumSTATPROPSETSTG;
#endif /* __IEnumSTATPROPSETSTG_FWD_DEFINED__ */

#include "objidl.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* #include <winapifamily.h> */

#ifndef _PROPIDLBASE_

typedef struct tagVersionedStream {
    GUID guidVersion;
    IStream *pStream;
} VERSIONEDSTREAM, *LPVERSIONEDSTREAM;

#define PROPSETFLAG_DEFAULT  (0)
#define PROPSETFLAG_NONSIMPLE  (1)
#define PROPSETFLAG_ANSI  (2)
#define PROPSETFLAG_UNBUFFERED  (4)
#define PROPSETFLAG_CASE_SENSITIVE  (8)

#define PROPSET_BEHAVIOR_CASE_SENSITIVE  (1)

typedef struct tagPROPVARIANT PROPVARIANT;

typedef struct tagCAC {
    ULONG cElems;
    CHAR *pElems;
} CAC;

typedef struct tagCAUB {
    ULONG cElems;
    UCHAR *pElems;
} CAUB;

typedef struct tagCAI {
    ULONG cElems;
    SHORT *pElems;
} CAI;

typedef struct tagCAUI {
    ULONG cElems;
    USHORT *pElems;
} CAUI;

typedef struct tagCAL {
    ULONG cElems;
    LONG *pElems;
} CAL;

typedef struct tagCAUL {
    ULONG cElems;
    ULONG *pElems;
} CAUL;

typedef struct tagCAFLT {
    ULONG cElems;
    FLOAT *pElems;
} CAFLT;

typedef struct tagCADBL {
    ULONG cElems;
    DOUBLE *pElems;
} CADBL;

typedef struct tagCACY {
    ULONG cElems;
    CY *pElems;
} CACY;

typedef struct tagCADATE {
    ULONG cElems;
    DATE *pElems;
} CADATE;

typedef struct tagCABSTR {
    ULONG cElems;
    BSTR *pElems;
} CABSTR;

typedef struct tagCABSTRBLOB {
    ULONG cElems;
    BSTRBLOB *pElems;
} CABSTRBLOB;

typedef struct tagCABOOL {
    ULONG cElems;
    VARIANT_BOOL *pElems;
} CABOOL;

typedef struct tagCASCODE {
    ULONG cElems;
    SCODE *pElems;
} CASCODE;

typedef struct tagCAPROPVARIANT {
    ULONG cElems;
    PROPVARIANT *pElems;
} CAPROPVARIANT;

typedef struct tagCAH {
    ULONG cElems;
    LARGE_INTEGER *pElems;
} CAH;

typedef struct tagCAUH {
    ULONG cElems;
    ULARGE_INTEGER *pElems;
} CAUH;

typedef struct tagCALPSTR {
    ULONG cElems;
    LPSTR *pElems;
} CALPSTR;

typedef struct tagCALPWSTR {
    ULONG cElems;
    LPWSTR *pElems;
} CALPWSTR;

typedef struct tagCAFILETIME {
    ULONG cElems;
    FILETIME *pElems;
} CAFILETIME;

typedef struct tagCACLIPDATA {
    ULONG cElems;
    CLIPDATA *pElems;
} CACLIPDATA;

typedef struct tagCACLSID {
    ULONG cElems;
    CLSID *pElems;
} CACLSID;

typedef WORD PROPVAR_PAD1;
typedef WORD PROPVAR_PAD2;
typedef WORD PROPVAR_PAD3;
#define tag_inner_PROPVARIANT

#if !defined(_MSC_EXTENSIONS)
struct tagPROPVARIANT;
#else /* _MSC_EXTENSIONS */
struct tagPROPVARIANT {
    union {
        struct tag_inner_PROPVARIANT {
            VARTYPE vt;
            PROPVAR_PAD1 wReserved1;
            PROPVAR_PAD2 wReserved2;
            PROPVAR_PAD3 wReserved3;
            union {
                CHAR cVal;
                UCHAR bVal;
                SHORT iVal;
                USHORT uiVal;
                LONG lVal;
                ULONG ulVal;
                INT intVal;
                UINT uintVal;
                LARGE_INTEGER hVal;
                ULARGE_INTEGER uhVal;
                FLOAT fltVal;
                DOUBLE dblVal;
                VARIANT_BOOL boolVal;
                _VARIANT_BOOL bool;
                SCODE scode;
                CY cyVal;
                DATE date;
                FILETIME filetime;
                CLSID *puuid;
                CLIPDATA *pclipdata;
                BSTR bstrVal;
                BSTRBLOB bstrblobVal;
                BLOB blob;
                LPSTR pszVal;
                LPWSTR pwszVal;
                IUnknown *punkVal;
                IDispatch *pdispVal;
                IStream *pStream;
                IStorage *pStorage;
                LPVERSIONEDSTREAM pVersionedStream;
                LPSAFEARRAY parray;
                CAC cac;
                CAUB caub;
                CAI cai;
                CAUI caui;
                CAL cal;
                CAUL caul;
                CAH cah;
                CAUH cauh;
                CAFLT caflt;
                CADBL cadbl;
                CABOOL cabool;
                CASCODE cascode;
                CACY cacy;
                CADATE cadate;
                CAFILETIME cafiletime;
                CACLSID cauuid;
                CACLIPDATA caclipdata;
                CABSTR cabstr;
                CABSTRBLOB cabstrblob;
                CALPSTR calpstr;
                CALPWSTR calpwstr;
                CAPROPVARIANT capropvar;
                CHAR *pcVal;
                UCHAR *pbVal;
                SHORT *piVal;
                USHORT *puiVal;
                LONG *plVal;
                ULONG *pulVal;
                INT *pintVal;
                UINT *puintVal;
                FLOAT *pfltVal;
                DOUBLE *pdblVal;
                VARIANT_BOOL *pboolVal;
                DECIMAL *pdecVal;
                SCODE *pscode;
                CY *pcyVal;
                DATE *pdate;
                BSTR *pbstrVal;
                IUnknown **ppunkVal;
                IDispatch **ppdispVal;
                LPSAFEARRAY *pparray;
                PROPVARIANT *pvarVal;
            };
        };
        DECIMAL decVal;
    };
};
#endif /* _MSC_EXTENSIONS */

typedef struct tagPROPVARIANT * LPPROPVARIANT;

#ifndef _REFPROPVARIANT_DEFINED
#define _REFPROPVARIANT_DEFINED
#define REFPROPVARIANT const PROPVARIANT * __MIDL_CONST
#endif /* _REFPROPVARIANT_DEFINED */

#define PID_DICTIONARY  (0)

#define PID_CODEPAGE  (0x1)

#define PID_FIRST_USABLE  (0x2)

#define PID_FIRST_NAME_DEFAULT  (0xfff)

#define PID_LOCALE  (0x80000000)
#define PID_MODIFY_TIME  (0x80000001)
#define PID_SECURITY  (0x80000002)
#define PID_BEHAVIOR  (0x80000003)
#define PID_ILLEGAL  (0xffffffff)

#define PID_MIN_READONLY  (0x80000000)
#define PID_MAX_READONLY  (0xbfffffff)

#define PRSPEC_INVALID  (0xffffffff)
#define PRSPEC_LPWSTR  (0)
#define PRSPEC_PROPID  (1)

typedef struct tagPROPSPEC {
    ULONG ulKind;
    union {
        PROPID propid;
        LPOLESTR lpwstr;
    } DUMMYUNIONNAME;
} PROPSPEC;

typedef struct tagSTATPROPSTG {
    LPOLESTR lpwstrName;
    PROPID propid;
    VARTYPE vt;
} STATPROPSTG;

#define PROPSETHDR_OSVER_KIND(dwOSVer)   HIWORD((dwOSVer))
#define PROPSETHDR_OSVER_MAJOR(dwOSVer)  LOBYTE(LOWORD((dwOSVer)))
#define PROPSETHDR_OSVER_MINOR(dwOSVer)  HIBYTE(LOWORD((dwOSVer)))
#define PROPSETHDR_OSVERSION_UNKNOWN     0xFFFFFFFF

typedef struct tagSTATPROPSETSTG {
    FMTID fmtid;
    CLSID clsid;
    DWORD grfFlags;
    FILETIME mtime;
    FILETIME ctime;
    FILETIME atime;
    DWORD dwOSVersion;
} STATPROPSETSTG;

extern RPC_IF_HANDLE __MIDL_itf_propidlbase_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_propidlbase_0000_0000_v0_0_s_ifspec;

#ifndef __IPropertyStorage_INTERFACE_DEFINED__
#define __IPropertyStorage_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPropertyStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("00000138-0000-0000-C000-000000000046")
IPropertyStorage:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ReadMultiple(ULONG cpspec, const PROPSPEC rgpspec[], PROPVARIANT rgpropvar[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteMultiple(ULONG cpspec, const PROPSPEC rgpspec[], const PROPVARIANT rgpropvar[], PROPID propidNameFirst) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteMultiple(ULONG cpspec, const PROPSPEC rgpspec[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReadPropertyNames(ULONG cpropid, const PROPID rgpropid[], LPOLESTR rglpwstrName[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE WritePropertyNames(ULONG cpropid, const PROPID rgpropid[], const LPOLESTR rglpwstrName[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeletePropertyNames(ULONG cpropid, const PROPID rgpropid[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE Revert(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Enum(IEnumSTATPROPSTG ** ppenum) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTimes(const FILETIME * pctime, const FILETIME * patime, const FILETIME * pmtime) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetClass(REFCLSID clsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stat(STATPROPSETSTG * pstatpsstg) = 0;
};

#else /* C style interface */

typedef struct IPropertyStorageVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IPropertyStorage * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IPropertyStorage * This);
    ULONG(STDMETHODCALLTYPE * Release) (IPropertyStorage * This);
    HRESULT(STDMETHODCALLTYPE * ReadMultiple) (IPropertyStorage * This, ULONG cpspec, const PROPSPEC rgpspec[], PROPVARIANT rgpropvar[]);
    HRESULT(STDMETHODCALLTYPE * WriteMultiple) (IPropertyStorage * This, ULONG cpspec, const PROPSPEC rgpspec[], const PROPVARIANT rgpropvar[], PROPID propidNameFirst);
    HRESULT(STDMETHODCALLTYPE * DeleteMultiple) (IPropertyStorage * This, ULONG cpspec, const PROPSPEC rgpspec[]);
    HRESULT(STDMETHODCALLTYPE * ReadPropertyNames) (IPropertyStorage * This, ULONG cpropid, const PROPID rgpropid[], LPOLESTR rglpwstrName[]);
    HRESULT(STDMETHODCALLTYPE * WritePropertyNames) (IPropertyStorage * This, ULONG cpropid, const PROPID rgpropid[], const LPOLESTR rglpwstrName[]);
    HRESULT(STDMETHODCALLTYPE * DeletePropertyNames) (IPropertyStorage * This, ULONG cpropid, const PROPID rgpropid[]);
    HRESULT(STDMETHODCALLTYPE * Commit) (IPropertyStorage * This, DWORD grfCommitFlags);
    HRESULT(STDMETHODCALLTYPE * Revert) (IPropertyStorage * This);
    HRESULT(STDMETHODCALLTYPE * Enum) (IPropertyStorage * This, IEnumSTATPROPSTG ** ppenum);
    HRESULT(STDMETHODCALLTYPE * SetTimes) (IPropertyStorage * This, const FILETIME * pctime, const FILETIME * patime, const FILETIME * pmtime);
    HRESULT(STDMETHODCALLTYPE * SetClass) (IPropertyStorage * This, REFCLSID clsid);
    HRESULT(STDMETHODCALLTYPE * Stat) (IPropertyStorage * This, STATPROPSETSTG * pstatpsstg);
    END_INTERFACE
} IPropertyStorageVtbl;

interface IPropertyStorage {
    CONST_VTBL struct IPropertyStorageVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPropertyStorage_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPropertyStorage_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPropertyStorage_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPropertyStorage_ReadMultiple(This,cpspec,rgpspec,rgpropvar)   ((This)->lpVtbl->ReadMultiple(This,cpspec,rgpspec,rgpropvar)) 
#define IPropertyStorage_WriteMultiple(This,cpspec,rgpspec,rgpropvar,propidNameFirst)  ((This)->lpVtbl->WriteMultiple(This,cpspec,rgpspec,rgpropvar,propidNameFirst)) 
#define IPropertyStorage_DeleteMultiple(This,cpspec,rgpspec)  ((This)->lpVtbl->DeleteMultiple(This,cpspec,rgpspec)) 
#define IPropertyStorage_ReadPropertyNames(This,cpropid,rgpropid,rglpwstrName)  ((This)->lpVtbl->ReadPropertyNames(This,cpropid,rgpropid,rglpwstrName)) 
#define IPropertyStorage_WritePropertyNames(This,cpropid,rgpropid,rglpwstrName)  ((This)->lpVtbl->WritePropertyNames(This,cpropid,rgpropid,rglpwstrName)) 
#define IPropertyStorage_DeletePropertyNames(This,cpropid,rgpropid)  ((This)->lpVtbl->DeletePropertyNames(This,cpropid,rgpropid)) 
#define IPropertyStorage_Commit(This,grfCommitFlags)  ((This)->lpVtbl->Commit(This,grfCommitFlags)) 
#define IPropertyStorage_Revert(This)  ((This)->lpVtbl->Revert(This)) 
#define IPropertyStorage_Enum(This,ppenum)  ((This)->lpVtbl->Enum(This,ppenum)) 
#define IPropertyStorage_SetTimes(This,pctime,patime,pmtime)  ((This)->lpVtbl->SetTimes(This,pctime,patime,pmtime)) 
#define IPropertyStorage_SetClass(This,clsid)  ((This)->lpVtbl->SetClass(This,clsid)) 
#define IPropertyStorage_Stat(This,pstatpsstg)  ((This)->lpVtbl->Stat(This,pstatpsstg)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPropertyStorage_INTERFACE_DEFINED__ */


#ifndef __IPropertySetStorage_INTERFACE_DEFINED__
#define __IPropertySetStorage_INTERFACE_DEFINED__

typedef IPropertySetStorage *LPPROPERTYSETSTORAGE;

EXTERN_C const IID IID_IPropertySetStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("0000013A-0000-0000-C000-000000000046")
IPropertySetStorage:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Create(REFFMTID rfmtid, const CLSID * pclsid, DWORD grfFlags, DWORD grfMode, IPropertyStorage ** ppprstg) = 0;
    virtual HRESULT STDMETHODCALLTYPE Open(REFFMTID rfmtid, DWORD grfMode, IPropertyStorage ** ppprstg) = 0;
    virtual HRESULT STDMETHODCALLTYPE Delete(REFFMTID rfmtid) = 0;
    virtual HRESULT STDMETHODCALLTYPE Enum(IEnumSTATPROPSETSTG ** ppenum) = 0;
};

#else /* C style interface */

typedef struct IPropertySetStorageVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IPropertySetStorage * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IPropertySetStorage * This);
    ULONG(STDMETHODCALLTYPE * Release) (IPropertySetStorage * This);
    HRESULT(STDMETHODCALLTYPE * Create) (IPropertySetStorage * This, REFFMTID rfmtid, const CLSID * pclsid, DWORD grfFlags, DWORD grfMode, IPropertyStorage ** ppprstg);
    HRESULT(STDMETHODCALLTYPE * Open) (IPropertySetStorage * This, REFFMTID rfmtid, DWORD grfMode, IPropertyStorage ** ppprstg);
    HRESULT(STDMETHODCALLTYPE * Delete) (IPropertySetStorage * This, REFFMTID rfmtid);
    HRESULT(STDMETHODCALLTYPE * Enum) (IPropertySetStorage * This, IEnumSTATPROPSETSTG ** ppenum);
    END_INTERFACE
} IPropertySetStorageVtbl;

interface IPropertySetStorage {
    CONST_VTBL struct IPropertySetStorageVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPropertySetStorage_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPropertySetStorage_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPropertySetStorage_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPropertySetStorage_Create(This,rfmtid,pclsid,grfFlags,grfMode,ppprstg)  ((This)->lpVtbl->Create(This,rfmtid,pclsid,grfFlags,grfMode,ppprstg)) 
#define IPropertySetStorage_Open(This,rfmtid,grfMode,ppprstg)  ((This)->lpVtbl->Open(This,rfmtid,grfMode,ppprstg)) 
#define IPropertySetStorage_Delete(This,rfmtid)  ((This)->lpVtbl->Delete(This,rfmtid)) 
#define IPropertySetStorage_Enum(This,ppenum)  ((This)->lpVtbl->Enum(This,ppenum)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPropertySetStorage_INTERFACE_DEFINED__ */


#ifndef __IEnumSTATPROPSTG_INTERFACE_DEFINED__
#define __IEnumSTATPROPSTG_INTERFACE_DEFINED__

typedef IEnumSTATPROPSTG *LPENUMSTATPROPSTG;

EXTERN_C const IID IID_IEnumSTATPROPSTG;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("00000139-0000-0000-C000-000000000046")
IEnumSTATPROPSTG:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, STATPROPSTG * rgelt, ULONG * pceltFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumSTATPROPSTG ** ppenum) = 0;
};

#else /* C style interface */

typedef struct IEnumSTATPROPSTGVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IEnumSTATPROPSTG * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IEnumSTATPROPSTG * This);
    ULONG(STDMETHODCALLTYPE * Release) (IEnumSTATPROPSTG * This);
    HRESULT(STDMETHODCALLTYPE * Next) (IEnumSTATPROPSTG * This, ULONG celt, STATPROPSTG * rgelt, ULONG * pceltFetched);
    HRESULT(STDMETHODCALLTYPE * Skip) (IEnumSTATPROPSTG * This, ULONG celt);
    HRESULT(STDMETHODCALLTYPE * Reset) (IEnumSTATPROPSTG * This);
    HRESULT(STDMETHODCALLTYPE * Clone) (IEnumSTATPROPSTG * This, IEnumSTATPROPSTG ** ppenum);
    END_INTERFACE
} IEnumSTATPROPSTGVtbl;

interface IEnumSTATPROPSTG {
    CONST_VTBL struct IEnumSTATPROPSTGVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumSTATPROPSTG_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IEnumSTATPROPSTG_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IEnumSTATPROPSTG_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IEnumSTATPROPSTG_Next(This,celt,rgelt,pceltFetched)  ((This)->lpVtbl->Next(This,celt,rgelt,pceltFetched)) 
#define IEnumSTATPROPSTG_Skip(This,celt)  ((This)->lpVtbl->Skip(This,celt)) 
#define IEnumSTATPROPSTG_Reset(This)  ((This)->lpVtbl->Reset(This)) 
#define IEnumSTATPROPSTG_Clone(This,ppenum)  ((This)->lpVtbl->Clone(This,ppenum)) 
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IEnumSTATPROPSTG_RemoteNext_Proxy(IEnumSTATPROPSTG *This, ULONG celt, STATPROPSTG *rgelt, ULONG *pceltFetched);
void __RPC_STUB IEnumSTATPROPSTG_RemoteNext_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);

#endif /* __IEnumSTATPROPSTG_INTERFACE_DEFINED__ */


#ifndef __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__
#define __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__

typedef IEnumSTATPROPSETSTG *LPENUMSTATPROPSETSTG;

EXTERN_C const IID IID_IEnumSTATPROPSETSTG;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("0000013B-0000-0000-C000-000000000046")
IEnumSTATPROPSETSTG:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, STATPROPSETSTG * rgelt, ULONG * pceltFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumSTATPROPSETSTG ** ppenum) = 0;
};

#else /* C style interface */

typedef struct IEnumSTATPROPSETSTGVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IEnumSTATPROPSETSTG * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IEnumSTATPROPSETSTG * This);
    ULONG(STDMETHODCALLTYPE * Release) (IEnumSTATPROPSETSTG * This);
    HRESULT(STDMETHODCALLTYPE * Next) (IEnumSTATPROPSETSTG * This, ULONG celt, STATPROPSETSTG * rgelt, ULONG * pceltFetched);
    HRESULT(STDMETHODCALLTYPE * Skip) (IEnumSTATPROPSETSTG * This, ULONG celt);
    HRESULT(STDMETHODCALLTYPE * Reset) (IEnumSTATPROPSETSTG * This);
    HRESULT(STDMETHODCALLTYPE * Clone) (IEnumSTATPROPSETSTG * This, IEnumSTATPROPSETSTG ** ppenum);
    END_INTERFACE
} IEnumSTATPROPSETSTGVtbl;

interface IEnumSTATPROPSETSTG {
    CONST_VTBL struct IEnumSTATPROPSETSTGVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumSTATPROPSETSTG_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IEnumSTATPROPSETSTG_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IEnumSTATPROPSETSTG_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IEnumSTATPROPSETSTG_Next(This,celt,rgelt,pceltFetched)  ((This)->lpVtbl->Next(This,celt,rgelt,pceltFetched)) 
#define IEnumSTATPROPSETSTG_Skip(This,celt)  ((This)->lpVtbl->Skip(This,celt)) 
#define IEnumSTATPROPSETSTG_Reset(This)  ((This)->lpVtbl->Reset(This)) 
#define IEnumSTATPROPSETSTG_Clone(This,ppenum)  ((This)->lpVtbl->Clone(This,ppenum)) 
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IEnumSTATPROPSETSTG_RemoteNext_Proxy(IEnumSTATPROPSETSTG *This, ULONG celt, STATPROPSETSTG *rgelt, ULONG *pceltFetched);
void __RPC_STUB IEnumSTATPROPSETSTG_RemoteNext_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);

#endif /* __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__ */


typedef IPropertyStorage *LPPROPERTYSTORAGE;

#define _PROPIDLBASE_
#endif /* _PROPIDLBASE_ */

extern RPC_IF_HANDLE __MIDL_itf_propidlbase_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_propidlbase_0000_0004_v0_0_s_ifspec;

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER LPSAFEARRAY_UserSize(unsigned long *, unsigned long, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserMarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
void __RPC_USER LPSAFEARRAY_UserFree(unsigned long *, LPSAFEARRAY *);

unsigned long __RPC_USER BSTR_UserSize64(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal64(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree64(unsigned long *, BSTR *);

unsigned long __RPC_USER LPSAFEARRAY_UserSize64(unsigned long *, unsigned long, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserMarshal64(unsigned long *, unsigned char *, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserUnmarshal64(unsigned long *, unsigned char *, LPSAFEARRAY *);
void __RPC_USER LPSAFEARRAY_UserFree64(unsigned long *, LPSAFEARRAY *);

HRESULT STDMETHODCALLTYPE IEnumSTATPROPSTG_Next_Proxy(IEnumSTATPROPSTG *This, ULONG celt, STATPROPSTG *rgelt, ULONG *pceltFetched);
HRESULT STDMETHODCALLTYPE IEnumSTATPROPSTG_Next_Stub(IEnumSTATPROPSTG *This, ULONG celt, STATPROPSTG *rgelt, ULONG *pceltFetched);
HRESULT STDMETHODCALLTYPE IEnumSTATPROPSETSTG_Next_Proxy(IEnumSTATPROPSETSTG *This, ULONG celt, STATPROPSETSTG *rgelt, ULONG *pceltFetched);
HRESULT STDMETHODCALLTYPE IEnumSTATPROPSETSTG_Next_Stub(IEnumSTATPROPSETSTG *This, ULONG celt, STATPROPSETSTG *rgelt, ULONG *pceltFetched);


#ifdef __cplusplus
}
#endif

#endif /* _PROPIDLBASE_H */
