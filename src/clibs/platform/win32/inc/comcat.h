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
#endif

#ifndef _COMCAT_H
#define _COMCAT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IEnumGUID_FWD_DEFINED__
#define __IEnumGUID_FWD_DEFINED__
typedef interface IEnumGUID IEnumGUID;
#endif

#ifndef __IEnumCATEGORYINFO_FWD_DEFINED__
#define __IEnumCATEGORYINFO_FWD_DEFINED__
typedef interface IEnumCATEGORYINFO IEnumCATEGORYINFO;
#endif

#ifndef __ICatRegister_FWD_DEFINED__
#define __ICatRegister_FWD_DEFINED__
typedef interface ICatRegister ICatRegister;
#endif

#ifndef __ICatInformation_FWD_DEFINED__
#define __ICatInformation_FWD_DEFINED__
typedef interface ICatInformation ICatInformation;
#endif

#include "unknwn.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

EXTERN_C const CLSID CLSID_StdComponentCategoriesMgr;

typedef GUID CATID;
typedef REFGUID REFCATID;

#define IID_IEnumCLSID  IID_IEnumGUID
#define IEnumCLSID  IEnumGUID
#define LPENUMCLSID  LPENUMGUID
#define CATID_NULL  GUID_NULL
#define IsEqualCATID(rcatid1,rcatid2)  IsEqualGUID(rcatid1,rcatid2)
#define IID_IEnumCATID  IID_IEnumGUID
#define IEnumCATID  IEnumGUID

EXTERN_C const CATID CATID_Insertable;
EXTERN_C const CATID CATID_Control;
EXTERN_C const CATID CATID_Programmable;
EXTERN_C const CATID CATID_IsShortcut;
EXTERN_C const CATID CATID_NeverShowExt;
EXTERN_C const CATID CATID_DocObject;
EXTERN_C const CATID CATID_Printable;
EXTERN_C const CATID CATID_RequiresDataPathHost;
EXTERN_C const CATID CATID_PersistsToMoniker;
EXTERN_C const CATID CATID_PersistsToStorage;
EXTERN_C const CATID CATID_PersistsToStreamInit;
EXTERN_C const CATID CATID_PersistsToStream;
EXTERN_C const CATID CATID_PersistsToMemory;
EXTERN_C const CATID CATID_PersistsToFile;
EXTERN_C const CATID CATID_PersistsToPropertyBag;
EXTERN_C const CATID CATID_InternetAware;
EXTERN_C const CATID CATID_DesignTimeUIActivatableControl;

#ifndef _LPENUMGUID_DEFINED
#define _LPENUMGUID_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_comcat_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0000_v0_0_s_ifspec;

#ifndef __IEnumGUID_INTERFACE_DEFINED__
#define __IEnumGUID_INTERFACE_DEFINED__

typedef IEnumGUID *LPENUMGUID;

EXTERN_C const IID IID_IEnumGUID;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0002E000-0000-0000-C000-000000000046") IEnumGUID:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG,GUID*,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumGUID**) = 0;
};
#else
typedef struct IEnumGUIDVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumGUID*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IEnumGUID*);
    ULONG (STDMETHODCALLTYPE *Release)(IEnumGUID*);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumGUID*,ULONG,GUID*,ULONG*);
    HRESULT (STDMETHODCALLTYPE *Skip)(IEnumGUID*,ULONG);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumGUID*);
    HRESULT (STDMETHODCALLTYPE *Clone)(IEnumGUID*,IEnumGUID**);
    END_INTERFACE
} IEnumGUIDVtbl;

interface IEnumGUID {
    CONST_VTBL struct IEnumGUIDVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumGUID_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEnumGUID_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumGUID_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumGUID_Next(This,celt,rgelt,pceltFetched)  (This)->lpVtbl->Next(This,celt,rgelt,pceltFetched)
#define IEnumGUID_Skip(This,celt)  (This)->lpVtbl->Skip(This,celt)
#define IEnumGUID_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumGUID_Clone(This,ppenum)  (This)->lpVtbl->Clone(This,ppenum)
#endif

#endif

HRESULT STDMETHODCALLTYPE IEnumGUID_Next_Proxy(IEnumGUID*,ULONG,GUID*,ULONG*);
void __RPC_STUB IEnumGUID_Next_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumGUID_Skip_Proxy(IEnumGUID*,ULONG);
void __RPC_STUB IEnumGUID_Skip_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumGUID_Reset_Proxy(IEnumGUID*);
void __RPC_STUB IEnumGUID_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumGUID_Clone_Proxy(IEnumGUID*,IEnumGUID**);
void __RPC_STUB IEnumGUID_Clone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#endif
#ifndef _LPENUMCATEGORYINFO_DEFINED
#define _LPENUMCATEGORYINFO_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_comcat_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0009_v0_0_s_ifspec;

#ifndef __IEnumCATEGORYINFO_INTERFACE_DEFINED__
#define __IEnumCATEGORYINFO_INTERFACE_DEFINED__

typedef IEnumCATEGORYINFO *LPENUMCATEGORYINFO;

typedef struct tagCATEGORYINFO {
    CATID catid;
    LCID lcid;
    OLECHAR szDescription[128];
} CATEGORYINFO,*LPCATEGORYINFO;

EXTERN_C const IID IID_IEnumCATEGORYINFO;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0002E011-0000-0000-C000-000000000046") IEnumCATEGORYINFO:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG,CATEGORYINFO*,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumCATEGORYINFO**) = 0;
};
#else
typedef struct IEnumCATEGORYINFOVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumCATEGORYINFO*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IEnumCATEGORYINFO*);
    ULONG (STDMETHODCALLTYPE *Release)(IEnumCATEGORYINFO*);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumCATEGORYINFO*,ULONG,CATEGORYINFO*,ULONG*);
    HRESULT (STDMETHODCALLTYPE *Skip)(IEnumCATEGORYINFO*,ULONG);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumCATEGORYINFO*);
    HRESULT (STDMETHODCALLTYPE *Clone)(IEnumCATEGORYINFO*,IEnumCATEGORYINFO**);
    END_INTERFACE
} IEnumCATEGORYINFOVtbl;

interface IEnumCATEGORYINFO {
    CONST_VTBL struct IEnumCATEGORYINFOVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumCATEGORYINFO_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEnumCATEGORYINFO_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumCATEGORYINFO_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumCATEGORYINFO_Next(This,celt,rgelt,pceltFetched)  (This)->lpVtbl->Next(This,celt,rgelt,pceltFetched)
#define IEnumCATEGORYINFO_Skip(This,celt)  (This)->lpVtbl->Skip(This,celt)
#define IEnumCATEGORYINFO_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumCATEGORYINFO_Clone(This,ppenum)  (This)->lpVtbl->Clone(This,ppenum)
#endif

#endif

HRESULT STDMETHODCALLTYPE IEnumCATEGORYINFO_Next_Proxy(IEnumCATEGORYINFO*,ULONG,CATEGORYINFO*,ULONG*);
void __RPC_STUB IEnumCATEGORYINFO_Next_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCATEGORYINFO_Skip_Proxy(IEnumCATEGORYINFO*,ULONG);
void __RPC_STUB IEnumCATEGORYINFO_Skip_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCATEGORYINFO_Reset_Proxy(IEnumCATEGORYINFO*);
void __RPC_STUB IEnumCATEGORYINFO_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCATEGORYINFO_Clone_Proxy(IEnumCATEGORYINFO*,IEnumCATEGORYINFO**);
void __RPC_STUB IEnumCATEGORYINFO_Clone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

#endif

#ifndef _LPCATREGISTER_DEFINED
#define _LPCATREGISTER_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_comcat_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0010_v0_0_s_ifspec;

#ifndef __ICatRegister_INTERFACE_DEFINED__
#define __ICatRegister_INTERFACE_DEFINED__

typedef ICatRegister *LPCATREGISTER;

EXTERN_C const IID IID_ICatRegister;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0002E012-0000-0000-C000-000000000046") ICatRegister:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE RegisterCategories(ULONG,CATEGORYINFO[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnRegisterCategories(ULONG,CATID[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE RegisterClassImplCategories(REFCLSID,ULONG,CATID[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnRegisterClassImplCategories(REFCLSID,ULONG,CATID[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE RegisterClassReqCategories(REFCLSID,ULONG,CATID[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnRegisterClassReqCategories(REFCLSID,ULONG,CATID[]) = 0;
};
#else
typedef struct ICatRegisterVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ICatRegister*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ICatRegister*);
    ULONG (STDMETHODCALLTYPE *Release)(ICatRegister*);
    HRESULT (STDMETHODCALLTYPE *RegisterCategories)(ICatRegister*,ULONG,CATEGORYINFO[]);
    HRESULT (STDMETHODCALLTYPE *UnRegisterCategories)(ICatRegister*,ULONG,CATID[]);
    HRESULT (STDMETHODCALLTYPE *RegisterClassImplCategories)(ICatRegister*,REFCLSID,ULONG,CATID[]);
    HRESULT (STDMETHODCALLTYPE *UnRegisterClassImplCategories)(ICatRegister*,REFCLSID,ULONG,CATID[]);
    HRESULT (STDMETHODCALLTYPE *RegisterClassReqCategories)(ICatRegister*,REFCLSID,ULONG,CATID[]);
    HRESULT (STDMETHODCALLTYPE *UnRegisterClassReqCategories)(ICatRegister*,REFCLSID,ULONG,CATID[]);
    END_INTERFACE
} ICatRegisterVtbl;

interface ICatRegister {
    CONST_VTBL struct ICatRegisterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICatRegister_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICatRegister_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICatRegister_Release(This)  (This)->lpVtbl->Release(This)
#define ICatRegister_RegisterCategories(This,cCategories,rgCategoryInfo)  (This)->lpVtbl->RegisterCategories(This,cCategories,rgCategoryInfo)
#define ICatRegister_UnRegisterCategories(This,cCategories,rgcatid)  (This)->lpVtbl->UnRegisterCategories(This,cCategories,rgcatid)
#define ICatRegister_RegisterClassImplCategories(This,rclsid,cCategories,rgcatid)  (This)->lpVtbl->RegisterClassImplCategories(This,rclsid,cCategories,rgcatid)
#define ICatRegister_UnRegisterClassImplCategories(This,rclsid,cCategories,rgcatid)  (This)->lpVtbl->UnRegisterClassImplCategories(This,rclsid,cCategories,rgcatid)
#define ICatRegister_RegisterClassReqCategories(This,rclsid,cCategories,rgcatid)  (This)->lpVtbl->RegisterClassReqCategories(This,rclsid,cCategories,rgcatid)
#define ICatRegister_UnRegisterClassReqCategories(This,rclsid,cCategories,rgcatid)  (This)->lpVtbl->UnRegisterClassReqCategories(This,rclsid,cCategories,rgcatid)
#endif

#endif

HRESULT STDMETHODCALLTYPE ICatRegister_RegisterCategories_Proxy(ICatRegister*,ULONG,CATEGORYINFO[]);
void __RPC_STUB ICatRegister_RegisterCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatRegister_UnRegisterCategories_Proxy(ICatRegister*,ULONG,CATID[]);
void __RPC_STUB ICatRegister_UnRegisterCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatRegister_RegisterClassImplCategories_Proxy(ICatRegister*,REFCLSID,ULONG,CATID[]);
void __RPC_STUB ICatRegister_RegisterClassImplCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatRegister_UnRegisterClassImplCategories_Proxy(ICatRegister*,REFCLSID,ULONG,CATID[]);
void __RPC_STUB ICatRegister_UnRegisterClassImplCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatRegister_RegisterClassReqCategories_Proxy(ICatRegister*,REFCLSID,ULONG,CATID[]);
void __RPC_STUB ICatRegister_RegisterClassReqCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatRegister_UnRegisterClassReqCategories_Proxy(ICatRegister*,REFCLSID,ULONG,CATID[]);
void __RPC_STUB ICatRegister_UnRegisterClassReqCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#endif

#ifndef _LPCATINFORMATION_DEFINED
#define _LPCATINFORMATION_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_comcat_0011_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0011_v0_0_s_ifspec;

#ifndef __ICatInformation_INTERFACE_DEFINED__
#define __ICatInformation_INTERFACE_DEFINED__

typedef ICatInformation *LPCATINFORMATION;

EXTERN_C const IID IID_ICatInformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0002E013-0000-0000-C000-000000000046") ICatInformation:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE EnumCategories(LCID,IEnumCATEGORYINFO**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCategoryDesc(REFCATID,LCID,LPWSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumClassesOfCategories(ULONG,CATID[],ULONG,CATID[],IEnumGUID**) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsClassOfCategories(REFCLSID,ULONG,CATID[],ULONG,CATID[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumImplCategoriesOfClass(REFCLSID,IEnumGUID**) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumReqCategoriesOfClass(REFCLSID,IEnumGUID**) = 0;
};
#else
typedef struct ICatInformationVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ICatInformation*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ICatInformation*);
    ULONG (STDMETHODCALLTYPE *Release)(ICatInformation*);
    HRESULT (STDMETHODCALLTYPE *EnumCategories)(ICatInformation*,LCID,IEnumCATEGORYINFO**);
    HRESULT (STDMETHODCALLTYPE *GetCategoryDesc)(ICatInformation*,REFCATID,LCID,LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *EnumClassesOfCategories)(ICatInformation*,ULONG,CATID[],ULONG,CATID[],IEnumGUID**);
    HRESULT (STDMETHODCALLTYPE *IsClassOfCategories)(ICatInformation*,REFCLSID,ULONG,CATID[],ULONG,CATID[]);
    HRESULT (STDMETHODCALLTYPE *EnumImplCategoriesOfClass)(ICatInformation*,REFCLSID,IEnumGUID**);
    HRESULT (STDMETHODCALLTYPE *EnumReqCategoriesOfClass)(ICatInformation*,REFCLSID,IEnumGUID**);
    END_INTERFACE
} ICatInformationVtbl;

interface ICatInformation {
    CONST_VTBL struct ICatInformationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICatInformation_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICatInformation_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICatInformation_Release(This)  (This)->lpVtbl->Release(This)
#define ICatInformation_EnumCategories(This,lcid,ppenumCategoryInfo)  (This)->lpVtbl->EnumCategories(This,lcid,ppenumCategoryInfo)
#define ICatInformation_GetCategoryDesc(This,rcatid,lcid,pszDesc)  (This)->lpVtbl->GetCategoryDesc(This,rcatid,lcid,pszDesc)
#define ICatInformation_EnumClassesOfCategories(This,cImplemented,rgcatidImpl,cRequired,rgcatidReq,ppenumClsid)  (This)->lpVtbl->EnumClassesOfCategories(This,cImplemented,rgcatidImpl,cRequired,rgcatidReq,ppenumClsid)
#define ICatInformation_IsClassOfCategories(This,rclsid,cImplemented,rgcatidImpl,cRequired,rgcatidReq)  (This)->lpVtbl->IsClassOfCategories(This,rclsid,cImplemented,rgcatidImpl,cRequired,rgcatidReq)
#define ICatInformation_EnumImplCategoriesOfClass(This,rclsid,ppenumCatid)  (This)->lpVtbl->EnumImplCategoriesOfClass(This,rclsid,ppenumCatid)
#define ICatInformation_EnumReqCategoriesOfClass(This,rclsid,ppenumCatid)  (This)->lpVtbl->EnumReqCategoriesOfClass(This,rclsid,ppenumCatid)
#endif

#endif

HRESULT STDMETHODCALLTYPE ICatInformation_EnumCategories_Proxy(ICatInformation*,LCID,IEnumCATEGORYINFO**);
void __RPC_STUB ICatInformation_EnumCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatInformation_GetCategoryDesc_Proxy(ICatInformation*,REFCATID,LCID,LPWSTR*);
void __RPC_STUB ICatInformation_GetCategoryDesc_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatInformation_RemoteEnumClassesOfCategories_Proxy(ICatInformation*,ULONG,CATID[],ULONG,CATID[],IEnumGUID**);
void __RPC_STUB ICatInformation_RemoteEnumClassesOfCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatInformation_RemoteIsClassOfCategories_Proxy(ICatInformation*,REFCLSID,ULONG,CATID[],ULONG,CATID[]);
void __RPC_STUB ICatInformation_RemoteIsClassOfCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatInformation_EnumImplCategoriesOfClass_Proxy(ICatInformation*,REFCLSID,IEnumGUID**);
void __RPC_STUB ICatInformation_EnumImplCategoriesOfClass_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatInformation_EnumReqCategoriesOfClass_Proxy(ICatInformation*,REFCLSID,IEnumGUID**);
void __RPC_STUB ICatInformation_EnumReqCategoriesOfClass_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#endif

extern RPC_IF_HANDLE __MIDL_itf_comcat_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0012_v0_0_s_ifspec;

HRESULT STDMETHODCALLTYPE ICatInformation_EnumClassesOfCategories_Proxy(ICatInformation*,ULONG,CATID[],ULONG,CATID[],IEnumGUID**);
HRESULT STDMETHODCALLTYPE ICatInformation_EnumClassesOfCategories_Stub(ICatInformation*,ULONG,CATID[],ULONG,CATID[],IEnumGUID**);
HRESULT STDMETHODCALLTYPE ICatInformation_IsClassOfCategories_Proxy(ICatInformation*,REFCLSID,ULONG,CATID[],ULONG,CATID[]);
HRESULT STDMETHODCALLTYPE ICatInformation_IsClassOfCategories_Stub(ICatInformation*,REFCLSID,ULONG,CATID[],ULONG,CATID[]);

#ifdef __cplusplus
}
#endif

#endif /* _COMCAT_H */
