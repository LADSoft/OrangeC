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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /* COM_NO_WINDOWS_H */

#ifndef _OAIDL_H
#define _OAIDL_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __ICreateTypeInfo_FWD_DEFINED__
#define __ICreateTypeInfo_FWD_DEFINED__
typedef interface ICreateTypeInfo ICreateTypeInfo;
#endif
#ifndef __ICreateTypeInfo2_FWD_DEFINED__
#define __ICreateTypeInfo2_FWD_DEFINED__
typedef interface ICreateTypeInfo2 ICreateTypeInfo2;
#endif
#ifndef __ICreateTypeLib_FWD_DEFINED__
#define __ICreateTypeLib_FWD_DEFINED__
typedef interface ICreateTypeLib ICreateTypeLib;
#endif
#ifndef __ICreateTypeLib2_FWD_DEFINED__
#define __ICreateTypeLib2_FWD_DEFINED__
typedef interface ICreateTypeLib2 ICreateTypeLib2;
#endif
#ifndef __IDispatch_FWD_DEFINED__
#define __IDispatch_FWD_DEFINED__
typedef interface IDispatch IDispatch;
#endif
#ifndef __IEnumVARIANT_FWD_DEFINED__
#define __IEnumVARIANT_FWD_DEFINED__
typedef interface IEnumVARIANT IEnumVARIANT;
#endif
#ifndef __ITypeComp_FWD_DEFINED__
#define __ITypeComp_FWD_DEFINED__
typedef interface ITypeComp ITypeComp;
#endif
#ifndef __ITypeInfo_FWD_DEFINED__
#define __ITypeInfo_FWD_DEFINED__
typedef interface ITypeInfo ITypeInfo;
#endif
#ifndef __ITypeInfo2_FWD_DEFINED__
#define __ITypeInfo2_FWD_DEFINED__
typedef interface ITypeInfo2 ITypeInfo2;
#endif
#ifndef __ITypeLib_FWD_DEFINED__
#define __ITypeLib_FWD_DEFINED__
typedef interface ITypeLib ITypeLib;
#endif
#ifndef __ITypeLib2_FWD_DEFINED__
#define __ITypeLib2_FWD_DEFINED__
typedef interface ITypeLib2 ITypeLib2;
#endif
#ifndef __ITypeChangeEvents_FWD_DEFINED__
#define __ITypeChangeEvents_FWD_DEFINED__
typedef interface ITypeChangeEvents ITypeChangeEvents;
#endif
#ifndef __IErrorInfo_FWD_DEFINED__
#define __IErrorInfo_FWD_DEFINED__
typedef interface IErrorInfo IErrorInfo;
#endif
#ifndef __ICreateErrorInfo_FWD_DEFINED__
#define __ICreateErrorInfo_FWD_DEFINED__
typedef interface ICreateErrorInfo ICreateErrorInfo;
#endif
#ifndef __ISupportErrorInfo_FWD_DEFINED__
#define __ISupportErrorInfo_FWD_DEFINED__
typedef interface ISupportErrorInfo ISupportErrorInfo;
#endif
#ifndef __ITypeFactory_FWD_DEFINED__
#define __ITypeFactory_FWD_DEFINED__
typedef interface ITypeFactory ITypeFactory;
#endif
#ifndef __ITypeMarshal_FWD_DEFINED__
#define __ITypeMarshal_FWD_DEFINED__
typedef interface ITypeMarshal ITypeMarshal;
#endif
#ifndef __IRecordInfo_FWD_DEFINED__
#define __IRecordInfo_FWD_DEFINED__
typedef interface IRecordInfo IRecordInfo;
#endif
#ifndef __IErrorLog_FWD_DEFINED__
#define __IErrorLog_FWD_DEFINED__
typedef interface IErrorLog IErrorLog;
#endif
#ifndef __IPropertyBag_FWD_DEFINED__
#define __IPropertyBag_FWD_DEFINED__
typedef interface IPropertyBag IPropertyBag;
#endif

#include "objidl.h"

#ifdef __cplusplus
extern "C" {
#endif


void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void*);

extern RPC_IF_HANDLE __MIDL_itf_oaidl_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_oaidl_0000_v0_0_s_ifspec;


#ifndef __IOleAutomationTypes_INTERFACE_DEFINED__
#define __IOleAutomationTypes_INTERFACE_DEFINED__

typedef CY CURRENCY;

typedef struct tagSAFEARRAYBOUND {
    ULONG cElements;
    LONG lLbound;
} SAFEARRAYBOUND;

typedef struct tagSAFEARRAYBOUND *LPSAFEARRAYBOUND;
typedef struct _wireVARIANT *wireVARIANT;
typedef struct _wireBRECORD *wireBRECORD;

typedef struct _wireSAFEARR_BSTR {
    ULONG Size;
    wireBSTR *aBstr;
} SAFEARR_BSTR;

typedef struct _wireSAFEARR_UNKNOWN {
    ULONG Size;
    IUnknown **apUnknown;
} SAFEARR_UNKNOWN;

typedef struct _wireSAFEARR_DISPATCH {
    ULONG Size;
    IDispatch **apDispatch;
} SAFEARR_DISPATCH;

typedef struct _wireSAFEARR_VARIANT {
    ULONG Size;
    wireVARIANT *aVariant;
} SAFEARR_VARIANT;

typedef struct _wireSAFEARR_BRECORD {
    ULONG Size;
    wireBRECORD *aRecord;
} SAFEARR_BRECORD;

typedef struct _wireSAFEARR_HAVEIID {
    ULONG Size;
    IUnknown **apUnknown;
    IID iid;
} SAFEARR_HAVEIID;

typedef enum tagSF_TYPE {
    SF_ERROR = VT_ERROR,
    SF_I1 = VT_I1,
    SF_I2 = VT_I2,
    SF_I4 = VT_I4,
    SF_I8 = VT_I8,
    SF_BSTR = VT_BSTR,
    SF_UNKNOWN = VT_UNKNOWN,
    SF_DISPATCH = VT_DISPATCH,
    SF_VARIANT = VT_VARIANT,
    SF_RECORD = VT_RECORD,
    SF_HAVEIID = VT_UNKNOWN|VT_RESERVED
} SF_TYPE;

typedef struct _wireSAFEARRAY_UNION {
    ULONG sfType;
    union __MIDL_IOleAutomationTypes_0001 {
        SAFEARR_BSTR BstrStr;
        SAFEARR_UNKNOWN UnknownStr;
        SAFEARR_DISPATCH DispatchStr;
        SAFEARR_VARIANT VariantStr;
        SAFEARR_BRECORD RecordStr;
        SAFEARR_HAVEIID HaveIidStr;
        BYTE_SIZEDARR ByteStr;
        WORD_SIZEDARR WordStr;
        DWORD_SIZEDARR LongStr;
        HYPER_SIZEDARR HyperStr;
    } u;
} SAFEARRAYUNION;

typedef  struct _wireSAFEARRAY {
    USHORT cDims;
    USHORT fFeatures;
    ULONG cbElements;
    ULONG cLocks;
    SAFEARRAYUNION uArrayStructs;
    SAFEARRAYBOUND rgsabound[1];
} *wireSAFEARRAY;

typedef  wireSAFEARRAY *wirePSAFEARRAY;

typedef struct tagSAFEARRAY {
    USHORT cDims;
    USHORT fFeatures;
    ULONG cbElements;
    ULONG cLocks;
    PVOID pvData;
    SAFEARRAYBOUND rgsabound[1];
} SAFEARRAY;

typedef SAFEARRAY *LPSAFEARRAY;

#define FADF_AUTO  0x1
#define FADF_STATIC  0x2
#define FADF_EMBEDDED  0x4
#define FADF_FIXEDSIZE  0x10
#define FADF_RECORD  0x20
#define FADF_HAVEIID  0x40
#define FADF_HAVEVARTYPE  0x80
#define FADF_BSTR  0x100
#define FADF_UNKNOWN  0x200
#define FADF_DISPATCH  0x400
#define FADF_VARIANT  0x800
#define FADF_RESERVED  0xF008

#if (__STDC__ && !defined(_FORCENAMELESSUNION)) || defined(NONAMELESSUNION)
#define __VARIANT_NAME_1  n1
#define __VARIANT_NAME_2  n2
#define __VARIANT_NAME_3  n3
#define __VARIANT_NAME_4  brecVal
#else
#define __tagVARIANT
#define __VARIANT_NAME_1
#define __VARIANT_NAME_2
#define __VARIANT_NAME_3
#define __tagBRECORD
#define __VARIANT_NAME_4
#endif

typedef struct tagVARIANT VARIANT;

struct tagVARIANT {
    union {
        struct __tagVARIANT {
            VARTYPE vt;
            WORD wReserved1;
            WORD wReserved2;
            WORD wReserved3;
            union {
                ULONGLONG ullVal;
                LONGLONG llVal;
                LONG lVal;
                BYTE bVal;
                SHORT iVal;
                FLOAT fltVal;
                DOUBLE dblVal;
                VARIANT_BOOL boolVal;
                _VARIANT_BOOL _bool;
                SCODE scode;
                CY cyVal;
                DATE date;
                BSTR bstrVal;
                IUnknown *punkVal;
                IDispatch *pdispVal;
                SAFEARRAY *parray;
                BYTE *pbVal;
                SHORT *piVal;
                LONG *plVal;
                LONGLONG *pllVal;
                FLOAT *pfltVal;
                DOUBLE *pdblVal;
                VARIANT_BOOL *pboolVal;
                _VARIANT_BOOL *pbool;
                SCODE *pscode;
                CY *pcyVal;
                DATE *pdate;
                BSTR *pbstrVal;
                IUnknown **ppunkVal;
                IDispatch **ppdispVal;
                SAFEARRAY **pparray;
                VARIANT *pvarVal;
                PVOID byref;
                CHAR cVal;
                USHORT uiVal;
                ULONG ulVal;
                INT intVal;
                UINT uintVal;
                DECIMAL *pdecVal;
                CHAR *pcVal;
                USHORT *puiVal;
                ULONG *pulVal;
                ULONGLONG *pullVal;
                INT *pintVal;
                UINT *puintVal;
                struct __tagBRECORD {
                    PVOID pvRecord;
                    IRecordInfo *pRecInfo;
                } __VARIANT_NAME_4;
            } __VARIANT_NAME_3;
        } __VARIANT_NAME_2;
        DECIMAL decVal;
    } __VARIANT_NAME_1;
};

typedef VARIANT *LPVARIANT;
typedef VARIANT VARIANTARG;
typedef VARIANT *LPVARIANTARG;

struct _wireBRECORD {
    ULONG fFlags;
    ULONG clSize;
    IRecordInfo *pRecInfo;
    byte *pRecord;
};

struct _wireVARIANT {
    DWORD clSize;
    DWORD rpcReserved;
    USHORT vt;
    USHORT wReserved1;
    USHORT wReserved2;
    USHORT wReserved3;
    union {
        LONG lVal;
        BYTE bVal;
        SHORT iVal;
        FLOAT fltVal;
        DOUBLE dblVal;
        VARIANT_BOOL boolVal;
        SCODE scode;
        CY cyVal;
        DATE date;
        wireBSTR bstrVal;
        IUnknown *punkVal;
        IDispatch *pdispVal;
        wirePSAFEARRAY parray;
        wireBRECORD brecVal;
        BYTE *pbVal;
        SHORT *piVal;
        LONG *plVal;
        FLOAT *pfltVal;
        DOUBLE *pdblVal;
        VARIANT_BOOL *pboolVal;
        SCODE *pscode;
        CY *pcyVal;
        DATE *pdate;
        wireBSTR *pbstrVal;
        IUnknown **ppunkVal;
        IDispatch **ppdispVal;
        wirePSAFEARRAY *pparray;
        wireVARIANT *pvarVal;
        CHAR cVal;
        USHORT uiVal;
        ULONG ulVal;
        INT intVal;
        UINT uintVal;
        DECIMAL decVal;
        DECIMAL *pdecVal;
        CHAR *pcVal;
        USHORT *puiVal;
        ULONG *pulVal;
        INT *pintVal;
        UINT *puintVal;
    };
};

typedef LONG DISPID;
typedef DISPID MEMBERID;
typedef DWORD HREFTYPE;

typedef enum tagTYPEKIND {
    TKIND_ENUM = 0,
    TKIND_RECORD = TKIND_ENUM+1,
    TKIND_MODULE = TKIND_RECORD+1,
    TKIND_INTERFACE = TKIND_MODULE+1,
    TKIND_DISPATCH = TKIND_INTERFACE+1,
    TKIND_COCLASS = TKIND_DISPATCH+1,
    TKIND_ALIAS = TKIND_COCLASS+1,
    TKIND_UNION = TKIND_ALIAS+1,
    TKIND_MAX = TKIND_UNION+1
} TYPEKIND;

typedef struct tagTYPEDESC {
    union {
        struct tagTYPEDESC *lptdesc;
        struct tagARRAYDESC *lpadesc;
        HREFTYPE hreftype;
    };
    VARTYPE vt;
} TYPEDESC;

typedef struct tagARRAYDESC {
    TYPEDESC tdescElem;
    USHORT cDims;
    SAFEARRAYBOUND rgbounds[1];
} ARRAYDESC;

typedef struct tagPARAMDESCEX {
    ULONG cBytes;
    VARIANTARG varDefaultValue;
} PARAMDESCEX;

typedef struct tagPARAMDESCEX *LPPARAMDESCEX;
typedef struct tagPARAMDESC {
    LPPARAMDESCEX pparamdescex;
    USHORT wParamFlags;
} PARAMDESC;

typedef struct tagPARAMDESC *LPPARAMDESC;

#define PARAMFLAG_NONE  0
#define PARAMFLAG_FIN  0x1
#define PARAMFLAG_FOUT  0x2
#define PARAMFLAG_FLCID  0x4
#define PARAMFLAG_FRETVAL  0x8
#define PARAMFLAG_FOPT  0x10
#define PARAMFLAG_FHASDEFAULT  0x20
#define PARAMFLAG_FHASCUSTDATA  0x40

typedef struct tagIDLDESC {
    ULONG_PTR dwReserved;
    USHORT wIDLFlags;
} IDLDESC;

typedef struct tagIDLDESC *LPIDLDESC;

#define IDLFLAG_NONE  (PARAMFLAG_NONE)
#define IDLFLAG_FIN  (PARAMFLAG_FIN)
#define IDLFLAG_FOUT  (PARAMFLAG_FOUT)
#define IDLFLAG_FLCID  (PARAMFLAG_FLCID)
#define IDLFLAG_FRETVAL (PARAMFLAG_FRETVAL)

typedef struct tagELEMDESC {
    TYPEDESC tdesc;
    union {
        IDLDESC idldesc;
        PARAMDESC paramdesc;
    };
} ELEMDESC,*LPELEMDESC;

typedef struct tagTYPEATTR {
    GUID guid;
    LCID lcid;
    DWORD dwReserved;
    MEMBERID memidConstructor;
    MEMBERID memidDestructor;
    LPOLESTR lpstrSchema;
    ULONG cbSizeInstance;
    TYPEKIND typekind;
    WORD cFuncs;
    WORD cVars;
    WORD cImplTypes;
    WORD cbSizeVft;
    WORD cbAlignment;
    WORD wTypeFlags;
    WORD wMajorVerNum;
    WORD wMinorVerNum;
    TYPEDESC tdescAlias;
    IDLDESC idldescType;
} TYPEATTR;

typedef struct tagTYPEATTR *LPTYPEATTR;

typedef struct tagDISPPARAMS {
    VARIANTARG *rgvarg;
    DISPID *rgdispidNamedArgs;
    UINT cArgs;
    UINT cNamedArgs;
} DISPPARAMS;

typedef struct tagEXCEPINFO {
    WORD wCode;
    WORD wReserved;
    BSTR bstrSource;
    BSTR bstrDescription;
    BSTR bstrHelpFile;
    DWORD dwHelpContext;
    PVOID pvReserved;
    HRESULT(__stdcall *pfnDeferredFillIn)(struct tagEXCEPINFO*);
    SCODE scode;
} EXCEPINFO,*LPEXCEPINFO;

typedef enum tagCALLCONV {
    CC_FASTCALL = 0,
    CC_CDECL = 1,
    CC_MSCPASCAL = CC_CDECL+1,
    CC_PASCAL = CC_MSCPASCAL,
    CC_MACPASCAL = CC_PASCAL+1,
    CC_STDCALL = CC_MACPASCAL+1,
    CC_FPFASTCALL = CC_STDCALL+1,
    CC_SYSCALL = CC_FPFASTCALL+1,
    CC_MPWCDECL = CC_SYSCALL+1,
    CC_MPWPASCAL = CC_MPWCDECL+1,
    CC_MAX = CC_MPWPASCAL+1
} CALLCONV;

typedef enum tagFUNCKIND {
    FUNC_VIRTUAL = 0,
    FUNC_PUREVIRTUAL = FUNC_VIRTUAL+1,
    FUNC_NONVIRTUAL = FUNC_PUREVIRTUAL+1,
    FUNC_STATIC = FUNC_NONVIRTUAL+1,
    FUNC_DISPATCH = FUNC_STATIC+1
} FUNCKIND;

typedef enum tagINVOKEKIND {
    INVOKE_FUNC = 1,
    INVOKE_PROPERTYGET = 2,
    INVOKE_PROPERTYPUT = 4,
    INVOKE_PROPERTYPUTREF = 8
} INVOKEKIND;

typedef struct tagFUNCDESC {
    MEMBERID memid;
    SCODE *lprgscode;
    ELEMDESC *lprgelemdescParam;
    FUNCKIND funckind;
    INVOKEKIND invkind;
    CALLCONV callconv;
    SHORT cParams;
    SHORT cParamsOpt;
    SHORT oVft;
    SHORT cScodes;
    ELEMDESC elemdescFunc;
    WORD wFuncFlags;
} FUNCDESC;

typedef struct tagFUNCDESC *LPFUNCDESC;

typedef enum tagVARKIND {
    VAR_PERINSTANCE = 0,
    VAR_STATIC = VAR_PERINSTANCE+1,
    VAR_CONST = VAR_STATIC+1,
    VAR_DISPATCH = VAR_CONST+1
} VARKIND;

#define IMPLTYPEFLAG_FDEFAULT  0x1
#define IMPLTYPEFLAG_FSOURCE  0x2
#define IMPLTYPEFLAG_FRESTRICTED  0x4
#define IMPLTYPEFLAG_FDEFAULTVTABLE  0x8

typedef struct tagVARDESC {
    MEMBERID memid;
    LPOLESTR lpstrSchema;
    union {
        ULONG oInst;
        VARIANT *lpvarValue;
    };
    ELEMDESC elemdescVar;
    WORD wVarFlags;
    VARKIND varkind;
} VARDESC;

typedef struct tagVARDESC *LPVARDESC;

typedef enum tagTYPEFLAGS {
    TYPEFLAG_FAPPOBJECT = 0x1,
    TYPEFLAG_FCANCREATE = 0x2,
    TYPEFLAG_FLICENSED = 0x4,
    TYPEFLAG_FPREDECLID = 0x8,
    TYPEFLAG_FHIDDEN = 0x10,
    TYPEFLAG_FCONTROL = 0x20,
    TYPEFLAG_FDUAL = 0x40,
    TYPEFLAG_FNONEXTENSIBLE = 0x80,
    TYPEFLAG_FOLEAUTOMATION = 0x100,
    TYPEFLAG_FRESTRICTED = 0x200,
    TYPEFLAG_FAGGREGATABLE = 0x400,
    TYPEFLAG_FREPLACEABLE = 0x800,
    TYPEFLAG_FDISPATCHABLE = 0x1000,
    TYPEFLAG_FREVERSEBIND = 0x2000,
    TYPEFLAG_FPROXY = 0x4000
} TYPEFLAGS;

typedef enum tagFUNCFLAGS {
    FUNCFLAG_FRESTRICTED = 0x1,
    FUNCFLAG_FSOURCE = 0x2,
    FUNCFLAG_FBINDABLE = 0x4,
    FUNCFLAG_FREQUESTEDIT = 0x8,
    FUNCFLAG_FDISPLAYBIND = 0x10,
    FUNCFLAG_FDEFAULTBIND = 0x20,
    FUNCFLAG_FHIDDEN = 0x40,
    FUNCFLAG_FUSESGETLASTERROR = 0x80,
    FUNCFLAG_FDEFAULTCOLLELEM = 0x100,
    FUNCFLAG_FUIDEFAULT = 0x200,
    FUNCFLAG_FNONBROWSABLE = 0x400,
    FUNCFLAG_FREPLACEABLE = 0x800,
    FUNCFLAG_FIMMEDIATEBIND = 0x1000
} FUNCFLAGS;

typedef enum tagVARFLAGS {
    VARFLAG_FREADONLY = 0x1,
    VARFLAG_FSOURCE = 0x2,
    VARFLAG_FBINDABLE = 0x4,
    VARFLAG_FREQUESTEDIT = 0x8,
    VARFLAG_FDISPLAYBIND = 0x10,
    VARFLAG_FDEFAULTBIND = 0x20,
    VARFLAG_FHIDDEN = 0x40,
    VARFLAG_FRESTRICTED = 0x80,
    VARFLAG_FDEFAULTCOLLELEM = 0x100,
    VARFLAG_FUIDEFAULT = 0x200,
    VARFLAG_FNONBROWSABLE = 0x400,
    VARFLAG_FREPLACEABLE = 0x800,
    VARFLAG_FIMMEDIATEBIND = 0x1000
} VARFLAGS;

typedef struct tagCLEANLOCALSTORAGE {
    IUnknown *pInterface;
    PVOID pStorage;
    DWORD flags;
} CLEANLOCALSTORAGE;

typedef struct tagCUSTDATAITEM {
    GUID guid;
    VARIANTARG varValue;
} CUSTDATAITEM;

typedef struct tagCUSTDATAITEM *LPCUSTDATAITEM;

typedef struct tagCUSTDATA {
    DWORD cCustData;
    LPCUSTDATAITEM prgCustData;
} CUSTDATA;

typedef struct tagCUSTDATA *LPCUSTDATA;

extern RPC_IF_HANDLE IOleAutomationTypes_v1_0_c_ifspec;
extern RPC_IF_HANDLE IOleAutomationTypes_v1_0_s_ifspec;

#endif /* __IOleAutomationTypes_INTERFACE_DEFINED__ */


#ifndef __ICreateTypeInfo_INTERFACE_DEFINED__
#define __ICreateTypeInfo_INTERFACE_DEFINED__

typedef  ICreateTypeInfo *LPCREATETYPEINFO;

EXTERN_C const IID IID_ICreateTypeInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020405-0000-0000-C000-000000000046") ICreateTypeInfo:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE SetGuid(REFGUID) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTypeFlags(UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDocString(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelpContext(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVersion(WORD,WORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddRefTypeInfo(ITypeInfo*,HREFTYPE*) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddFuncDesc(UINT,FUNCDESC*) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddImplType(UINT,HREFTYPE) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetImplTypeFlags(UINT,INT) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAlignment(WORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSchema(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddVarDesc(UINT,VARDESC*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFuncAndParamNames(UINT,LPOLESTR*,UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVarName(UINT,LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTypeDescAlias(TYPEDESC*) = 0;
    virtual HRESULT STDMETHODCALLTYPE DefineFuncAsDllEntry(UINT,LPOLESTR,LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFuncDocString(UINT,LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVarDocString(UINT,LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFuncHelpContext(UINT,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVarHelpContext(UINT,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMops(UINT,BSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTypeIdldesc(IDLDESC*) = 0;
    virtual HRESULT STDMETHODCALLTYPE LayOut(void) = 0;
};
#else
typedef struct ICreateTypeInfoVtbl {
    BEGIN_INTERFACE HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICreateTypeInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICreateTypeInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(ICreateTypeInfo*);
    HRESULT(STDMETHODCALLTYPE *SetGuid)(ICreateTypeInfo*,REFGUID);
    HRESULT(STDMETHODCALLTYPE *SetTypeFlags)(ICreateTypeInfo*,UINT);
    HRESULT(STDMETHODCALLTYPE *SetDocString)(ICreateTypeInfo*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetHelpContext)(ICreateTypeInfo*,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetVersion)(ICreateTypeInfo*,WORD,WORD);
    HRESULT(STDMETHODCALLTYPE *AddRefTypeInfo)(ICreateTypeInfo*,ITypeInfo*,HREFTYPE*);
    HRESULT(STDMETHODCALLTYPE *AddFuncDesc)(ICreateTypeInfo*,UINT,FUNCDESC*);
    HRESULT(STDMETHODCALLTYPE *AddImplType)(ICreateTypeInfo*,UINT,HREFTYPE);
    HRESULT(STDMETHODCALLTYPE *SetImplTypeFlags)(ICreateTypeInfo*,UINT,INT);
    HRESULT(STDMETHODCALLTYPE *SetAlignment)(ICreateTypeInfo*,WORD);
    HRESULT(STDMETHODCALLTYPE *SetSchema)(ICreateTypeInfo*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *AddVarDesc)(ICreateTypeInfo*,UINT,VARDESC*);
    HRESULT(STDMETHODCALLTYPE *SetFuncAndParamNames)(ICreateTypeInfo*,UINT,LPOLESTR*,UINT);
    HRESULT(STDMETHODCALLTYPE *SetVarName)(ICreateTypeInfo*,UINT,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetTypeDescAlias)(ICreateTypeInfo*,TYPEDESC*);
    HRESULT(STDMETHODCALLTYPE *DefineFuncAsDllEntry)(ICreateTypeInfo*,UINT,LPOLESTR,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetFuncDocString)(ICreateTypeInfo*,UINT,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetVarDocString)(ICreateTypeInfo*,UINT,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetFuncHelpContext)(ICreateTypeInfo*,UINT,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetVarHelpContext)(ICreateTypeInfo*,UINT,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetMops)(ICreateTypeInfo*,UINT,BSTR);
    HRESULT(STDMETHODCALLTYPE *SetTypeIdldesc)(ICreateTypeInfo*,IDLDESC*);
    HRESULT(STDMETHODCALLTYPE *LayOut)(ICreateTypeInfo*);
    END_INTERFACE
} ICreateTypeInfoVtbl;

interface ICreateTypeInfo {
    CONST_VTBL struct ICreateTypeInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICreateTypeInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICreateTypeInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICreateTypeInfo_Release(This)  (This)->lpVtbl->Release(This)
#define ICreateTypeInfo_SetGuid(This,guid)  (This)->lpVtbl->SetGuid(This,guid)
#define ICreateTypeInfo_SetTypeFlags(This,uTypeFlags)  (This)->lpVtbl->SetTypeFlags(This,uTypeFlags)
#define ICreateTypeInfo_SetDocString(This,pStrDoc)  (This)->lpVtbl->SetDocString(This,pStrDoc)
#define ICreateTypeInfo_SetHelpContext(This,dwHelpContext)  (This)->lpVtbl->SetHelpContext(This,dwHelpContext)
#define ICreateTypeInfo_SetVersion(This,wMajorVerNum,wMinorVerNum)  (This)->lpVtbl->SetVersion(This,wMajorVerNum,wMinorVerNum)
#define ICreateTypeInfo_AddRefTypeInfo(This,pTInfo,phRefType)  (This)->lpVtbl->AddRefTypeInfo(This,pTInfo,phRefType)
#define ICreateTypeInfo_AddFuncDesc(This,index,pFuncDesc)  (This)->lpVtbl->AddFuncDesc(This,index,pFuncDesc)
#define ICreateTypeInfo_AddImplType(This,index,hRefType)  (This)->lpVtbl->AddImplType(This,index,hRefType)
#define ICreateTypeInfo_SetImplTypeFlags(This,index,implTypeFlags)  (This)->lpVtbl->SetImplTypeFlags(This,index,implTypeFlags)
#define ICreateTypeInfo_SetAlignment(This,cbAlignment)  (This)->lpVtbl->SetAlignment(This,cbAlignment)
#define ICreateTypeInfo_SetSchema(This,pStrSchema)  (This)->lpVtbl->SetSchema(This,pStrSchema)
#define ICreateTypeInfo_AddVarDesc(This,index,pVarDesc)  (This)->lpVtbl->AddVarDesc(This,index,pVarDesc)
#define ICreateTypeInfo_SetFuncAndParamNames(This,index,rgszNames,cNames)  (This)->lpVtbl->SetFuncAndParamNames(This,index,rgszNames,cNames)
#define ICreateTypeInfo_SetVarName(This,index,szName)  (This)->lpVtbl->SetVarName(This,index,szName)
#define ICreateTypeInfo_SetTypeDescAlias(This,pTDescAlias)  (This)->lpVtbl->SetTypeDescAlias(This,pTDescAlias)
#define ICreateTypeInfo_DefineFuncAsDllEntry(This,index,szDllName,szProcName)  (This)->lpVtbl->DefineFuncAsDllEntry(This,index,szDllName,szProcName)
#define ICreateTypeInfo_SetFuncDocString(This,index,szDocString)  (This)->lpVtbl->SetFuncDocString(This,index,szDocString)
#define ICreateTypeInfo_SetVarDocString(This,index,szDocString)  (This)->lpVtbl->SetVarDocString(This,index,szDocString)
#define ICreateTypeInfo_SetFuncHelpContext(This,index,dwHelpContext)  (This)->lpVtbl->SetFuncHelpContext(This,index,dwHelpContext)
#define ICreateTypeInfo_SetVarHelpContext(This,index,dwHelpContext)  (This)->lpVtbl->SetVarHelpContext(This,index,dwHelpContext)
#define ICreateTypeInfo_SetMops(This,index,bstrMops)  (This)->lpVtbl->SetMops(This,index,bstrMops)
#define ICreateTypeInfo_SetTypeIdldesc(This,pIdlDesc)  (This)->lpVtbl->SetTypeIdldesc(This,pIdlDesc)
#define ICreateTypeInfo_LayOut(This)  (This)->lpVtbl->LayOut(This)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetGuid_Proxy(ICreateTypeInfo*,REFGUID);
void __RPC_STUB ICreateTypeInfo_SetGuid_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetTypeFlags_Proxy(ICreateTypeInfo*,UINT);
void __RPC_STUB ICreateTypeInfo_SetTypeFlags_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetDocString_Proxy(ICreateTypeInfo*,LPOLESTR);
void __RPC_STUB ICreateTypeInfo_SetDocString_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetHelpContext_Proxy(ICreateTypeInfo*,DWORD);
void __RPC_STUB ICreateTypeInfo_SetHelpContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetVersion_Proxy(ICreateTypeInfo*,WORD,WORD);
void __RPC_STUB ICreateTypeInfo_SetVersion_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_AddRefTypeInfo_Proxy(ICreateTypeInfo*,ITypeInfo*,HREFTYPE*);
void __RPC_STUB ICreateTypeInfo_AddRefTypeInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_AddFuncDesc_Proxy(ICreateTypeInfo*,UINT,FUNCDESC*);
void __RPC_STUB ICreateTypeInfo_AddFuncDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_AddImplType_Proxy(ICreateTypeInfo*,UINT,HREFTYPE);
void __RPC_STUB ICreateTypeInfo_AddImplType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetImplTypeFlags_Proxy(ICreateTypeInfo*,UINT,INT);
void __RPC_STUB ICreateTypeInfo_SetImplTypeFlags_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetAlignment_Proxy(ICreateTypeInfo*,WORD);
void __RPC_STUB ICreateTypeInfo_SetAlignment_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetSchema_Proxy(ICreateTypeInfo*,LPOLESTR);
void __RPC_STUB ICreateTypeInfo_SetSchema_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_AddVarDesc_Proxy(ICreateTypeInfo*,UINT,VARDESC*);
void __RPC_STUB ICreateTypeInfo_AddVarDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetFuncAndParamNames_Proxy(ICreateTypeInfo*,UINT,LPOLESTR*,UINT);
void __RPC_STUB ICreateTypeInfo_SetFuncAndParamNames_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetVarName_Proxy(ICreateTypeInfo*,UINT,LPOLESTR);
void __RPC_STUB ICreateTypeInfo_SetVarName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetTypeDescAlias_Proxy(ICreateTypeInfo*,TYPEDESC*);
void __RPC_STUB ICreateTypeInfo_SetTypeDescAlias_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_DefineFuncAsDllEntry_Proxy(ICreateTypeInfo*,UINT,LPOLESTR,LPOLESTR);
void __RPC_STUB ICreateTypeInfo_DefineFuncAsDllEntry_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetFuncDocString_Proxy(ICreateTypeInfo*,UINT,LPOLESTR);
void __RPC_STUB ICreateTypeInfo_SetFuncDocString_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetVarDocString_Proxy(ICreateTypeInfo*,UINT,LPOLESTR);
void __RPC_STUB ICreateTypeInfo_SetVarDocString_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetFuncHelpContext_Proxy(ICreateTypeInfo*,UINT,DWORD);
void __RPC_STUB ICreateTypeInfo_SetFuncHelpContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetVarHelpContext_Proxy(ICreateTypeInfo*,UINT,DWORD);
void __RPC_STUB ICreateTypeInfo_SetVarHelpContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetMops_Proxy(ICreateTypeInfo*,UINT,BSTR);
void __RPC_STUB ICreateTypeInfo_SetMops_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_SetTypeIdldesc_Proxy(ICreateTypeInfo*,IDLDESC*);
void __RPC_STUB ICreateTypeInfo_SetTypeIdldesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo_LayOut_Proxy(ICreateTypeInfo*);
void __RPC_STUB ICreateTypeInfo_LayOut_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ICreateTypeInfo_INTERFACE_DEFINED__ */


#ifndef __ICreateTypeInfo2_INTERFACE_DEFINED__
#define __ICreateTypeInfo2_INTERFACE_DEFINED__

typedef ICreateTypeInfo2 *LPCREATETYPEINFO2;
EXTERN_C const IID IID_ICreateTypeInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0002040E-0000-0000-C000-000000000046") ICreateTypeInfo2:public ICreateTypeInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE DeleteFuncDesc(UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteFuncDescByMemId(MEMBERID,INVOKEKIND) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteVarDesc(UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteVarDescByMemId(MEMBERID) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteImplType(UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCustData(REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFuncCustData(UINT,REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetParamCustData(UINT,UINT,REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVarCustData(UINT,REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetImplTypeCustData(UINT,REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelpStringContext(ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFuncHelpStringContext(UINT,ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVarHelpStringContext(UINT,ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Invalidate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetName(LPOLESTR) = 0;
};
#else
typedef struct ICreateTypeInfo2Vtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICreateTypeInfo2*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICreateTypeInfo2*);
    ULONG(STDMETHODCALLTYPE *Release)(ICreateTypeInfo2*);
    HRESULT(STDMETHODCALLTYPE *SetGuid)(ICreateTypeInfo2*,REFGUID);
    HRESULT(STDMETHODCALLTYPE *SetTypeFlags)(ICreateTypeInfo2*,UINT);
    HRESULT(STDMETHODCALLTYPE *SetDocString)(ICreateTypeInfo2*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetHelpContext)(ICreateTypeInfo2*,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetVersion)(ICreateTypeInfo2*,WORD,WORD);
    HRESULT(STDMETHODCALLTYPE *AddRefTypeInfo)(ICreateTypeInfo2*,ITypeInfo*,HREFTYPE*);
    HRESULT(STDMETHODCALLTYPE *AddFuncDesc)(ICreateTypeInfo2*,UINT,FUNCDESC*);
    HRESULT(STDMETHODCALLTYPE *AddImplType)(ICreateTypeInfo2*,UINT,HREFTYPE);
    HRESULT(STDMETHODCALLTYPE *SetImplTypeFlags)(ICreateTypeInfo2*,UINT,INT);
    HRESULT(STDMETHODCALLTYPE *SetAlignment)(ICreateTypeInfo2*,WORD);
    HRESULT(STDMETHODCALLTYPE *SetSchema)(ICreateTypeInfo2*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *AddVarDesc)(ICreateTypeInfo2*,UINT,VARDESC*);
    HRESULT(STDMETHODCALLTYPE *SetFuncAndParamNames)(ICreateTypeInfo2*,UINT,LPOLESTR*,UINT);
    HRESULT(STDMETHODCALLTYPE *SetVarName)(ICreateTypeInfo2*,UINT,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetTypeDescAlias)(ICreateTypeInfo2*,TYPEDESC*);
    HRESULT(STDMETHODCALLTYPE *DefineFuncAsDllEntry)(ICreateTypeInfo2*,UINT,LPOLESTR,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetFuncDocString)(ICreateTypeInfo2*,UINT,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetVarDocString)(ICreateTypeInfo2*,UINT,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetFuncHelpContext)(ICreateTypeInfo2*,UINT,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetVarHelpContext)(ICreateTypeInfo2*,UINT,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetMops)(ICreateTypeInfo2*,UINT,BSTR);
    HRESULT(STDMETHODCALLTYPE *SetTypeIdldesc)(ICreateTypeInfo2*,IDLDESC*);
    HRESULT(STDMETHODCALLTYPE *LayOut)(ICreateTypeInfo2*);
    HRESULT(STDMETHODCALLTYPE *DeleteFuncDesc)(ICreateTypeInfo2*,UINT);
    HRESULT(STDMETHODCALLTYPE *DeleteFuncDescByMemId)(ICreateTypeInfo2*,MEMBERID,INVOKEKIND);
    HRESULT(STDMETHODCALLTYPE *DeleteVarDesc)(ICreateTypeInfo2*,UINT);
    HRESULT(STDMETHODCALLTYPE *DeleteVarDescByMemId)(ICreateTypeInfo2*,MEMBERID);
    HRESULT(STDMETHODCALLTYPE *DeleteImplType)(ICreateTypeInfo2*,UINT);
    HRESULT(STDMETHODCALLTYPE *SetCustData)(ICreateTypeInfo2*,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *SetFuncCustData)(ICreateTypeInfo2*,UINT,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *SetParamCustData)(ICreateTypeInfo2*,UINT,UINT,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *SetVarCustData)(ICreateTypeInfo2*,UINT,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *SetImplTypeCustData)(ICreateTypeInfo2*,UINT,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *SetHelpStringContext)(ICreateTypeInfo2*,ULONG);
    HRESULT(STDMETHODCALLTYPE *SetFuncHelpStringContext)(ICreateTypeInfo2*,UINT,ULONG);
    HRESULT(STDMETHODCALLTYPE *SetVarHelpStringContext)(ICreateTypeInfo2*,UINT,ULONG);
    HRESULT(STDMETHODCALLTYPE *Invalidate)(ICreateTypeInfo2*);
    HRESULT(STDMETHODCALLTYPE *SetName)(ICreateTypeInfo2*,LPOLESTR);
    END_INTERFACE
} ICreateTypeInfo2Vtbl;

interface ICreateTypeInfo2 {
    CONST_VTBL struct ICreateTypeInfo2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICreateTypeInfo2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICreateTypeInfo2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICreateTypeInfo2_Release(This)  (This)->lpVtbl->Release(This)
#define ICreateTypeInfo2_SetGuid(This,guid)  (This)->lpVtbl->SetGuid(This,guid)
#define ICreateTypeInfo2_SetTypeFlags(This,uTypeFlags)  (This)->lpVtbl->SetTypeFlags(This,uTypeFlags)
#define ICreateTypeInfo2_SetDocString(This,pStrDoc)  (This)->lpVtbl->SetDocString(This,pStrDoc)
#define ICreateTypeInfo2_SetHelpContext(This,dwHelpContext)  (This)->lpVtbl->SetHelpContext(This,dwHelpContext)
#define ICreateTypeInfo2_SetVersion(This,wMajorVerNum,wMinorVerNum)  (This)->lpVtbl->SetVersion(This,wMajorVerNum,wMinorVerNum)
#define ICreateTypeInfo2_AddRefTypeInfo(This,pTInfo,phRefType)  (This)->lpVtbl->AddRefTypeInfo(This,pTInfo,phRefType)
#define ICreateTypeInfo2_AddFuncDesc(This,index,pFuncDesc)  (This)->lpVtbl->AddFuncDesc(This,index,pFuncDesc)
#define ICreateTypeInfo2_AddImplType(This,index,hRefType)  (This)->lpVtbl->AddImplType(This,index,hRefType)
#define ICreateTypeInfo2_SetImplTypeFlags(This,index,implTypeFlags)  (This)->lpVtbl->SetImplTypeFlags(This,index,implTypeFlags)
#define ICreateTypeInfo2_SetAlignment(This,cbAlignment)  (This)->lpVtbl->SetAlignment(This,cbAlignment)
#define ICreateTypeInfo2_SetSchema(This,pStrSchema)  (This)->lpVtbl->SetSchema(This,pStrSchema)
#define ICreateTypeInfo2_AddVarDesc(This,index,pVarDesc)  (This)->lpVtbl->AddVarDesc(This,index,pVarDesc)
#define ICreateTypeInfo2_SetFuncAndParamNames(This,index,rgszNames,cNames)  (This)->lpVtbl->SetFuncAndParamNames(This,index,rgszNames,cNames)
#define ICreateTypeInfo2_SetVarName(This,index,szName)  (This)->lpVtbl->SetVarName(This,index,szName)
#define ICreateTypeInfo2_SetTypeDescAlias(This,pTDescAlias)  (This)->lpVtbl->SetTypeDescAlias(This,pTDescAlias)
#define ICreateTypeInfo2_DefineFuncAsDllEntry(This,index,szDllName,szProcName)  (This)->lpVtbl->DefineFuncAsDllEntry(This,index,szDllName,szProcName)
#define ICreateTypeInfo2_SetFuncDocString(This,index,szDocString)  (This)->lpVtbl->SetFuncDocString(This,index,szDocString)
#define ICreateTypeInfo2_SetVarDocString(This,index,szDocString)  (This)->lpVtbl->SetVarDocString(This,index,szDocString)
#define ICreateTypeInfo2_SetFuncHelpContext(This,index,dwHelpContext)  (This)->lpVtbl->SetFuncHelpContext(This,index,dwHelpContext)
#define ICreateTypeInfo2_SetVarHelpContext(This,index,dwHelpContext)  (This)->lpVtbl->SetVarHelpContext(This,index,dwHelpContext)
#define ICreateTypeInfo2_SetMops(This,index,bstrMops)  (This)->lpVtbl->SetMops(This,index,bstrMops)
#define ICreateTypeInfo2_SetTypeIdldesc(This,pIdlDesc)  (This)->lpVtbl->SetTypeIdldesc(This,pIdlDesc)
#define ICreateTypeInfo2_LayOut(This)  (This)->lpVtbl->LayOut(This)
#define ICreateTypeInfo2_DeleteFuncDesc(This,index)  (This)->lpVtbl->DeleteFuncDesc(This,index)
#define ICreateTypeInfo2_DeleteFuncDescByMemId(This,memid,invKind)  (This)->lpVtbl->DeleteFuncDescByMemId(This,memid,invKind)
#define ICreateTypeInfo2_DeleteVarDesc(This,index)  (This)->lpVtbl->DeleteVarDesc(This,index)
#define ICreateTypeInfo2_DeleteVarDescByMemId(This,memid)  (This)->lpVtbl->DeleteVarDescByMemId(This,memid)
#define ICreateTypeInfo2_DeleteImplType(This,index)  (This)->lpVtbl->DeleteImplType(This,index)
#define ICreateTypeInfo2_SetCustData(This,guid,pVarVal)  (This)->lpVtbl->SetCustData(This,guid,pVarVal)
#define ICreateTypeInfo2_SetFuncCustData(This,index,guid,pVarVal)  (This)->lpVtbl->SetFuncCustData(This,index,guid,pVarVal)
#define ICreateTypeInfo2_SetParamCustData(This,indexFunc,indexParam,guid,pVarVal)  (This)->lpVtbl->SetParamCustData(This,indexFunc,indexParam,guid,pVarVal)
#define ICreateTypeInfo2_SetVarCustData(This,index,guid,pVarVal)  (This)->lpVtbl->SetVarCustData(This,index,guid,pVarVal)
#define ICreateTypeInfo2_SetImplTypeCustData(This,index,guid,pVarVal)  (This)->lpVtbl->SetImplTypeCustData(This,index,guid,pVarVal)
#define ICreateTypeInfo2_SetHelpStringContext(This,dwHelpStringContext)  (This)->lpVtbl->SetHelpStringContext(This,dwHelpStringContext)
#define ICreateTypeInfo2_SetFuncHelpStringContext(This,index,dwHelpStringContext)  (This)->lpVtbl->SetFuncHelpStringContext(This,index,dwHelpStringContext)
#define ICreateTypeInfo2_SetVarHelpStringContext(This,index,dwHelpStringContext)  (This)->lpVtbl->SetVarHelpStringContext(This,index,dwHelpStringContext)
#define ICreateTypeInfo2_Invalidate(This)  (This)->lpVtbl->Invalidate(This)
#define ICreateTypeInfo2_SetName(This,szName)  (This)->lpVtbl->SetName(This,szName)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_DeleteFuncDesc_Proxy(ICreateTypeInfo2*,UINT);
void __RPC_STUB ICreateTypeInfo2_DeleteFuncDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_DeleteFuncDescByMemId_Proxy(ICreateTypeInfo2*,MEMBERID,INVOKEKIND);
void __RPC_STUB ICreateTypeInfo2_DeleteFuncDescByMemId_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_DeleteVarDesc_Proxy(ICreateTypeInfo2*,UINT);
void __RPC_STUB ICreateTypeInfo2_DeleteVarDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_DeleteVarDescByMemId_Proxy(ICreateTypeInfo2*,MEMBERID);
void __RPC_STUB ICreateTypeInfo2_DeleteVarDescByMemId_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_DeleteImplType_Proxy(ICreateTypeInfo2*,UINT);
void __RPC_STUB ICreateTypeInfo2_DeleteImplType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetCustData_Proxy(ICreateTypeInfo2*,REFGUID,VARIANT*);
void __RPC_STUB ICreateTypeInfo2_SetCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetFuncCustData_Proxy(ICreateTypeInfo2*,UINT,REFGUID,VARIANT*);
void __RPC_STUB ICreateTypeInfo2_SetFuncCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetParamCustData_Proxy(ICreateTypeInfo2*,UINT,UINT,REFGUID,VARIANT*);
void __RPC_STUB ICreateTypeInfo2_SetParamCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetVarCustData_Proxy(ICreateTypeInfo2*,UINT,REFGUID,VARIANT*);
void __RPC_STUB ICreateTypeInfo2_SetVarCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetImplTypeCustData_Proxy(ICreateTypeInfo2*,UINT,REFGUID,VARIANT*);
void __RPC_STUB ICreateTypeInfo2_SetImplTypeCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetHelpStringContext_Proxy(ICreateTypeInfo2*,ULONG);
void __RPC_STUB ICreateTypeInfo2_SetHelpStringContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetFuncHelpStringContext_Proxy(ICreateTypeInfo2*,UINT,ULONG);
void __RPC_STUB ICreateTypeInfo2_SetFuncHelpStringContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetVarHelpStringContext_Proxy(ICreateTypeInfo2*,UINT,ULONG);
void __RPC_STUB ICreateTypeInfo2_SetVarHelpStringContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_Invalidate_Proxy(ICreateTypeInfo2*);
void __RPC_STUB ICreateTypeInfo2_Invalidate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeInfo2_SetName_Proxy(ICreateTypeInfo2*,LPOLESTR);
void __RPC_STUB ICreateTypeInfo2_SetName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ICreateTypeInfo2_INTERFACE_DEFINED__ */


#ifndef __ICreateTypeLib_INTERFACE_DEFINED__
#define __ICreateTypeLib_INTERFACE_DEFINED__

typedef ICreateTypeLib *LPCREATETYPELIB;
EXTERN_C const IID IID_ICreateTypeLib;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020406-0000-0000-C000-000000000046") ICreateTypeLib:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateTypeInfo(LPOLESTR,TYPEKIND,ICreateTypeInfo**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetName(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVersion(WORD,WORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetGuid(REFGUID) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDocString(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelpFileName(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelpContext(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetLcid(LCID) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetLibFlags(UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE SaveAllChanges(void) = 0;
};
#else
typedef struct ICreateTypeLibVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICreateTypeLib*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICreateTypeLib*);
    ULONG(STDMETHODCALLTYPE *Release)(ICreateTypeLib*);
    HRESULT(STDMETHODCALLTYPE *CreateTypeInfo)(ICreateTypeLib*,LPOLESTR,TYPEKIND,ICreateTypeInfo**);
    HRESULT(STDMETHODCALLTYPE *SetName)(ICreateTypeLib*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetVersion)(ICreateTypeLib*,WORD,WORD);
    HRESULT(STDMETHODCALLTYPE *SetGuid)(ICreateTypeLib*,REFGUID);
    HRESULT(STDMETHODCALLTYPE *SetDocString)(ICreateTypeLib*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetHelpFileName)(ICreateTypeLib*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetHelpContext)(ICreateTypeLib*,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetLcid)(ICreateTypeLib*,LCID);
    HRESULT(STDMETHODCALLTYPE *SetLibFlags)(ICreateTypeLib*,UINT);
    HRESULT(STDMETHODCALLTYPE *SaveAllChanges)(ICreateTypeLib*);
    END_INTERFACE
} ICreateTypeLibVtbl;

interface ICreateTypeLib {
    CONST_VTBL struct ICreateTypeLibVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICreateTypeLib_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICreateTypeLib_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICreateTypeLib_Release(This)  (This)->lpVtbl->Release(This)
#define ICreateTypeLib_CreateTypeInfo(This,szName,tkind,ppCTInfo)  (This)->lpVtbl->CreateTypeInfo(This,szName,tkind,ppCTInfo)
#define ICreateTypeLib_SetName(This,szName)  (This)->lpVtbl->SetName(This,szName)
#define ICreateTypeLib_SetVersion(This,wMajorVerNum,wMinorVerNum)  (This)->lpVtbl->SetVersion(This,wMajorVerNum,wMinorVerNum)
#define ICreateTypeLib_SetGuid(This,guid)  (This)->lpVtbl->SetGuid(This,guid)
#define ICreateTypeLib_SetDocString(This,szDoc)  (This)->lpVtbl->SetDocString(This,szDoc)
#define ICreateTypeLib_SetHelpFileName(This,szHelpFileName)  (This)->lpVtbl->SetHelpFileName(This,szHelpFileName)
#define ICreateTypeLib_SetHelpContext(This,dwHelpContext)  (This)->lpVtbl->SetHelpContext(This,dwHelpContext)
#define ICreateTypeLib_SetLcid(This,lcid)  (This)->lpVtbl->SetLcid(This,lcid)
#define ICreateTypeLib_SetLibFlags(This,uLibFlags)  (This)->lpVtbl->SetLibFlags(This,uLibFlags)
#define ICreateTypeLib_SaveAllChanges(This)  (This)->lpVtbl->SaveAllChanges(This)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ICreateTypeLib_CreateTypeInfo_Proxy(ICreateTypeLib*,LPOLESTR,TYPEKIND,ICreateTypeInfo**);
void __RPC_STUB ICreateTypeLib_CreateTypeInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SetName_Proxy(ICreateTypeLib*,LPOLESTR);
void __RPC_STUB ICreateTypeLib_SetName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SetVersion_Proxy(ICreateTypeLib*,WORD,WORD);
void __RPC_STUB ICreateTypeLib_SetVersion_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SetGuid_Proxy(ICreateTypeLib*,REFGUID);
void __RPC_STUB ICreateTypeLib_SetGuid_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SetDocString_Proxy(ICreateTypeLib*,LPOLESTR);
void __RPC_STUB ICreateTypeLib_SetDocString_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SetHelpFileName_Proxy(ICreateTypeLib*,LPOLESTR);
void __RPC_STUB ICreateTypeLib_SetHelpFileName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SetHelpContext_Proxy(ICreateTypeLib*,DWORD);
void __RPC_STUB ICreateTypeLib_SetHelpContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SetLcid_Proxy(ICreateTypeLib*,LCID);
void __RPC_STUB ICreateTypeLib_SetLcid_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SetLibFlags_Proxy(ICreateTypeLib*,UINT);
void __RPC_STUB ICreateTypeLib_SetLibFlags_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib_SaveAllChanges_Proxy(ICreateTypeLib*);
void __RPC_STUB ICreateTypeLib_SaveAllChanges_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ICreateTypeLib_INTERFACE_DEFINED__ */


#ifndef __ICreateTypeLib2_INTERFACE_DEFINED__
#define __ICreateTypeLib2_INTERFACE_DEFINED__

typedef  ICreateTypeLib2 *LPCREATETYPELIB2;
EXTERN_C const IID IID_ICreateTypeLib2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0002040F-0000-0000-C000-000000000046") ICreateTypeLib2:public ICreateTypeLib
{
    public:
    virtual HRESULT STDMETHODCALLTYPE DeleteTypeInfo(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCustData(REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelpStringContext(ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelpStringDll(LPOLESTR) = 0;
};
#else
typedef struct ICreateTypeLib2Vtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICreateTypeLib2*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICreateTypeLib2*);
    ULONG(STDMETHODCALLTYPE *Release)(ICreateTypeLib2*);
    HRESULT(STDMETHODCALLTYPE *CreateTypeInfo)(ICreateTypeLib2*,LPOLESTR,TYPEKIND,ICreateTypeInfo**);
    HRESULT(STDMETHODCALLTYPE *SetName)(ICreateTypeLib2*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetVersion)(ICreateTypeLib2*,WORD,WORD);
    HRESULT(STDMETHODCALLTYPE *SetGuid)(ICreateTypeLib2*,REFGUID);
    HRESULT(STDMETHODCALLTYPE *SetDocString)(ICreateTypeLib2*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetHelpFileName)(ICreateTypeLib2*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetHelpContext)(ICreateTypeLib2*,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetLcid)(ICreateTypeLib2*,LCID);
    HRESULT(STDMETHODCALLTYPE *SetLibFlags)(ICreateTypeLib2*,UINT);
    HRESULT(STDMETHODCALLTYPE *SaveAllChanges)(ICreateTypeLib2*);
    HRESULT(STDMETHODCALLTYPE *DeleteTypeInfo)(ICreateTypeLib2*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetCustData)(ICreateTypeLib2*,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *SetHelpStringContext)(ICreateTypeLib2*,ULONG);
    HRESULT(STDMETHODCALLTYPE *SetHelpStringDll)(ICreateTypeLib2*,LPOLESTR);
    END_INTERFACE
} ICreateTypeLib2Vtbl;

interface ICreateTypeLib2 {
    CONST_VTBL struct ICreateTypeLib2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICreateTypeLib2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICreateTypeLib2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICreateTypeLib2_Release(This)  (This)->lpVtbl->Release(This)
#define ICreateTypeLib2_CreateTypeInfo(This,szName,tkind,ppCTInfo)  (This)->lpVtbl->CreateTypeInfo(This,szName,tkind,ppCTInfo)
#define ICreateTypeLib2_SetName(This,szName)  (This)->lpVtbl->SetName(This,szName)
#define ICreateTypeLib2_SetVersion(This,wMajorVerNum,wMinorVerNum)  (This)->lpVtbl->SetVersion(This,wMajorVerNum,wMinorVerNum)
#define ICreateTypeLib2_SetGuid(This,guid)  (This)->lpVtbl->SetGuid(This,guid)
#define ICreateTypeLib2_SetDocString(This,szDoc)  (This)->lpVtbl->SetDocString(This,szDoc)
#define ICreateTypeLib2_SetHelpFileName(This,szHelpFileName)  (This)->lpVtbl->SetHelpFileName(This,szHelpFileName)
#define ICreateTypeLib2_SetHelpContext(This,dwHelpContext)  (This)->lpVtbl->SetHelpContext(This,dwHelpContext)
#define ICreateTypeLib2_SetLcid(This,lcid)  (This)->lpVtbl->SetLcid(This,lcid)
#define ICreateTypeLib2_SetLibFlags(This,uLibFlags)  (This)->lpVtbl->SetLibFlags(This,uLibFlags)
#define ICreateTypeLib2_SaveAllChanges(This)  (This)->lpVtbl->SaveAllChanges(This)
#define ICreateTypeLib2_DeleteTypeInfo(This,szName)  (This)->lpVtbl->DeleteTypeInfo(This,szName)
#define ICreateTypeLib2_SetCustData(This,guid,pVarVal)  (This)->lpVtbl->SetCustData(This,guid,pVarVal)
#define ICreateTypeLib2_SetHelpStringContext(This,dwHelpStringContext)  (This)->lpVtbl->SetHelpStringContext(This,dwHelpStringContext)
#define ICreateTypeLib2_SetHelpStringDll(This,szFileName)  (This)->lpVtbl->SetHelpStringDll(This,szFileName)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ICreateTypeLib2_DeleteTypeInfo_Proxy(ICreateTypeLib2*,LPOLESTR);
void __RPC_STUB ICreateTypeLib2_DeleteTypeInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib2_SetCustData_Proxy(ICreateTypeLib2*,REFGUID,VARIANT*);
void __RPC_STUB ICreateTypeLib2_SetCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib2_SetHelpStringContext_Proxy(ICreateTypeLib2*,ULONG);
void __RPC_STUB ICreateTypeLib2_SetHelpStringContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateTypeLib2_SetHelpStringDll_Proxy(ICreateTypeLib2*,LPOLESTR);
void __RPC_STUB ICreateTypeLib2_SetHelpStringDll_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ICreateTypeLib2_INTERFACE_DEFINED__ */


#ifndef __IDispatch_INTERFACE_DEFINED__
#define __IDispatch_INTERFACE_DEFINED__

typedef IDispatch *LPDISPATCH;

#define DISPID_UNKNOWN  (-1)
#define DISPID_VALUE  (0)
#define DISPID_PROPERTYPUT  (-3)
#define DISPID_NEWENUM  (-4)
#define DISPID_EVALUATE  (-5)
#define DISPID_CONSTRUCTOR  (-6)
#define DISPID_DESTRUCTOR  (-7)
#define DISPID_COLLECT  (-8)

EXTERN_C const IID IID_IDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020400-0000-0000-C000-000000000046") IDispatch:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT,LCID,ITypeInfo**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID,LPOLESTR*,UINT,LCID,DISPID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*) = 0;
};
#else
typedef struct IDispatchVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDispatch*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IDispatch*);
    ULONG(STDMETHODCALLTYPE *Release)(IDispatch*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IDispatch*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IDispatch*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IDispatch*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(IDispatch*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    END_INTERFACE
} IDispatchVtbl;

interface IDispatch {
    CONST_VTBL struct IDispatchVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDispatch_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDispatch_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDispatch_Release(This)  (This)->lpVtbl->Release(This)
#define IDispatch_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IDispatch_GetTypeInfoCount_Proxy(IDispatch*,UINT*);
void __RPC_STUB IDispatch_GetTypeInfoCount_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatch_GetTypeInfo_Proxy(IDispatch*,UINT,LCID,ITypeInfo**);
void __RPC_STUB IDispatch_GetTypeInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatch_GetIDsOfNames_Proxy(IDispatch*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
void __RPC_STUB IDispatch_GetIDsOfNames_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE IDispatch_RemoteInvoke_Proxy(IDispatch*,DISPID,REFIID,LCID,DWORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*,UINT,UINT*,VARIANTARG*);
void __RPC_STUB IDispatch_RemoteInvoke_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __IDispatch_INTERFACE_DEFINED__ */


#ifndef __IEnumVARIANT_INTERFACE_DEFINED__
#define __IEnumVARIANT_INTERFACE_DEFINED__

typedef IEnumVARIANT *LPENUMVARIANT;
EXTERN_C const IID IID_IEnumVARIANT;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020404-0000-0000-C000-000000000046") IEnumVARIANT:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG,VARIANT*,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumVARIANT**) = 0;
};
#else
typedef struct IEnumVARIANTVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IEnumVARIANT*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEnumVARIANT*);
    ULONG(STDMETHODCALLTYPE *Release)(IEnumVARIANT*);
    HRESULT(STDMETHODCALLTYPE *Next)(IEnumVARIANT*,ULONG,VARIANT*,ULONG*);
    HRESULT(STDMETHODCALLTYPE *Skip)(IEnumVARIANT*,ULONG);
    HRESULT(STDMETHODCALLTYPE *Reset)(IEnumVARIANT*);
    HRESULT(STDMETHODCALLTYPE *Clone)(IEnumVARIANT*,IEnumVARIANT**);
    END_INTERFACE
} IEnumVARIANTVtbl;

interface IEnumVARIANT {
    CONST_VTBL struct IEnumVARIANTVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumVARIANT_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEnumVARIANT_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumVARIANT_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumVARIANT_Next(This,celt,rgVar,pCeltFetched)  (This)->lpVtbl->Next(This,celt,rgVar,pCeltFetched)
#define IEnumVARIANT_Skip(This,celt)  (This)->lpVtbl->Skip(This,celt)
#define IEnumVARIANT_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumVARIANT_Clone(This,ppEnum)  (This)->lpVtbl->Clone(This,ppEnum)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IEnumVARIANT_RemoteNext_Proxy(IEnumVARIANT*,ULONG,VARIANT*,ULONG*);
void __RPC_STUB IEnumVARIANT_RemoteNext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumVARIANT_Skip_Proxy(IEnumVARIANT*,ULONG);
void __RPC_STUB IEnumVARIANT_Skip_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumVARIANT_Reset_Proxy(IEnumVARIANT*);
void __RPC_STUB IEnumVARIANT_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumVARIANT_Clone_Proxy(IEnumVARIANT*,IEnumVARIANT**);
void __RPC_STUB IEnumVARIANT_Clone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __IEnumVARIANT_INTERFACE_DEFINED__ */


#ifndef __ITypeComp_INTERFACE_DEFINED__
#define __ITypeComp_INTERFACE_DEFINED__

typedef  ITypeComp *LPTYPECOMP;

typedef enum tagDESCKIND {
    DESCKIND_NONE = 0,
    DESCKIND_FUNCDESC = DESCKIND_NONE+1,
    DESCKIND_VARDESC = DESCKIND_FUNCDESC+1,
    DESCKIND_TYPECOMP = DESCKIND_VARDESC+1,
    DESCKIND_IMPLICITAPPOBJ = DESCKIND_TYPECOMP+1,
    DESCKIND_MAX = DESCKIND_IMPLICITAPPOBJ+1
} DESCKIND;

typedef union tagBINDPTR {
    FUNCDESC *lpfuncdesc;
    VARDESC *lpvardesc;
    ITypeComp *lptcomp;
} BINDPTR;

typedef union tagBINDPTR *LPBINDPTR;
EXTERN_C const IID IID_ITypeComp;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020403-0000-0000-C000-000000000046") ITypeComp:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Bind(LPOLESTR,ULONG,WORD,ITypeInfo**,DESCKIND*,BINDPTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE BindType(LPOLESTR,ULONG,ITypeInfo**,ITypeComp**) = 0;
};
#else
typedef struct ITypeCompVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ITypeComp*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITypeComp*);
    ULONG(STDMETHODCALLTYPE *Release)(ITypeComp*);
    HRESULT(STDMETHODCALLTYPE *Bind)(ITypeComp*,LPOLESTR,ULONG,WORD,ITypeInfo**,DESCKIND*,BINDPTR*);
    HRESULT(STDMETHODCALLTYPE *BindType)(ITypeComp*,LPOLESTR,ULONG,ITypeInfo**,ITypeComp**);
    END_INTERFACE
} ITypeCompVtbl;

interface ITypeComp {
    CONST_VTBL struct ITypeCompVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITypeComp_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ITypeComp_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ITypeComp_Release(This)  (This)->lpVtbl->Release(This)
#define ITypeComp_Bind(This,szName,lHashVal,wFlags,ppTInfo,pDescKind,pBindPtr)  (This)->lpVtbl->Bind(This,szName,lHashVal,wFlags,ppTInfo,pDescKind,pBindPtr)
#define ITypeComp_BindType(This,szName,lHashVal,ppTInfo,ppTComp)  (This)->lpVtbl->BindType(This,szName,lHashVal,ppTInfo,ppTComp)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ITypeComp_RemoteBind_Proxy(ITypeComp*,LPOLESTR,ULONG,WORD,ITypeInfo**,DESCKIND*,LPFUNCDESC*,LPVARDESC*,ITypeComp**,CLEANLOCALSTORAGE*);
void __RPC_STUB ITypeComp_RemoteBind_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeComp_RemoteBindType_Proxy(ITypeComp*,LPOLESTR,ULONG,ITypeInfo**);
void __RPC_STUB ITypeComp_RemoteBindType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ITypeComp_INTERFACE_DEFINED__ */


#ifndef __ITypeInfo_INTERFACE_DEFINED__
#define __ITypeInfo_INTERFACE_DEFINED__

typedef  ITypeInfo *LPTYPEINFO;
EXTERN_C const IID IID_ITypeInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020401-0000-0000-C000-000000000046") ITypeInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetTypeAttr(TYPEATTR**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTypeComp(ITypeComp**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFuncDesc(UINT,FUNCDESC**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVarDesc(UINT,VARDESC**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNames(MEMBERID,BSTR*,UINT,UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRefTypeOfImplType(UINT,HREFTYPE*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetImplTypeFlags(UINT,INT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(LPOLESTR*,UINT,MEMBERID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Invoke(PVOID,MEMBERID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDocumentation(MEMBERID,BSTR*,BSTR*,DWORD*,BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDllEntry(MEMBERID,INVOKEKIND,BSTR*,BSTR*,WORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRefTypeInfo(HREFTYPE,ITypeInfo**) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddressOfMember(MEMBERID,INVOKEKIND,PVOID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown*,REFIID,PVOID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMops(MEMBERID,BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContainingTypeLib(ITypeLib**,UINT*) = 0;
    virtual void STDMETHODCALLTYPE ReleaseTypeAttr(TYPEATTR*) = 0;
    virtual void STDMETHODCALLTYPE ReleaseFuncDesc(FUNCDESC*) = 0;
    virtual void STDMETHODCALLTYPE ReleaseVarDesc(VARDESC*) = 0;
};
#else
typedef struct ITypeInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ITypeInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITypeInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(ITypeInfo*);
    HRESULT(STDMETHODCALLTYPE *GetTypeAttr)(ITypeInfo*,TYPEATTR**);
    HRESULT(STDMETHODCALLTYPE *GetTypeComp)(ITypeInfo*,ITypeComp**);
    HRESULT(STDMETHODCALLTYPE *GetFuncDesc)(ITypeInfo*,UINT,FUNCDESC**);
    HRESULT(STDMETHODCALLTYPE *GetVarDesc)(ITypeInfo*,UINT,VARDESC**);
    HRESULT(STDMETHODCALLTYPE *GetNames)(ITypeInfo*,MEMBERID,BSTR*,UINT,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetRefTypeOfImplType)(ITypeInfo*,UINT,HREFTYPE*);
    HRESULT(STDMETHODCALLTYPE *GetImplTypeFlags)(ITypeInfo*,UINT,INT*);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ITypeInfo*,LPOLESTR*,UINT,MEMBERID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ITypeInfo*,PVOID,MEMBERID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetDocumentation)(ITypeInfo*,MEMBERID,BSTR*,BSTR*,DWORD*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetDllEntry)(ITypeInfo*,MEMBERID,INVOKEKIND,BSTR*,BSTR*,WORD*);
    HRESULT(STDMETHODCALLTYPE *GetRefTypeInfo)(ITypeInfo*,HREFTYPE,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *AddressOfMember)(ITypeInfo*,MEMBERID,INVOKEKIND,PVOID*);
    HRESULT(STDMETHODCALLTYPE *CreateInstance)(ITypeInfo*,IUnknown*,REFIID,PVOID*);
    HRESULT(STDMETHODCALLTYPE *GetMops)(ITypeInfo*,MEMBERID,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetContainingTypeLib)(ITypeInfo*,ITypeLib**,UINT*);
    void (STDMETHODCALLTYPE *ReleaseTypeAttr)(ITypeInfo*,TYPEATTR*);
    void (STDMETHODCALLTYPE *ReleaseFuncDesc)(ITypeInfo*,FUNCDESC*);
    void (STDMETHODCALLTYPE *ReleaseVarDesc)(ITypeInfo*,VARDESC*);
    END_INTERFACE
} ITypeInfoVtbl;

interface ITypeInfo {
    CONST_VTBL struct ITypeInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITypeInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ITypeInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ITypeInfo_Release(This)  (This)->lpVtbl->Release(This)
#define ITypeInfo_GetTypeAttr(This,ppTypeAttr)  (This)->lpVtbl->GetTypeAttr(This,ppTypeAttr)
#define ITypeInfo_GetTypeComp(This,ppTComp)  (This)->lpVtbl->GetTypeComp(This,ppTComp)
#define ITypeInfo_GetFuncDesc(This,index,ppFuncDesc)  (This)->lpVtbl->GetFuncDesc(This,index,ppFuncDesc)
#define ITypeInfo_GetVarDesc(This,index,ppVarDesc)  (This)->lpVtbl->GetVarDesc(This,index,ppVarDesc)
#define ITypeInfo_GetNames(This,memid,rgBstrNames,cMaxNames,pcNames)  (This)->lpVtbl->GetNames(This,memid,rgBstrNames,cMaxNames,pcNames)
#define ITypeInfo_GetRefTypeOfImplType(This,index,pRefType)  (This)->lpVtbl->GetRefTypeOfImplType(This,index,pRefType)
#define ITypeInfo_GetImplTypeFlags(This,index,pImplTypeFlags)  (This)->lpVtbl->GetImplTypeFlags(This,index,pImplTypeFlags)
#define ITypeInfo_GetIDsOfNames(This,rgszNames,cNames,pMemId)  (This)->lpVtbl->GetIDsOfNames(This,rgszNames,cNames,pMemId)
#define ITypeInfo_Invoke(This,pvInstance,memid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,pvInstance,memid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ITypeInfo_GetDocumentation(This,memid,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile)  (This)->lpVtbl->GetDocumentation(This,memid,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile)
#define ITypeInfo_GetDllEntry(This,memid,invKind,pBstrDllName,pBstrName,pwOrdinal)  (This)->lpVtbl->GetDllEntry(This,memid,invKind,pBstrDllName,pBstrName,pwOrdinal)
#define ITypeInfo_GetRefTypeInfo(This,hRefType,ppTInfo)  (This)->lpVtbl->GetRefTypeInfo(This,hRefType,ppTInfo)
#define ITypeInfo_AddressOfMember(This,memid,invKind,ppv)  (This)->lpVtbl->AddressOfMember(This,memid,invKind,ppv)
#define ITypeInfo_CreateInstance(This,pUnkOuter,riid,ppvObj)  (This)->lpVtbl->CreateInstance(This,pUnkOuter,riid,ppvObj)
#define ITypeInfo_GetMops(This,memid,pBstrMops)  (This)->lpVtbl->GetMops(This,memid,pBstrMops)
#define ITypeInfo_GetContainingTypeLib(This,ppTLib,pIndex)  (This)->lpVtbl->GetContainingTypeLib(This,ppTLib,pIndex)
#define ITypeInfo_ReleaseTypeAttr(This,pTypeAttr)  (This)->lpVtbl->ReleaseTypeAttr(This,pTypeAttr)
#define ITypeInfo_ReleaseFuncDesc(This,pFuncDesc)  (This)->lpVtbl->ReleaseFuncDesc(This,pFuncDesc)
#define ITypeInfo_ReleaseVarDesc(This,pVarDesc)  (This)->lpVtbl->ReleaseVarDesc(This,pVarDesc)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetTypeAttr_Proxy(ITypeInfo*,LPTYPEATTR*,CLEANLOCALSTORAGE*);
void __RPC_STUB ITypeInfo_RemoteGetTypeAttr_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetTypeComp_Proxy(ITypeInfo*,ITypeComp**);
void __RPC_STUB ITypeInfo_GetTypeComp_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetFuncDesc_Proxy(ITypeInfo*,UINT,LPFUNCDESC*,CLEANLOCALSTORAGE*);
void __RPC_STUB ITypeInfo_RemoteGetFuncDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetVarDesc_Proxy(ITypeInfo*,UINT,LPVARDESC*,CLEANLOCALSTORAGE*);
void __RPC_STUB ITypeInfo_RemoteGetVarDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetNames_Proxy(ITypeInfo*,MEMBERID,BSTR*,UINT,UINT*);
void __RPC_STUB ITypeInfo_RemoteGetNames_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetRefTypeOfImplType_Proxy(ITypeInfo*,UINT,HREFTYPE*);
void __RPC_STUB ITypeInfo_GetRefTypeOfImplType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetImplTypeFlags_Proxy(ITypeInfo*,UINT,INT*);
void __RPC_STUB ITypeInfo_GetImplTypeFlags_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_LocalGetIDsOfNames_Proxy(ITypeInfo*);
void __RPC_STUB ITypeInfo_LocalGetIDsOfNames_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_LocalInvoke_Proxy(ITypeInfo*);
void __RPC_STUB ITypeInfo_LocalInvoke_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetDocumentation_Proxy(ITypeInfo*,MEMBERID,DWORD,BSTR*,BSTR*,DWORD*,BSTR*);
void __RPC_STUB ITypeInfo_RemoteGetDocumentation_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetDllEntry_Proxy(ITypeInfo*,MEMBERID,INVOKEKIND,DWORD,BSTR*,BSTR*,WORD*);
void __RPC_STUB ITypeInfo_RemoteGetDllEntry_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetRefTypeInfo_Proxy(ITypeInfo*,HREFTYPE,ITypeInfo**);
void __RPC_STUB ITypeInfo_GetRefTypeInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_LocalAddressOfMember_Proxy(ITypeInfo*);
void __RPC_STUB ITypeInfo_LocalAddressOfMember_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteCreateInstance_Proxy(ITypeInfo*,REFIID,IUnknown**);
void __RPC_STUB ITypeInfo_RemoteCreateInstance_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetMops_Proxy(ITypeInfo*,MEMBERID,BSTR*);
void __RPC_STUB ITypeInfo_GetMops_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetContainingTypeLib_Proxy(ITypeInfo*,ITypeLib**,UINT*);
void __RPC_STUB ITypeInfo_RemoteGetContainingTypeLib_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_LocalReleaseTypeAttr_Proxy(ITypeInfo*);
void __RPC_STUB ITypeInfo_LocalReleaseTypeAttr_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_LocalReleaseFuncDesc_Proxy(ITypeInfo*);
void __RPC_STUB ITypeInfo_LocalReleaseFuncDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_LocalReleaseVarDesc_Proxy(ITypeInfo*);
void __RPC_STUB ITypeInfo_LocalReleaseVarDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ITypeInfo_INTERFACE_DEFINED__ */


#ifndef __ITypeInfo2_INTERFACE_DEFINED__
#define __ITypeInfo2_INTERFACE_DEFINED__

typedef  ITypeInfo2 *LPTYPEINFO2;
EXTERN_C const IID IID_ITypeInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020412-0000-0000-C000-000000000046") ITypeInfo2:public ITypeInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetTypeKind(TYPEKIND*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTypeFlags(ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFuncIndexOfMemId(MEMBERID,INVOKEKIND,UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVarIndexOfMemId(MEMBERID,UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCustData(REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFuncCustData(UINT,REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParamCustData(UINT,UINT,REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVarCustData(UINT,REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetImplTypeCustData(UINT,REFGUID,VARIANT*) = 0;
    virtual  HRESULT STDMETHODCALLTYPE GetDocumentation2(MEMBERID,LCID,BSTR*,DWORD*,BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllCustData(CUSTDATA*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllFuncCustData(UINT,CUSTDATA*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllParamCustData(UINT,UINT,CUSTDATA*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllVarCustData(UINT,CUSTDATA*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllImplTypeCustData(UINT,CUSTDATA*) = 0;
};
#else
typedef struct ITypeInfo2Vtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ITypeInfo2*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITypeInfo2*);
    ULONG(STDMETHODCALLTYPE *Release)(ITypeInfo2*);
    HRESULT(STDMETHODCALLTYPE *GetTypeAttr)(ITypeInfo2*,TYPEATTR**);
    HRESULT(STDMETHODCALLTYPE *GetTypeComp)(ITypeInfo2*,ITypeComp**);
    HRESULT(STDMETHODCALLTYPE *GetFuncDesc)(ITypeInfo2*,UINT,FUNCDESC**);
    HRESULT(STDMETHODCALLTYPE *GetVarDesc)(ITypeInfo2*,UINT,VARDESC**);
    HRESULT(STDMETHODCALLTYPE *GetNames)(ITypeInfo2*,MEMBERID,BSTR*,UINT,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetRefTypeOfImplType)(ITypeInfo2*,UINT,HREFTYPE*);
    HRESULT(STDMETHODCALLTYPE *GetImplTypeFlags)(ITypeInfo2*,UINT,INT*);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ITypeInfo2*,LPOLESTR*,UINT,MEMBERID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ITypeInfo2*,PVOID,MEMBERID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetDocumentation)(ITypeInfo2*,MEMBERID,BSTR*,BSTR*,DWORD*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetDllEntry)(ITypeInfo2*,MEMBERID,INVOKEKIND,BSTR*,BSTR*,WORD*);
    HRESULT(STDMETHODCALLTYPE *GetRefTypeInfo)(ITypeInfo2*,HREFTYPE,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *AddressOfMember)(ITypeInfo2*,MEMBERID,INVOKEKIND,PVOID*);
    HRESULT(STDMETHODCALLTYPE *CreateInstance)(ITypeInfo2*,IUnknown*,REFIID,PVOID*);
    HRESULT(STDMETHODCALLTYPE *GetMops)(ITypeInfo2*,MEMBERID,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetContainingTypeLib)(ITypeInfo2*,ITypeLib**,UINT*);
    void (STDMETHODCALLTYPE *ReleaseTypeAttr)(ITypeInfo2*,TYPEATTR*);
    void (STDMETHODCALLTYPE *ReleaseFuncDesc)(ITypeInfo2*,FUNCDESC*);
    void (STDMETHODCALLTYPE *ReleaseVarDesc)(ITypeInfo2*,VARDESC*);
    HRESULT(STDMETHODCALLTYPE *GetTypeKind)(ITypeInfo2*,TYPEKIND*);
    HRESULT(STDMETHODCALLTYPE *GetTypeFlags)(ITypeInfo2*,ULONG*);
    HRESULT(STDMETHODCALLTYPE *GetFuncIndexOfMemId)(ITypeInfo2*,MEMBERID,INVOKEKIND,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetVarIndexOfMemId)(ITypeInfo2*,MEMBERID,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetCustData)(ITypeInfo2*,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *GetFuncCustData)(ITypeInfo2*,UINT,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *GetParamCustData)(ITypeInfo2*,UINT,UINT,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *GetVarCustData)(ITypeInfo2*,UINT,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *GetImplTypeCustData)(ITypeInfo2*,UINT,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *GetDocumentation2)(ITypeInfo2*,MEMBERID,LCID,BSTR*,DWORD*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetAllCustData)(ITypeInfo2*,CUSTDATA*);
    HRESULT(STDMETHODCALLTYPE *GetAllFuncCustData)(ITypeInfo2*,UINT,CUSTDATA*);
    HRESULT(STDMETHODCALLTYPE *GetAllParamCustData)(ITypeInfo2*,UINT,UINT,CUSTDATA*);
    HRESULT(STDMETHODCALLTYPE *GetAllVarCustData)(ITypeInfo2*,UINT,CUSTDATA*);
    HRESULT(STDMETHODCALLTYPE *GetAllImplTypeCustData)(ITypeInfo2*,UINT,CUSTDATA*);
    END_INTERFACE
} ITypeInfo2Vtbl;

interface ITypeInfo2 {
    CONST_VTBL struct ITypeInfo2Vtbl *lpVtbl;
};
#ifdef COBJMACROS
#define ITypeInfo2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ITypeInfo2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ITypeInfo2_Release(This)  (This)->lpVtbl->Release(This)
#define ITypeInfo2_GetTypeAttr(This,ppTypeAttr)  (This)->lpVtbl->GetTypeAttr(This,ppTypeAttr)
#define ITypeInfo2_GetTypeComp(This,ppTComp)  (This)->lpVtbl->GetTypeComp(This,ppTComp)
#define ITypeInfo2_GetFuncDesc(This,index,ppFuncDesc)  (This)->lpVtbl->GetFuncDesc(This,index,ppFuncDesc)
#define ITypeInfo2_GetVarDesc(This,index,ppVarDesc)  (This)->lpVtbl->GetVarDesc(This,index,ppVarDesc)
#define ITypeInfo2_GetNames(This,memid,rgBstrNames,cMaxNames,pcNames)  (This)->lpVtbl->GetNames(This,memid,rgBstrNames,cMaxNames,pcNames)
#define ITypeInfo2_GetRefTypeOfImplType(This,index,pRefType)  (This)->lpVtbl->GetRefTypeOfImplType(This,index,pRefType)
#define ITypeInfo2_GetImplTypeFlags(This,index,pImplTypeFlags)  (This)->lpVtbl->GetImplTypeFlags(This,index,pImplTypeFlags)
#define ITypeInfo2_GetIDsOfNames(This,rgszNames,cNames,pMemId)  (This)->lpVtbl->GetIDsOfNames(This,rgszNames,cNames,pMemId)
#define ITypeInfo2_Invoke(This,pvInstance,memid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,pvInstance,memid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ITypeInfo2_GetDocumentation(This,memid,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile)  (This)->lpVtbl->GetDocumentation(This,memid,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile)
#define ITypeInfo2_GetDllEntry(This,memid,invKind,pBstrDllName,pBstrName,pwOrdinal)  (This)->lpVtbl->GetDllEntry(This,memid,invKind,pBstrDllName,pBstrName,pwOrdinal)
#define ITypeInfo2_GetRefTypeInfo(This,hRefType,ppTInfo)  (This)->lpVtbl->GetRefTypeInfo(This,hRefType,ppTInfo)
#define ITypeInfo2_AddressOfMember(This,memid,invKind,ppv)  (This)->lpVtbl->AddressOfMember(This,memid,invKind,ppv)
#define ITypeInfo2_CreateInstance(This,pUnkOuter,riid,ppvObj)  (This)->lpVtbl->CreateInstance(This,pUnkOuter,riid,ppvObj)
#define ITypeInfo2_GetMops(This,memid,pBstrMops)  (This)->lpVtbl->GetMops(This,memid,pBstrMops)
#define ITypeInfo2_GetContainingTypeLib(This,ppTLib,pIndex)  (This)->lpVtbl->GetContainingTypeLib(This,ppTLib,pIndex)
#define ITypeInfo2_ReleaseTypeAttr(This,pTypeAttr)  (This)->lpVtbl->ReleaseTypeAttr(This,pTypeAttr)
#define ITypeInfo2_ReleaseFuncDesc(This,pFuncDesc)  (This)->lpVtbl->ReleaseFuncDesc(This,pFuncDesc)
#define ITypeInfo2_ReleaseVarDesc(This,pVarDesc)  (This)->lpVtbl->ReleaseVarDesc(This,pVarDesc)
#define ITypeInfo2_GetTypeKind(This,pTypeKind)  (This)->lpVtbl->GetTypeKind(This,pTypeKind)
#define ITypeInfo2_GetTypeFlags(This,pTypeFlags)  (This)->lpVtbl->GetTypeFlags(This,pTypeFlags)
#define ITypeInfo2_GetFuncIndexOfMemId(This,memid,invKind,pFuncIndex)  (This)->lpVtbl->GetFuncIndexOfMemId(This,memid,invKind,pFuncIndex)
#define ITypeInfo2_GetVarIndexOfMemId(This,memid,pVarIndex)  (This)->lpVtbl->GetVarIndexOfMemId(This,memid,pVarIndex)
#define ITypeInfo2_GetCustData(This,guid,pVarVal)  (This)->lpVtbl->GetCustData(This,guid,pVarVal)
#define ITypeInfo2_GetFuncCustData(This,index,guid,pVarVal)  (This)->lpVtbl->GetFuncCustData(This,index,guid,pVarVal)
#define ITypeInfo2_GetParamCustData(This,indexFunc,indexParam,guid,pVarVal)  (This)->lpVtbl->GetParamCustData(This,indexFunc,indexParam,guid,pVarVal)
#define ITypeInfo2_GetVarCustData(This,index,guid,pVarVal)  (This)->lpVtbl->GetVarCustData(This,index,guid,pVarVal)
#define ITypeInfo2_GetImplTypeCustData(This,index,guid,pVarVal)  (This)->lpVtbl->GetImplTypeCustData(This,index,guid,pVarVal)
#define ITypeInfo2_GetDocumentation2(This,memid,lcid,pbstrHelpString,pdwHelpStringContext,pbstrHelpStringDll)  (This)->lpVtbl->GetDocumentation2(This,memid,lcid,pbstrHelpString,pdwHelpStringContext,pbstrHelpStringDll)
#define ITypeInfo2_GetAllCustData(This,pCustData)  (This)->lpVtbl->GetAllCustData(This,pCustData)
#define ITypeInfo2_GetAllFuncCustData(This,index,pCustData)  (This)->lpVtbl->GetAllFuncCustData(This,index,pCustData)
#define ITypeInfo2_GetAllParamCustData(This,indexFunc,indexParam,pCustData)  (This)->lpVtbl->GetAllParamCustData(This,indexFunc,indexParam,pCustData)
#define ITypeInfo2_GetAllVarCustData(This,index,pCustData)  (This)->lpVtbl->GetAllVarCustData(This,index,pCustData)
#define ITypeInfo2_GetAllImplTypeCustData(This,index,pCustData)  (This)->lpVtbl->GetAllImplTypeCustData(This,index,pCustData)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ITypeInfo2_GetTypeKind_Proxy(ITypeInfo2*,TYPEKIND*);
void __RPC_STUB ITypeInfo2_GetTypeKind_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetTypeFlags_Proxy(ITypeInfo2*,ULONG*);
void __RPC_STUB ITypeInfo2_GetTypeFlags_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetFuncIndexOfMemId_Proxy(ITypeInfo2*,MEMBERID,INVOKEKIND,UINT*);
void __RPC_STUB ITypeInfo2_GetFuncIndexOfMemId_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetVarIndexOfMemId_Proxy(ITypeInfo2*,MEMBERID,UINT*);
void __RPC_STUB ITypeInfo2_GetVarIndexOfMemId_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetCustData_Proxy(ITypeInfo2*,REFGUID,VARIANT*);
void __RPC_STUB ITypeInfo2_GetCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetFuncCustData_Proxy(ITypeInfo2*,UINT,REFGUID,VARIANT*);
void __RPC_STUB ITypeInfo2_GetFuncCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetParamCustData_Proxy(ITypeInfo2*,UINT,UINT,REFGUID,VARIANT*);
void __RPC_STUB ITypeInfo2_GetParamCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetVarCustData_Proxy(ITypeInfo2*,UINT,REFGUID,VARIANT*);
void __RPC_STUB ITypeInfo2_GetVarCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetImplTypeCustData_Proxy(ITypeInfo2*,UINT,REFGUID,VARIANT*);
void __RPC_STUB ITypeInfo2_GetImplTypeCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE ITypeInfo2_RemoteGetDocumentation2_Proxy(ITypeInfo2*,MEMBERID,LCID,DWORD,BSTR*,DWORD*,BSTR*);
void __RPC_STUB ITypeInfo2_RemoteGetDocumentation2_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetAllCustData_Proxy(ITypeInfo2*,CUSTDATA*);
void __RPC_STUB ITypeInfo2_GetAllCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetAllFuncCustData_Proxy(ITypeInfo2*,UINT,CUSTDATA*);
void __RPC_STUB ITypeInfo2_GetAllFuncCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetAllParamCustData_Proxy(ITypeInfo2*,UINT,UINT,CUSTDATA*);
void __RPC_STUB ITypeInfo2_GetAllParamCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetAllVarCustData_Proxy(ITypeInfo2*,UINT,CUSTDATA*);
void __RPC_STUB ITypeInfo2_GetAllVarCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetAllImplTypeCustData_Proxy(ITypeInfo2*,UINT,CUSTDATA*);
void __RPC_STUB ITypeInfo2_GetAllImplTypeCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ITypeInfo2_INTERFACE_DEFINED__ */


#ifndef __ITypeLib_INTERFACE_DEFINED__
#define __ITypeLib_INTERFACE_DEFINED__

typedef enum tagSYSKIND {
    SYS_WIN16 = 0,
    SYS_WIN32 = SYS_WIN16+1,
    SYS_MAC = SYS_WIN32+1,
    SYS_WIN64 = SYS_WIN32
} SYSKIND;

typedef enum tagLIBFLAGS {
    LIBFLAG_FRESTRICTED = 0x1,
    LIBFLAG_FCONTROL = 0x2,
    LIBFLAG_FHIDDEN = 0x4,
    LIBFLAG_FHASDISKIMAGE = 0x8
} LIBFLAGS;

typedef ITypeLib *LPTYPELIB;

typedef struct tagTLIBATTR {
    GUID guid;
    LCID lcid;
    SYSKIND syskind;
    WORD wMajorVerNum;
    WORD wMinorVerNum;
    WORD wLibFlags;
} TLIBATTR;

typedef struct tagTLIBATTR *LPTLIBATTR;
EXTERN_C const IID IID_ITypeLib;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020402-0000-0000-C000-000000000046") ITypeLib:public IUnknown
{
    public:
    virtual UINT STDMETHODCALLTYPE GetTypeInfoCount(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT,ITypeInfo**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoType(UINT,TYPEKIND*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoOfGuid(REFGUID,ITypeInfo**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLibAttr(TLIBATTR**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTypeComp(ITypeComp**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDocumentation(INT,BSTR*,BSTR*,DWORD*,BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsName(LPOLESTR,ULONG,BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindName(LPOLESTR,ULONG,ITypeInfo**,MEMBERID*,USHORT*) = 0;
    virtual void STDMETHODCALLTYPE ReleaseTLibAttr(TLIBATTR*) = 0;
};
#else
typedef struct ITypeLibVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ITypeLib*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITypeLib*);
    ULONG(STDMETHODCALLTYPE *Release)(ITypeLib*);
    UINT(STDMETHODCALLTYPE *GetTypeInfoCount)(ITypeLib*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ITypeLib*,UINT,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoType)(ITypeLib*,UINT,TYPEKIND*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoOfGuid)(ITypeLib*,REFGUID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetLibAttr)(ITypeLib*,TLIBATTR**);
    HRESULT(STDMETHODCALLTYPE *GetTypeComp)(ITypeLib*,ITypeComp**);
    HRESULT(STDMETHODCALLTYPE *GetDocumentation)(ITypeLib*,INT,BSTR*,BSTR*,DWORD*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *IsName)(ITypeLib*,LPOLESTR,ULONG,BOOL*);
    HRESULT(STDMETHODCALLTYPE *FindName)(ITypeLib*,LPOLESTR,ULONG,ITypeInfo**,MEMBERID*,USHORT*);
    void (STDMETHODCALLTYPE *ReleaseTLibAttr)(ITypeLib*,TLIBATTR*);
    END_INTERFACE
} ITypeLibVtbl;

interface ITypeLib {
    CONST_VTBL struct ITypeLibVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITypeLib_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ITypeLib_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ITypeLib_Release(This)  (This)->lpVtbl->Release(This)
#define ITypeLib_GetTypeInfoCount(This)  (This)->lpVtbl->GetTypeInfoCount(This)
#define ITypeLib_GetTypeInfo(This,index,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,index,ppTInfo)
#define ITypeLib_GetTypeInfoType(This,index,pTKind)  (This)->lpVtbl->GetTypeInfoType(This,index,pTKind)
#define ITypeLib_GetTypeInfoOfGuid(This,guid,ppTinfo)  (This)->lpVtbl->GetTypeInfoOfGuid(This,guid,ppTinfo)
#define ITypeLib_GetLibAttr(This,ppTLibAttr)  (This)->lpVtbl->GetLibAttr(This,ppTLibAttr)
#define ITypeLib_GetTypeComp(This,ppTComp)  (This)->lpVtbl->GetTypeComp(This,ppTComp)
#define ITypeLib_GetDocumentation(This,index,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile)  (This)->lpVtbl->GetDocumentation(This,index,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile)
#define ITypeLib_IsName(This,szNameBuf,lHashVal,pfName)  (This)->lpVtbl->IsName(This,szNameBuf,lHashVal,pfName)
#define ITypeLib_FindName(This,szNameBuf,lHashVal,ppTInfo,rgMemId,pcFound)  (This)->lpVtbl->FindName(This,szNameBuf,lHashVal,ppTInfo,rgMemId,pcFound)
#define ITypeLib_ReleaseTLibAttr(This,pTLibAttr)  (This)->lpVtbl->ReleaseTLibAttr(This,pTLibAttr)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetTypeInfoCount_Proxy(ITypeLib*,UINT*);
void __RPC_STUB ITypeLib_RemoteGetTypeInfoCount_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeLib_GetTypeInfo_Proxy(ITypeLib*,UINT,ITypeInfo**);
void __RPC_STUB ITypeLib_GetTypeInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeLib_GetTypeInfoType_Proxy(ITypeLib*,UINT,TYPEKIND*);
void __RPC_STUB ITypeLib_GetTypeInfoType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeLib_GetTypeInfoOfGuid_Proxy(ITypeLib*,REFGUID,ITypeInfo**);
void __RPC_STUB ITypeLib_GetTypeInfoOfGuid_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetLibAttr_Proxy(ITypeLib*,LPTLIBATTR*,CLEANLOCALSTORAGE*);
void __RPC_STUB ITypeLib_RemoteGetLibAttr_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeLib_GetTypeComp_Proxy(ITypeLib*,ITypeComp**);
void __RPC_STUB ITypeLib_GetTypeComp_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetDocumentation_Proxy(ITypeLib*,INT,DWORD,BSTR*,BSTR*,DWORD*,BSTR*);
void __RPC_STUB ITypeLib_RemoteGetDocumentation_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE ITypeLib_RemoteIsName_Proxy(ITypeLib*,LPOLESTR,ULONG,BOOL*,BSTR*);
void __RPC_STUB ITypeLib_RemoteIsName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE ITypeLib_RemoteFindName_Proxy(ITypeLib*,LPOLESTR,ULONG,ITypeInfo**,MEMBERID*,USHORT*,BSTR*);
void __RPC_STUB ITypeLib_RemoteFindName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE ITypeLib_LocalReleaseTLibAttr_Proxy(ITypeLib*);
void __RPC_STUB ITypeLib_LocalReleaseTLibAttr_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ITypeLib_INTERFACE_DEFINED__ */


#ifndef __ITypeLib2_INTERFACE_DEFINED__
#define __ITypeLib2_INTERFACE_DEFINED__

typedef ITypeLib2 *LPTYPELIB2;
EXTERN_C const IID IID_ITypeLib2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020411-0000-0000-C000-000000000046") ITypeLib2:public ITypeLib
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCustData(REFGUID,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLibStatistics(ULONG*,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDocumentation2(INT,LCID,BSTR*,DWORD*,BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllCustData(CUSTDATA*) = 0;
};
#else
typedef struct ITypeLib2Vtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ITypeLib2*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITypeLib2*);
    ULONG(STDMETHODCALLTYPE *Release)(ITypeLib2*);
    UINT(STDMETHODCALLTYPE *GetTypeInfoCount)(ITypeLib2*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ITypeLib2*,UINT,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoType)(ITypeLib2*,UINT,TYPEKIND*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoOfGuid)(ITypeLib2*,REFGUID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetLibAttr)(ITypeLib2*,TLIBATTR**);
    HRESULT(STDMETHODCALLTYPE *GetTypeComp)(ITypeLib2*,ITypeComp**);
    HRESULT(STDMETHODCALLTYPE *GetDocumentation)(ITypeLib2*,INT,BSTR*,BSTR*,DWORD*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *IsName)(ITypeLib2*,LPOLESTR,ULONG,BOOL*);
    HRESULT(STDMETHODCALLTYPE *FindName)(ITypeLib2*,LPOLESTR,ULONG,ITypeInfo**,MEMBERID*,USHORT*);
    void (STDMETHODCALLTYPE *ReleaseTLibAttr)(ITypeLib2*,TLIBATTR*);
    HRESULT(STDMETHODCALLTYPE *GetCustData)(ITypeLib2*,REFGUID,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *GetLibStatistics)(ITypeLib2*,ULONG*,ULONG*);
    HRESULT(STDMETHODCALLTYPE *GetDocumentation2)(ITypeLib2*,INT,LCID,BSTR*,DWORD*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetAllCustData)(ITypeLib2*,CUSTDATA*);
    END_INTERFACE
} ITypeLib2Vtbl;

interface ITypeLib2 {
    CONST_VTBL struct ITypeLib2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITypeLib2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ITypeLib2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ITypeLib2_Release(This)  (This)->lpVtbl->Release(This)
#define ITypeLib2_GetTypeInfoCount(This)  (This)->lpVtbl->GetTypeInfoCount(This)
#define ITypeLib2_GetTypeInfo(This,index,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,index,ppTInfo)
#define ITypeLib2_GetTypeInfoType(This,index,pTKind)  (This)->lpVtbl->GetTypeInfoType(This,index,pTKind)
#define ITypeLib2_GetTypeInfoOfGuid(This,guid,ppTinfo)  (This)->lpVtbl->GetTypeInfoOfGuid(This,guid,ppTinfo)
#define ITypeLib2_GetLibAttr(This,ppTLibAttr)  (This)->lpVtbl->GetLibAttr(This,ppTLibAttr)
#define ITypeLib2_GetTypeComp(This,ppTComp)  (This)->lpVtbl->GetTypeComp(This,ppTComp)
#define ITypeLib2_GetDocumentation(This,index,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile)  (This)->lpVtbl->GetDocumentation(This,index,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile)
#define ITypeLib2_IsName(This,szNameBuf,lHashVal,pfName)  (This)->lpVtbl->IsName(This,szNameBuf,lHashVal,pfName)
#define ITypeLib2_FindName(This,szNameBuf,lHashVal,ppTInfo,rgMemId,pcFound)  (This)->lpVtbl->FindName(This,szNameBuf,lHashVal,ppTInfo,rgMemId,pcFound)
#define ITypeLib2_ReleaseTLibAttr(This,pTLibAttr)  (This)->lpVtbl->ReleaseTLibAttr(This,pTLibAttr)
#define ITypeLib2_GetCustData(This,guid,pVarVal)  (This)->lpVtbl->GetCustData(This,guid,pVarVal)
#define ITypeLib2_GetLibStatistics(This,pcUniqueNames,pcchUniqueNames)  (This)->lpVtbl->GetLibStatistics(This,pcUniqueNames,pcchUniqueNames)
#define ITypeLib2_GetDocumentation2(This,index,lcid,pbstrHelpString,pdwHelpStringContext,pbstrHelpStringDll)  (This)->lpVtbl->GetDocumentation2(This,index,lcid,pbstrHelpString,pdwHelpStringContext,pbstrHelpStringDll)
#define ITypeLib2_GetAllCustData(This,pCustData)  (This)->lpVtbl->GetAllCustData(This,pCustData)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ITypeLib2_GetCustData_Proxy(ITypeLib2*,REFGUID,VARIANT*);
void __RPC_STUB ITypeLib2_GetCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE ITypeLib2_RemoteGetLibStatistics_Proxy(ITypeLib2*,ULONG*,ULONG*);
void __RPC_STUB ITypeLib2_RemoteGetLibStatistics_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
 HRESULT STDMETHODCALLTYPE ITypeLib2_RemoteGetDocumentation2_Proxy(ITypeLib2*,INT,LCID,DWORD,BSTR*,DWORD*,BSTR*);
void __RPC_STUB ITypeLib2_RemoteGetDocumentation2_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeLib2_GetAllCustData_Proxy(ITypeLib2*,CUSTDATA*);
void __RPC_STUB ITypeLib2_GetAllCustData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ITypeLib2_INTERFACE_DEFINED__ */


#ifndef __ITypeChangeEvents_INTERFACE_DEFINED__
#define __ITypeChangeEvents_INTERFACE_DEFINED__

typedef ITypeChangeEvents *LPTYPECHANGEEVENTS;
EXTERN_C const IID IID_ITypeChangeEvents;

typedef enum tagCHANGEKIND {
    CHANGEKIND_ADDMEMBER = 0,
    CHANGEKIND_DELETEMEMBER = CHANGEKIND_ADDMEMBER+1,
    CHANGEKIND_SETNAMES = CHANGEKIND_DELETEMEMBER+1,
    CHANGEKIND_SETDOCUMENTATION = CHANGEKIND_SETNAMES+1,
    CHANGEKIND_GENERAL = CHANGEKIND_SETDOCUMENTATION+1,
    CHANGEKIND_INVALIDATE = CHANGEKIND_GENERAL+1,
    CHANGEKIND_CHANGEFAILED = CHANGEKIND_INVALIDATE+1,
    CHANGEKIND_MAX = CHANGEKIND_CHANGEFAILED+1
} CHANGEKIND;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00020410-0000-0000-C000-000000000046") ITypeChangeEvents:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE RequestTypeChange(CHANGEKIND,ITypeInfo*,LPOLESTR,INT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE AfterTypeChange(CHANGEKIND,ITypeInfo*,LPOLESTR) = 0;
};
#else
typedef struct ITypeChangeEventsVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ITypeChangeEvents*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITypeChangeEvents*);
    ULONG(STDMETHODCALLTYPE *Release)(ITypeChangeEvents*);
    HRESULT(STDMETHODCALLTYPE *RequestTypeChange)(ITypeChangeEvents*,CHANGEKIND,ITypeInfo*,LPOLESTR,INT*);
    HRESULT(STDMETHODCALLTYPE *AfterTypeChange)(ITypeChangeEvents*,CHANGEKIND,ITypeInfo*,LPOLESTR);
    END_INTERFACE
} ITypeChangeEventsVtbl;

interface ITypeChangeEvents {
    CONST_VTBL struct ITypeChangeEventsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITypeChangeEvents_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ITypeChangeEvents_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ITypeChangeEvents_Release(This)  (This)->lpVtbl->Release(This)
#define ITypeChangeEvents_RequestTypeChange(This,changeKind,pTInfoBefore,pStrName,pfCancel)  (This)->lpVtbl->RequestTypeChange(This,changeKind,pTInfoBefore,pStrName,pfCancel)
#define ITypeChangeEvents_AfterTypeChange(This,changeKind,pTInfoAfter,pStrName)  (This)->lpVtbl->AfterTypeChange(This,changeKind,pTInfoAfter,pStrName)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ITypeChangeEvents_RequestTypeChange_Proxy(ITypeChangeEvents*,CHANGEKIND,ITypeInfo*,LPOLESTR,INT*);
void __RPC_STUB ITypeChangeEvents_RequestTypeChange_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeChangeEvents_AfterTypeChange_Proxy(ITypeChangeEvents*,CHANGEKIND,ITypeInfo*,LPOLESTR);
void __RPC_STUB ITypeChangeEvents_AfterTypeChange_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ITypeChangeEvents_INTERFACE_DEFINED__ */


#ifndef __IErrorInfo_INTERFACE_DEFINED__
#define __IErrorInfo_INTERFACE_DEFINED__

typedef  IErrorInfo *LPERRORINFO;
EXTERN_C const IID IID_IErrorInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1CF2B120-547D-101B-8E65-08002B2BD119") IErrorInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetGUID(GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDescription(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHelpFile(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHelpContext(DWORD*) = 0;
};
#else
typedef struct IErrorInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IErrorInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IErrorInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(IErrorInfo*);
    HRESULT(STDMETHODCALLTYPE *GetGUID)(IErrorInfo*,GUID*);
    HRESULT(STDMETHODCALLTYPE *GetSource)(IErrorInfo*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetDescription)(IErrorInfo*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetHelpFile)(IErrorInfo*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetHelpContext)(IErrorInfo*,DWORD*);
    END_INTERFACE
} IErrorInfoVtbl;

interface IErrorInfo {
    CONST_VTBL struct IErrorInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IErrorInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IErrorInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IErrorInfo_Release(This)  (This)->lpVtbl->Release(This)
#define IErrorInfo_GetGUID(This,pGUID)  (This)->lpVtbl->GetGUID(This,pGUID)
#define IErrorInfo_GetSource(This,pBstrSource)  (This)->lpVtbl->GetSource(This,pBstrSource)
#define IErrorInfo_GetDescription(This,pBstrDescription)  (This)->lpVtbl->GetDescription(This,pBstrDescription)
#define IErrorInfo_GetHelpFile(This,pBstrHelpFile)  (This)->lpVtbl->GetHelpFile(This,pBstrHelpFile)
#define IErrorInfo_GetHelpContext(This,pdwHelpContext)  (This)->lpVtbl->GetHelpContext(This,pdwHelpContext)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IErrorInfo_GetGUID_Proxy(IErrorInfo*,GUID*);
void __RPC_STUB IErrorInfo_GetGUID_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IErrorInfo_GetSource_Proxy(IErrorInfo*,BSTR*);
void __RPC_STUB IErrorInfo_GetSource_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IErrorInfo_GetDescription_Proxy(IErrorInfo*,BSTR*);
void __RPC_STUB IErrorInfo_GetDescription_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IErrorInfo_GetHelpFile_Proxy(IErrorInfo*,BSTR*);
void __RPC_STUB IErrorInfo_GetHelpFile_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IErrorInfo_GetHelpContext_Proxy(IErrorInfo*,DWORD*);
void __RPC_STUB IErrorInfo_GetHelpContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __IErrorInfo_INTERFACE_DEFINED__ */


#ifndef __ICreateErrorInfo_INTERFACE_DEFINED__
#define __ICreateErrorInfo_INTERFACE_DEFINED__

typedef ICreateErrorInfo *LPCREATEERRORINFO;
EXTERN_C const IID IID_ICreateErrorInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("22F03340-547D-101B-8E65-08002B2BD119") ICreateErrorInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetGUID(REFGUID) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSource(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDescription(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelpFile(LPOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelpContext(DWORD) = 0;
};
#else
typedef struct ICreateErrorInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICreateErrorInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICreateErrorInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(ICreateErrorInfo*);
    HRESULT(STDMETHODCALLTYPE *SetGUID)(ICreateErrorInfo*,REFGUID);
    HRESULT(STDMETHODCALLTYPE *SetSource)(ICreateErrorInfo*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetDescription)(ICreateErrorInfo*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetHelpFile)(ICreateErrorInfo*,LPOLESTR);
    HRESULT(STDMETHODCALLTYPE *SetHelpContext)(ICreateErrorInfo*,DWORD);
    END_INTERFACE
} ICreateErrorInfoVtbl;

interface ICreateErrorInfo {
    CONST_VTBL struct ICreateErrorInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICreateErrorInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICreateErrorInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICreateErrorInfo_Release(This)  (This)->lpVtbl->Release(This)
#define ICreateErrorInfo_SetGUID(This,rguid)  (This)->lpVtbl->SetGUID(This,rguid)
#define ICreateErrorInfo_SetSource(This,szSource)  (This)->lpVtbl->SetSource(This,szSource)
#define ICreateErrorInfo_SetDescription(This,szDescription)  (This)->lpVtbl->SetDescription(This,szDescription)
#define ICreateErrorInfo_SetHelpFile(This,szHelpFile)  (This)->lpVtbl->SetHelpFile(This,szHelpFile)
#define ICreateErrorInfo_SetHelpContext(This,dwHelpContext)  (This)->lpVtbl->SetHelpContext(This,dwHelpContext)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ICreateErrorInfo_SetGUID_Proxy(ICreateErrorInfo*,REFGUID);
void __RPC_STUB ICreateErrorInfo_SetGUID_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateErrorInfo_SetSource_Proxy(ICreateErrorInfo*,LPOLESTR);
void __RPC_STUB ICreateErrorInfo_SetSource_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateErrorInfo_SetDescription_Proxy(ICreateErrorInfo*,LPOLESTR);
void __RPC_STUB ICreateErrorInfo_SetDescription_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateErrorInfo_SetHelpFile_Proxy(ICreateErrorInfo*,LPOLESTR);
void __RPC_STUB ICreateErrorInfo_SetHelpFile_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ICreateErrorInfo_SetHelpContext_Proxy(ICreateErrorInfo*,DWORD);
void __RPC_STUB ICreateErrorInfo_SetHelpContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ICreateErrorInfo_INTERFACE_DEFINED__ */


#ifndef __ISupportErrorInfo_INTERFACE_DEFINED__
#define __ISupportErrorInfo_INTERFACE_DEFINED__

typedef ISupportErrorInfo *LPSUPPORTERRORINFO;
EXTERN_C const IID IID_ISupportErrorInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DF0B3D60-548F-101B-8E65-08002B2BD119") ISupportErrorInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InterfaceSupportsErrorInfo(REFIID) = 0;
};
#else
typedef struct ISupportErrorInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISupportErrorInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ISupportErrorInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(ISupportErrorInfo*);
    HRESULT(STDMETHODCALLTYPE *InterfaceSupportsErrorInfo)(ISupportErrorInfo*,REFIID);
    END_INTERFACE
} ISupportErrorInfoVtbl;

interface ISupportErrorInfo {
    CONST_VTBL struct ISupportErrorInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISupportErrorInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ISupportErrorInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ISupportErrorInfo_Release(This)  (This)->lpVtbl->Release(This)
#define ISupportErrorInfo_InterfaceSupportsErrorInfo(This,riid)  (This)->lpVtbl->InterfaceSupportsErrorInfo(This,riid)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ISupportErrorInfo_InterfaceSupportsErrorInfo_Proxy(ISupportErrorInfo*,REFIID);
void __RPC_STUB ISupportErrorInfo_InterfaceSupportsErrorInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ISupportErrorInfo_INTERFACE_DEFINED__ */


#ifndef __ITypeFactory_INTERFACE_DEFINED__
#define __ITypeFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITypeFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000002E-0000-0000-C000-000000000046") ITypeFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateFromTypeInfo(ITypeInfo*,REFIID,IUnknown**) = 0;
};
#else
typedef struct ITypeFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ITypeFactory*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITypeFactory*);
    ULONG(STDMETHODCALLTYPE *Release)(ITypeFactory*);
    HRESULT(STDMETHODCALLTYPE *CreateFromTypeInfo)(ITypeFactory*,ITypeInfo*,REFIID,IUnknown**);
    END_INTERFACE
} ITypeFactoryVtbl;

interface ITypeFactory {
    CONST_VTBL struct ITypeFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITypeFactory_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ITypeFactory_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ITypeFactory_Release(This)  (This)->lpVtbl->Release(This)
#define ITypeFactory_CreateFromTypeInfo(This,pTypeInfo,riid,ppv)  (This)->lpVtbl->CreateFromTypeInfo(This,pTypeInfo,riid,ppv)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ITypeFactory_CreateFromTypeInfo_Proxy(ITypeFactory*,ITypeInfo*,REFIID,IUnknown**);
void __RPC_STUB ITypeFactory_CreateFromTypeInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ITypeFactory_INTERFACE_DEFINED__ */


#ifndef __ITypeMarshal_INTERFACE_DEFINED__
#define __ITypeMarshal_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITypeMarshal;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000002D-0000-0000-C000-000000000046") ITypeMarshal:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Size(PVOID,DWORD,PVOID,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Marshal(PVOID,DWORD,PVOID,ULONG,BYTE*,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unmarshal(PVOID,DWORD,ULONG,BYTE*,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Free(PVOID) = 0;
};
#else
typedef struct ITypeMarshalVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ITypeMarshal*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITypeMarshal*);
    ULONG(STDMETHODCALLTYPE *Release)(ITypeMarshal*);
    HRESULT(STDMETHODCALLTYPE *Size)(ITypeMarshal*,PVOID,DWORD,PVOID,ULONG*);
    HRESULT(STDMETHODCALLTYPE *Marshal)(ITypeMarshal*,PVOID,DWORD,PVOID,ULONG,BYTE*,ULONG*);
    HRESULT(STDMETHODCALLTYPE *Unmarshal)(ITypeMarshal*,PVOID,DWORD,ULONG,BYTE*,ULONG*);
    HRESULT(STDMETHODCALLTYPE *Free)(ITypeMarshal*,PVOID);
    END_INTERFACE
} ITypeMarshalVtbl;

interface ITypeMarshal {
    CONST_VTBL struct ITypeMarshalVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITypeMarshal_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ITypeMarshal_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ITypeMarshal_Release(This)  (This)->lpVtbl->Release(This)
#define ITypeMarshal_Size(This,pvType,dwDestContext,pvDestContext,pSize)  (This)->lpVtbl->Size(This,pvType,dwDestContext,pvDestContext,pSize)
#define ITypeMarshal_Marshal(This,pvType,dwDestContext,pvDestContext,cbBufferLength,pBuffer,pcbWritten)  (This)->lpVtbl->Marshal(This,pvType,dwDestContext,pvDestContext,cbBufferLength,pBuffer,pcbWritten)
#define ITypeMarshal_Unmarshal(This,pvType,dwFlags,cbBufferLength,pBuffer,pcbRead)  (This)->lpVtbl->Unmarshal(This,pvType,dwFlags,cbBufferLength,pBuffer,pcbRead)
#define ITypeMarshal_Free(This,pvType)  (This)->lpVtbl->Free(This,pvType)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE ITypeMarshal_Size_Proxy(ITypeMarshal*,PVOID,DWORD,PVOID,ULONG*);
void __RPC_STUB ITypeMarshal_Size_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeMarshal_Marshal_Proxy(ITypeMarshal*,PVOID,DWORD,PVOID,ULONG,BYTE*,ULONG*);
void __RPC_STUB ITypeMarshal_Marshal_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeMarshal_Unmarshal_Proxy(ITypeMarshal*,PVOID,DWORD,ULONG,BYTE*,ULONG*);
void __RPC_STUB ITypeMarshal_Unmarshal_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE ITypeMarshal_Free_Proxy(ITypeMarshal*,PVOID);
void __RPC_STUB ITypeMarshal_Free_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __ITypeMarshal_INTERFACE_DEFINED__ */


#ifndef __IRecordInfo_INTERFACE_DEFINED__
#define __IRecordInfo_INTERFACE_DEFINED__

typedef IRecordInfo *LPRECORDINFO;
EXTERN_C const IID IID_IRecordInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000002F-0000-0000-C000-000000000046") IRecordInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE RecordInit(PVOID) = 0;
    virtual HRESULT STDMETHODCALLTYPE RecordClear(PVOID) = 0;
    virtual HRESULT STDMETHODCALLTYPE RecordCopy(PVOID,PVOID) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetGuid(GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSize(ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(ITypeInfo**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetField(PVOID,LPCOLESTR,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFieldNoCopy(PVOID,LPCOLESTR,VARIANT*,PVOID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE PutField(ULONG,PVOID,LPCOLESTR,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE PutFieldNoCopy(ULONG,PVOID,LPCOLESTR,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFieldNames(ULONG*,BSTR*) = 0;
    virtual BOOL STDMETHODCALLTYPE IsMatchingType(IRecordInfo*) = 0;
    virtual PVOID STDMETHODCALLTYPE RecordCreate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE RecordCreateCopy(PVOID,PVOID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE RecordDestroy(PVOID) = 0;
};
#else
typedef struct IRecordInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IRecordInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IRecordInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(IRecordInfo*);
    HRESULT(STDMETHODCALLTYPE *RecordInit)(IRecordInfo*,PVOID);
    HRESULT(STDMETHODCALLTYPE *RecordClear)(IRecordInfo*,PVOID);
    HRESULT(STDMETHODCALLTYPE *RecordCopy)(IRecordInfo*,PVOID,PVOID);
    HRESULT(STDMETHODCALLTYPE *GetGuid)(IRecordInfo*,GUID*);
    HRESULT(STDMETHODCALLTYPE *GetName)(IRecordInfo*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IRecordInfo*,ULONG*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IRecordInfo*,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetField)(IRecordInfo*,PVOID,LPCOLESTR,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *GetFieldNoCopy)(IRecordInfo*,PVOID,LPCOLESTR,VARIANT*,PVOID*);
    HRESULT(STDMETHODCALLTYPE *PutField)(IRecordInfo*,ULONG,PVOID,LPCOLESTR,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *PutFieldNoCopy)(IRecordInfo*,ULONG,PVOID,LPCOLESTR,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *GetFieldNames)(IRecordInfo*,ULONG*,BSTR*);
    BOOL(STDMETHODCALLTYPE *IsMatchingType)(IRecordInfo*,IRecordInfo*);
    PVOID(STDMETHODCALLTYPE *RecordCreate)(IRecordInfo*);
    HRESULT(STDMETHODCALLTYPE *RecordCreateCopy)(IRecordInfo*,PVOID,PVOID*);
    HRESULT(STDMETHODCALLTYPE *RecordDestroy)(IRecordInfo*,PVOID);
    END_INTERFACE
} IRecordInfoVtbl;

interface IRecordInfo {
    CONST_VTBL struct IRecordInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IRecordInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IRecordInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IRecordInfo_Release(This)  (This)->lpVtbl->Release(This)
#define IRecordInfo_RecordInit(This,pvNew)  (This)->lpVtbl->RecordInit(This,pvNew)
#define IRecordInfo_RecordClear(This,pvExisting)  (This)->lpVtbl->RecordClear(This,pvExisting)
#define IRecordInfo_RecordCopy(This,pvExisting,pvNew)  (This)->lpVtbl->RecordCopy(This,pvExisting,pvNew)
#define IRecordInfo_GetGuid(This,pguid)  (This)->lpVtbl->GetGuid(This,pguid)
#define IRecordInfo_GetName(This,pbstrName)  (This)->lpVtbl->GetName(This,pbstrName)
#define IRecordInfo_GetSize(This,pcbSize)  (This)->lpVtbl->GetSize(This,pcbSize)
#define IRecordInfo_GetTypeInfo(This,ppTypeInfo)  (This)->lpVtbl->GetTypeInfo(This,ppTypeInfo)
#define IRecordInfo_GetField(This,pvData,szFieldName,pvarField)  (This)->lpVtbl->GetField(This,pvData,szFieldName,pvarField)
#define IRecordInfo_GetFieldNoCopy(This,pvData,szFieldName,pvarField,ppvDataCArray)  (This)->lpVtbl->GetFieldNoCopy(This,pvData,szFieldName,pvarField,ppvDataCArray)
#define IRecordInfo_PutField(This,wFlags,pvData,szFieldName,pvarField)  (This)->lpVtbl->PutField(This,wFlags,pvData,szFieldName,pvarField)
#define IRecordInfo_PutFieldNoCopy(This,wFlags,pvData,szFieldName,pvarField)  (This)->lpVtbl->PutFieldNoCopy(This,wFlags,pvData,szFieldName,pvarField)
#define IRecordInfo_GetFieldNames(This,pcNames,rgBstrNames)  (This)->lpVtbl->GetFieldNames(This,pcNames,rgBstrNames)
#define IRecordInfo_IsMatchingType(This,pRecordInfo)  (This)->lpVtbl->IsMatchingType(This,pRecordInfo)
#define IRecordInfo_RecordCreate(This)  (This)->lpVtbl->RecordCreate(This)
#define IRecordInfo_RecordCreateCopy(This,pvSource,ppvDest)  (This)->lpVtbl->RecordCreateCopy(This,pvSource,ppvDest)
#define IRecordInfo_RecordDestroy(This,pvRecord)  (This)->lpVtbl->RecordDestroy(This,pvRecord)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IRecordInfo_RecordInit_Proxy(IRecordInfo*,PVOID);
void __RPC_STUB IRecordInfo_RecordInit_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_RecordClear_Proxy(IRecordInfo*,PVOID);
void __RPC_STUB IRecordInfo_RecordClear_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_RecordCopy_Proxy(IRecordInfo*,PVOID,PVOID);
void __RPC_STUB IRecordInfo_RecordCopy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_GetGuid_Proxy(IRecordInfo*,GUID*);
void __RPC_STUB IRecordInfo_GetGuid_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_GetName_Proxy(IRecordInfo*,BSTR*);
void __RPC_STUB IRecordInfo_GetName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_GetSize_Proxy(IRecordInfo*,ULONG*);
void __RPC_STUB IRecordInfo_GetSize_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_GetTypeInfo_Proxy(IRecordInfo*,ITypeInfo**);
void __RPC_STUB IRecordInfo_GetTypeInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_GetField_Proxy(IRecordInfo*,PVOID,LPCOLESTR,VARIANT*);
void __RPC_STUB IRecordInfo_GetField_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_GetFieldNoCopy_Proxy(IRecordInfo*,PVOID,LPCOLESTR,VARIANT*,PVOID*);
void __RPC_STUB IRecordInfo_GetFieldNoCopy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_PutField_Proxy(IRecordInfo*,ULONG,PVOID,LPCOLESTR,VARIANT*);
void __RPC_STUB IRecordInfo_PutField_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_PutFieldNoCopy_Proxy(IRecordInfo*,ULONG,PVOID,LPCOLESTR,VARIANT*);
void __RPC_STUB IRecordInfo_PutFieldNoCopy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_GetFieldNames_Proxy(IRecordInfo*,ULONG*,BSTR*);
void __RPC_STUB IRecordInfo_GetFieldNames_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
BOOL STDMETHODCALLTYPE IRecordInfo_IsMatchingType_Proxy(IRecordInfo*,IRecordInfo*);
void __RPC_STUB IRecordInfo_IsMatchingType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
PVOID STDMETHODCALLTYPE IRecordInfo_RecordCreate_Proxy(IRecordInfo*);
void __RPC_STUB IRecordInfo_RecordCreate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_RecordCreateCopy_Proxy(IRecordInfo*,PVOID,PVOID*);
void __RPC_STUB IRecordInfo_RecordCreateCopy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IRecordInfo_RecordDestroy_Proxy(IRecordInfo*,PVOID);
void __RPC_STUB IRecordInfo_RecordDestroy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __IRecordInfo_INTERFACE_DEFINED__ */


#ifndef __IErrorLog_INTERFACE_DEFINED__
#define __IErrorLog_INTERFACE_DEFINED__

typedef IErrorLog *LPERRORLOG;
EXTERN_C const IID IID_IErrorLog;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3127CA40-446E-11CE-8135-00AA004BB851") IErrorLog:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AddError(LPCOLESTR,EXCEPINFO*) = 0;
};
#else
typedef struct IErrorLogVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IErrorLog*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IErrorLog*);
    ULONG(STDMETHODCALLTYPE *Release)(IErrorLog*);
    HRESULT(STDMETHODCALLTYPE *AddError)(IErrorLog*,LPCOLESTR,EXCEPINFO*);
    END_INTERFACE
} IErrorLogVtbl;

interface IErrorLog {
    CONST_VTBL struct IErrorLogVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IErrorLog_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IErrorLog_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IErrorLog_Release(This)  (This)->lpVtbl->Release(This)
#define IErrorLog_AddError(This,pszPropName,pExcepInfo)  (This)->lpVtbl->AddError(This,pszPropName,pExcepInfo)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IErrorLog_AddError_Proxy(IErrorLog*,LPCOLESTR,EXCEPINFO*);
void __RPC_STUB IErrorLog_AddError_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __IErrorLog_INTERFACE_DEFINED__ */


#ifndef __IPropertyBag_INTERFACE_DEFINED__
#define __IPropertyBag_INTERFACE_DEFINED__

typedef IPropertyBag *LPPROPERTYBAG;
EXTERN_C const IID IID_IPropertyBag;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("55272A00-42CB-11CE-8135-00AA004BB851") IPropertyBag:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Read(LPCOLESTR,VARIANT*,IErrorLog*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Write(LPCOLESTR,VARIANT*) = 0;
};
#else
typedef struct IPropertyBagVtbl {
    BEGIN_INTERFACE HRESULT(STDMETHODCALLTYPE *QueryInterface)(IPropertyBag*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IPropertyBag*);
    ULONG(STDMETHODCALLTYPE *Release)(IPropertyBag*);
    HRESULT(STDMETHODCALLTYPE *Read)(IPropertyBag*,LPCOLESTR,VARIANT*,IErrorLog*);
    HRESULT(STDMETHODCALLTYPE *Write)(IPropertyBag*,LPCOLESTR,VARIANT*);
    END_INTERFACE
} IPropertyBagVtbl;

interface IPropertyBag {
    CONST_VTBL struct IPropertyBagVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPropertyBag_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IPropertyBag_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IPropertyBag_Release(This)  (This)->lpVtbl->Release(This)
#define IPropertyBag_Read(This,pszPropName,pVar,pErrorLog)  (This)->lpVtbl->Read(This,pszPropName,pVar,pErrorLog)
#define IPropertyBag_Write(This,pszPropName,pVar)  (This)->lpVtbl->Write(This,pszPropName,pVar)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IPropertyBag_RemoteRead_Proxy(IPropertyBag*,LPCOLESTR,VARIANT*,IErrorLog*,DWORD,IUnknown*);
void __RPC_STUB IPropertyBag_RemoteRead_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);
HRESULT STDMETHODCALLTYPE IPropertyBag_Write_Proxy(IPropertyBag*,LPCOLESTR,VARIANT*);
void __RPC_STUB IPropertyBag_Write_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE _pRpcMessage,DWORD*);

#endif /* __IPropertyBag_INTERFACE_DEFINED__ */


extern RPC_IF_HANDLE __MIDL_itf_oaidl_0103_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_oaidl_0103_v0_0_s_ifspec;

unsigned long __RPC_USER BSTR_UserSize(unsigned long*,unsigned long,BSTR*);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long*,unsigned char*,BSTR*);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long*,unsigned char*,BSTR*);
void __RPC_USER BSTR_UserFree(unsigned long*,BSTR*);
unsigned long __RPC_USER CLEANLOCALSTORAGE_UserSize(unsigned long*,unsigned long,CLEANLOCALSTORAGE*);
unsigned char *__RPC_USER CLEANLOCALSTORAGE_UserMarshal(unsigned long*,unsigned char*,CLEANLOCALSTORAGE*);
unsigned char *__RPC_USER CLEANLOCALSTORAGE_UserUnmarshal(unsigned long*,unsigned char*,CLEANLOCALSTORAGE*);
void __RPC_USER CLEANLOCALSTORAGE_UserFree(unsigned long*,CLEANLOCALSTORAGE*);
unsigned long __RPC_USER VARIANT_UserSize(unsigned long*,unsigned long,VARIANT*);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long*,unsigned char*,VARIANT*);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long*,unsigned char*,VARIANT*);
void __RPC_USER VARIANT_UserFree(unsigned long*,VARIANT*);
HRESULT STDMETHODCALLTYPE IDispatch_Invoke_Proxy(IDispatch*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
HRESULT STDMETHODCALLTYPE IDispatch_Invoke_Stub(IDispatch*,DISPID,REFIID,LCID,DWORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*,UINT,UINT*,VARIANTARG*);
HRESULT STDMETHODCALLTYPE IEnumVARIANT_Next_Proxy(IEnumVARIANT*,ULONG,VARIANT*,ULONG*);
HRESULT STDMETHODCALLTYPE IEnumVARIANT_Next_Stub(IEnumVARIANT*,ULONG,VARIANT*,ULONG*);
HRESULT STDMETHODCALLTYPE ITypeComp_Bind_Proxy(ITypeComp*,LPOLESTR,ULONG,WORD,ITypeInfo**,DESCKIND*,BINDPTR*);
HRESULT STDMETHODCALLTYPE ITypeComp_Bind_Stub(ITypeComp*,LPOLESTR,ULONG,WORD,ITypeInfo**,DESCKIND*,LPFUNCDESC*,LPVARDESC*,ITypeComp**,CLEANLOCALSTORAGE*);
HRESULT STDMETHODCALLTYPE ITypeComp_BindType_Proxy(ITypeComp*,LPOLESTR,ULONG,ITypeInfo**,ITypeComp**);
HRESULT STDMETHODCALLTYPE ITypeComp_BindType_Stub(ITypeComp*,LPOLESTR,ULONG,ITypeInfo**);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetTypeAttr_Proxy(ITypeInfo*,TYPEATTR**);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetTypeAttr_Stub(ITypeInfo*,LPTYPEATTR*,CLEANLOCALSTORAGE*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetFuncDesc_Proxy(ITypeInfo*,UINT,FUNCDESC**);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetFuncDesc_Stub(ITypeInfo*,UINT,LPFUNCDESC*,CLEANLOCALSTORAGE*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetVarDesc_Proxy(ITypeInfo*,UINT,VARDESC**);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetVarDesc_Stub(ITypeInfo*,UINT,LPVARDESC*,CLEANLOCALSTORAGE*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetNames_Proxy(ITypeInfo*,MEMBERID,BSTR*,UINT,UINT*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetNames_Stub(ITypeInfo*,MEMBERID,BSTR*,UINT,UINT*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetIDsOfNames_Proxy(ITypeInfo*,LPOLESTR*,UINT,MEMBERID*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetIDsOfNames_Stub(ITypeInfo*);
HRESULT STDMETHODCALLTYPE ITypeInfo_Invoke_Proxy(ITypeInfo*,PVOID,MEMBERID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
HRESULT STDMETHODCALLTYPE ITypeInfo_Invoke_Stub(ITypeInfo*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetDocumentation_Proxy(ITypeInfo*,MEMBERID,BSTR*,BSTR*,DWORD*,BSTR*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetDocumentation_Stub(ITypeInfo*,MEMBERID,DWORD,BSTR*,BSTR*,DWORD*,BSTR*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetDllEntry_Proxy(ITypeInfo*,MEMBERID,INVOKEKIND,BSTR*,BSTR*,WORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetDllEntry_Stub(ITypeInfo*,MEMBERID,INVOKEKIND,DWORD,BSTR*,BSTR*,WORD*);
HRESULT STDMETHODCALLTYPE ITypeInfo_AddressOfMember_Proxy(ITypeInfo*,MEMBERID,INVOKEKIND,PVOID*);
HRESULT STDMETHODCALLTYPE ITypeInfo_AddressOfMember_Stub(ITypeInfo*);
HRESULT STDMETHODCALLTYPE ITypeInfo_CreateInstance_Proxy(ITypeInfo*,IUnknown*,REFIID,PVOID*);
HRESULT STDMETHODCALLTYPE ITypeInfo_CreateInstance_Stub(ITypeInfo*,REFIID,IUnknown**);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetContainingTypeLib_Proxy(ITypeInfo*,ITypeLib**,UINT*);
HRESULT STDMETHODCALLTYPE ITypeInfo_GetContainingTypeLib_Stub(ITypeInfo*,ITypeLib**,UINT*);
void STDMETHODCALLTYPE ITypeInfo_ReleaseTypeAttr_Proxy(ITypeInfo*,TYPEATTR*);
HRESULT STDMETHODCALLTYPE ITypeInfo_ReleaseTypeAttr_Stub(ITypeInfo*);
void STDMETHODCALLTYPE ITypeInfo_ReleaseFuncDesc_Proxy(ITypeInfo*,FUNCDESC*);
HRESULT STDMETHODCALLTYPE ITypeInfo_ReleaseFuncDesc_Stub(ITypeInfo*);
void STDMETHODCALLTYPE ITypeInfo_ReleaseVarDesc_Proxy(ITypeInfo*,VARDESC*);
HRESULT STDMETHODCALLTYPE ITypeInfo_ReleaseVarDesc_Stub(ITypeInfo*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetDocumentation2_Proxy(ITypeInfo2*,MEMBERID,LCID,BSTR*,DWORD*,BSTR*);
HRESULT STDMETHODCALLTYPE ITypeInfo2_GetDocumentation2_Stub(ITypeInfo2*,MEMBERID,LCID,DWORD,BSTR*,DWORD*,BSTR*);
UINT STDMETHODCALLTYPE ITypeLib_GetTypeInfoCount_Proxy(ITypeLib*);
HRESULT STDMETHODCALLTYPE ITypeLib_GetTypeInfoCount_Stub(ITypeLib*,UINT*);
HRESULT STDMETHODCALLTYPE ITypeLib_GetLibAttr_Proxy(ITypeLib*,TLIBATTR**);
HRESULT STDMETHODCALLTYPE ITypeLib_GetLibAttr_Stub(ITypeLib*,LPTLIBATTR*,CLEANLOCALSTORAGE*);
HRESULT STDMETHODCALLTYPE ITypeLib_GetDocumentation_Proxy(ITypeLib*,INT,BSTR*,BSTR*,DWORD*,BSTR*);
HRESULT STDMETHODCALLTYPE ITypeLib_GetDocumentation_Stub(ITypeLib*,INT,DWORD,BSTR*,BSTR*,DWORD*,BSTR*);
HRESULT STDMETHODCALLTYPE ITypeLib_IsName_Proxy(ITypeLib*,LPOLESTR,ULONG,BOOL*);
HRESULT STDMETHODCALLTYPE ITypeLib_IsName_Stub(ITypeLib*,LPOLESTR,ULONG,BOOL*,BSTR*);
HRESULT STDMETHODCALLTYPE ITypeLib_FindName_Proxy(ITypeLib*,LPOLESTR,ULONG,ITypeInfo**,MEMBERID*,USHORT*);
HRESULT STDMETHODCALLTYPE ITypeLib_FindName_Stub(ITypeLib*,LPOLESTR,ULONG,ITypeInfo**,MEMBERID*,USHORT*,BSTR*);
void STDMETHODCALLTYPE ITypeLib_ReleaseTLibAttr_Proxy(ITypeLib*,TLIBATTR*);
HRESULT STDMETHODCALLTYPE ITypeLib_ReleaseTLibAttr_Stub(ITypeLib*);
HRESULT STDMETHODCALLTYPE ITypeLib2_GetLibStatistics_Proxy(ITypeLib2*,ULONG*,ULONG*);
HRESULT STDMETHODCALLTYPE ITypeLib2_GetLibStatistics_Stub(ITypeLib2*,ULONG*,ULONG*);
HRESULT STDMETHODCALLTYPE ITypeLib2_GetDocumentation2_Proxy(ITypeLib2*,INT,LCID,BSTR*,DWORD*,BSTR*);
HRESULT STDMETHODCALLTYPE ITypeLib2_GetDocumentation2_Stub(ITypeLib2*,INT,LCID,DWORD,BSTR*,DWORD*,BSTR*);
HRESULT STDMETHODCALLTYPE IPropertyBag_Read_Proxy(IPropertyBag*,LPCOLESTR,VARIANT*,IErrorLog*);
HRESULT STDMETHODCALLTYPE IPropertyBag_Read_Stub(IPropertyBag*,LPCOLESTR,VARIANT*,IErrorLog*,DWORD,IUnknown*);


#ifdef __cplusplus
}
#endif

#endif /* _OAIDL_H */
