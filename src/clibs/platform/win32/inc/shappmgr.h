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

#ifndef _SHAPPMGR_H
#define _SHAPPMGR_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IShellApp_FWD_DEFINED__
#define __IShellApp_FWD_DEFINED__
typedef interface IShellApp IShellApp;
#endif

#ifndef __IPublishedApp_FWD_DEFINED__
#define __IPublishedApp_FWD_DEFINED__
typedef interface IPublishedApp IPublishedApp;
#endif

#ifndef __IEnumPublishedApps_FWD_DEFINED__
#define __IEnumPublishedApps_FWD_DEFINED__
typedef interface IEnumPublishedApps IEnumPublishedApps;
#endif

#ifndef __IAppPublisher_FWD_DEFINED__
#define __IAppPublisher_FWD_DEFINED__
typedef interface IAppPublisher IAppPublisher;
#endif

#include "oaidl.h"
#include "ocidl.h"
#include "appmgmt.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *); 

extern RPC_IF_HANDLE __MIDL_itf_shappmgr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shappmgr_0000_v0_0_s_ifspec;

#ifndef __IShellApp_INTERFACE_DEFINED__
#define __IShellApp_INTERFACE_DEFINED__

typedef enum _tagAppInfoFlags {
    AIM_DISPLAYNAME = 0x1,
    AIM_VERSION = 0x2,
    AIM_PUBLISHER = 0x4,
    AIM_PRODUCTID = 0x8,
    AIM_REGISTEREDOWNER = 0x10,
    AIM_REGISTEREDCOMPANY = 0x20,
    AIM_LANGUAGE = 0x40,
    AIM_SUPPORTURL = 0x80,
    AIM_SUPPORTTELEPHONE = 0x100,
    AIM_HELPLINK = 0x200,
    AIM_INSTALLLOCATION = 0x400,
    AIM_INSTALLSOURCE = 0x800,
    AIM_INSTALLDATE = 0x1000,
    AIM_CONTACT = 0x4000,
    AIM_COMMENTS = 0x8000,
    AIM_IMAGE = 0x20000,
    AIM_READMEURL = 0x40000,
    AIM_UPDATEINFOURL = 0x80000
} APPINFODATAFLAGS;

typedef struct _AppInfoData {
    DWORD cbSize;
    DWORD dwMask;
    LPWSTR pszDisplayName;
    LPWSTR pszVersion;
    LPWSTR pszPublisher;
    LPWSTR pszProductID;
    LPWSTR pszRegisteredOwner;
    LPWSTR pszRegisteredCompany;
    LPWSTR pszLanguage;
    LPWSTR pszSupportUrl;
    LPWSTR pszSupportTelephone;
    LPWSTR pszHelpLink;
    LPWSTR pszInstallLocation;
    LPWSTR pszInstallSource;
    LPWSTR pszInstallDate;
    LPWSTR pszContact;
    LPWSTR pszComments;
    LPWSTR pszImage;
    LPWSTR pszReadmeUrl;
    LPWSTR pszUpdateInfoUrl;
} APPINFODATA, *PAPPINFODATA;

typedef enum _tagAppActionFlags {
    APPACTION_INSTALL = 0x1,
    APPACTION_UNINSTALL = 0x2,
    APPACTION_MODIFY = 0x4,
    APPACTION_REPAIR = 0x8,
    APPACTION_UPGRADE = 0x10,
    APPACTION_CANGETSIZE = 0x20,
    APPACTION_MODIFYREMOVE = 0x80,
    APPACTION_ADDLATER = 0x100,
    APPACTION_UNSCHEDULE = 0x200
} APPACTIONFLAGS;

typedef struct _tagSlowAppInfo {
    ULONGLONG ullSize;
    FILETIME ftLastUsed;
    int iTimesUsed;
    LPWSTR pszImage;
} SLOWAPPINFO;

typedef struct _tagSlowAppInfo *PSLOWAPPINFO;

EXTERN_C const IID IID_IShellApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A3E14960-935F-11D1-B8B8-006008059382") IShellApp : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetAppInfo(PAPPINFODATA) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPossibleActions(DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSlowAppInfo(PSLOWAPPINFO) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedSlowAppInfo(PSLOWAPPINFO) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsInstalled(void) = 0;
};
#else
typedef struct IShellAppVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IShellApp*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IShellApp*);
    ULONG (STDMETHODCALLTYPE *Release)(IShellApp*);
    HRESULT (STDMETHODCALLTYPE *GetAppInfo)(IShellApp*,PAPPINFODATA);
    HRESULT (STDMETHODCALLTYPE *GetPossibleActions)(IShellApp*,DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetSlowAppInfo)(IShellApp*,PSLOWAPPINFO);
    HRESULT (STDMETHODCALLTYPE *GetCachedSlowAppInfo)(IShellApp*,PSLOWAPPINFO);
    HRESULT (STDMETHODCALLTYPE *IsInstalled)(IShellApp*);
    END_INTERFACE
} IShellAppVtbl;

interface IShellApp {
    CONST_VTBL struct IShellAppVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IShellApp_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IShellApp_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IShellApp_Release(This)  (This)->lpVtbl -> Release(This)
#define IShellApp_GetAppInfo(This,pai)  (This)->lpVtbl -> GetAppInfo(This,pai)
#define IShellApp_GetPossibleActions(This,pdwActions)  (This)->lpVtbl -> GetPossibleActions(This,pdwActions)
#define IShellApp_GetSlowAppInfo(This,psaid)  (This)->lpVtbl -> GetSlowAppInfo(This,psaid)
#define IShellApp_GetCachedSlowAppInfo(This,psaid)  (This)->lpVtbl -> GetCachedSlowAppInfo(This,psaid)
#define IShellApp_IsInstalled(This)  (This)->lpVtbl -> IsInstalled(This)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IShellApp_GetAppInfo_Proxy(IShellApp*,PAPPINFODATA);
void __RPC_STUB IShellApp_GetAppInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IShellApp_GetPossibleActions_Proxy(IShellApp*,DWORD*);
void __RPC_STUB IShellApp_GetPossibleActions_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IShellApp_GetSlowAppInfo_Proxy(IShellApp*,PSLOWAPPINFO);
void __RPC_STUB IShellApp_GetSlowAppInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IShellApp_GetCachedSlowAppInfo_Proxy(IShellApp*,PSLOWAPPINFO);
void __RPC_STUB IShellApp_GetCachedSlowAppInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IShellApp_IsInstalled_Proxy(IShellApp*);
void __RPC_STUB IShellApp_IsInstalled_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IShellApp_INTERFACE_DEFINED__ */

#ifndef __IPublishedApp_INTERFACE_DEFINED__
#define __IPublishedApp_INTERFACE_DEFINED__

typedef enum _tagPublishedAppInfoFlags {
    PAI_SOURCE = 0x1,
    PAI_ASSIGNEDTIME = 0x2,
    PAI_PUBLISHEDTIME = 0x4,
    PAI_SCHEDULEDTIME = 0x8,
    PAI_EXPIRETIME = 0x10
} PUBAPPINFOFLAGS;

typedef struct _PubAppInfo {
    DWORD cbSize;
    DWORD dwMask;
    LPWSTR pszSource;
    SYSTEMTIME stAssigned;
    SYSTEMTIME stPublished;
    SYSTEMTIME stScheduled;
    SYSTEMTIME stExpire;
} PUBAPPINFO;

typedef struct _PubAppInfo *PPUBAPPINFO;

EXTERN_C const IID IID_IPublishedApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1BC752E0-9046-11D1-B8B3-006008059382") IPublishedApp : public IShellApp {
    public:
    virtual HRESULT STDMETHODCALLTYPE Install(LPSYSTEMTIME) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPublishedAppInfo(PPUBAPPINFO) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unschedule(void) = 0;
};
#else
typedef struct IPublishedAppVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IPublishedApp*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IPublishedApp*);
    ULONG (STDMETHODCALLTYPE *Release)(IPublishedApp*);
    HRESULT (STDMETHODCALLTYPE *GetAppInfo)(IPublishedApp*,PAPPINFODATA);
    HRESULT (STDMETHODCALLTYPE *GetPossibleActions)(IPublishedApp*,DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetSlowAppInfo)(IPublishedApp*,PSLOWAPPINFO);
    HRESULT (STDMETHODCALLTYPE *GetCachedSlowAppInfo)(IPublishedApp*,PSLOWAPPINFO);
    HRESULT (STDMETHODCALLTYPE *IsInstalled)(IPublishedApp*);
    HRESULT (STDMETHODCALLTYPE *Install)(IPublishedApp*,LPSYSTEMTIME);
    HRESULT (STDMETHODCALLTYPE *GetPublishedAppInfo)(IPublishedApp*,PPUBAPPINFO);
    HRESULT (STDMETHODCALLTYPE *Unschedule)(IPublishedApp*);
    END_INTERFACE
} IPublishedAppVtbl;

interface IPublishedApp {
    CONST_VTBL struct IPublishedAppVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPublishedApp_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IPublishedApp_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IPublishedApp_Release(This)  (This)->lpVtbl -> Release(This)
#define IPublishedApp_GetAppInfo(This,pai)  (This)->lpVtbl -> GetAppInfo(This,pai)
#define IPublishedApp_GetPossibleActions(This,pdwActions)  (This)->lpVtbl -> GetPossibleActions(This,pdwActions)
#define IPublishedApp_GetSlowAppInfo(This,psaid)  (This)->lpVtbl -> GetSlowAppInfo(This,psaid)
#define IPublishedApp_GetCachedSlowAppInfo(This,psaid)  (This)->lpVtbl -> GetCachedSlowAppInfo(This,psaid)
#define IPublishedApp_IsInstalled(This)  (This)->lpVtbl -> IsInstalled(This)
#define IPublishedApp_Install(This,pstInstall)  (This)->lpVtbl -> Install(This,pstInstall)
#define IPublishedApp_GetPublishedAppInfo(This,ppai)  (This)->lpVtbl -> GetPublishedAppInfo(This,ppai)
#define IPublishedApp_Unschedule(This)  (This)->lpVtbl -> Unschedule(This)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IPublishedApp_Install_Proxy(IPublishedApp*,LPSYSTEMTIME);
void __RPC_STUB IPublishedApp_Install_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPublishedApp_GetPublishedAppInfo_Proxy(IPublishedApp*,PPUBAPPINFO);
void __RPC_STUB IPublishedApp_GetPublishedAppInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPublishedApp_Unschedule_Proxy(IPublishedApp*);
void __RPC_STUB IPublishedApp_Unschedule_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IPublishedApp_INTERFACE_DEFINED__ */

#ifndef __IEnumPublishedApps_INTERFACE_DEFINED__
#define __IEnumPublishedApps_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumPublishedApps;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0B124F8C-91F0-11D1-B8B5-006008059382") IEnumPublishedApps : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(IPublishedApp**) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
};
#else
typedef struct IEnumPublishedAppsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumPublishedApps*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IEnumPublishedApps*);
    ULONG (STDMETHODCALLTYPE *Release)(IEnumPublishedApps*);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumPublishedApps*,IPublishedApp**);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumPublishedApps*);
    END_INTERFACE
} IEnumPublishedAppsVtbl;

interface IEnumPublishedApps {
    CONST_VTBL struct IEnumPublishedAppsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumPublishedApps_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IEnumPublishedApps_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IEnumPublishedApps_Release(This)  (This)->lpVtbl -> Release(This)
#define IEnumPublishedApps_Next(This,pia)  (This)->lpVtbl -> Next(This,pia)
#define IEnumPublishedApps_Reset(This)  (This)->lpVtbl -> Reset(This)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IEnumPublishedApps_Next_Proxy(IEnumPublishedApps*,IPublishedApp**);
void __RPC_STUB IEnumPublishedApps_Next_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumPublishedApps_Reset_Proxy(IEnumPublishedApps*);
void __RPC_STUB IEnumPublishedApps_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IEnumPublishedApps_INTERFACE_DEFINED__ */

#ifndef __IAppPublisher_INTERFACE_DEFINED__
#define __IAppPublisher_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppPublisher;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("07250A10-9CF9-11D1-9076-006008059382") IAppPublisher : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetNumberOfCategories(DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCategories(APPCATEGORYINFOLIST*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNumberOfApps(DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumApps(GUID*,IEnumPublishedApps**) = 0;
};
#else
typedef struct IAppPublisherVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IAppPublisher*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IAppPublisher*);
    ULONG (STDMETHODCALLTYPE *Release)(IAppPublisher*);
    HRESULT (STDMETHODCALLTYPE *GetNumberOfCategories)(IAppPublisher*,DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetCategories)(IAppPublisher*,APPCATEGORYINFOLIST*);
    HRESULT (STDMETHODCALLTYPE *GetNumberOfApps)(IAppPublisher*,DWORD*);
    HRESULT (STDMETHODCALLTYPE *EnumApps)(IAppPublisher*,GUID*,IEnumPublishedApps**);
    END_INTERFACE
} IAppPublisherVtbl;

interface IAppPublisher {
    CONST_VTBL struct IAppPublisherVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppPublisher_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IAppPublisher_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IAppPublisher_Release(This)  (This)->lpVtbl -> Release(This)
#define IAppPublisher_GetNumberOfCategories(This,pdwCat)  (This)->lpVtbl -> GetNumberOfCategories(This,pdwCat)
#define IAppPublisher_GetCategories(This,pAppCategoryList)  (This)->lpVtbl -> GetCategories(This,pAppCategoryList)
#define IAppPublisher_GetNumberOfApps(This,pdwApps)  (This)->lpVtbl -> GetNumberOfApps(This,pdwApps)
#define IAppPublisher_EnumApps(This,pAppCategoryId,ppepa)  (This)->lpVtbl -> EnumApps(This,pAppCategoryId,ppepa)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IAppPublisher_GetNumberOfCategories_Proxy(IAppPublisher*,DWORD*);
void __RPC_STUB IAppPublisher_GetNumberOfCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IAppPublisher_GetCategories_Proxy(IAppPublisher*,APPCATEGORYINFOLIST*);
void __RPC_STUB IAppPublisher_GetCategories_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IAppPublisher_GetNumberOfApps_Proxy(IAppPublisher*,DWORD*);
void __RPC_STUB IAppPublisher_GetNumberOfApps_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IAppPublisher_EnumApps_Proxy(IAppPublisher*,GUID*,IEnumPublishedApps**);
void __RPC_STUB IAppPublisher_EnumApps_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IAppPublisher_INTERFACE_DEFINED__ */

#endif /* _SHAPPMGR_H */

extern RPC_IF_HANDLE __MIDL_itf_shappmgr_0261_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shappmgr_0261_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif
