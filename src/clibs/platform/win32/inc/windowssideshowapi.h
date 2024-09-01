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
#endif 

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif 

#ifndef __windowssideshowapi_h__
#define __windowssideshowapi_h__

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __ISideShowSession_FWD_DEFINED__
#define __ISideShowSession_FWD_DEFINED__
typedef interface ISideShowSession ISideShowSession;
#endif

#ifndef __ISideShowNotificationManager_FWD_DEFINED__
#define __ISideShowNotificationManager_FWD_DEFINED__
typedef interface ISideShowNotificationManager ISideShowNotificationManager;
#endif

#ifndef __ISideShowNotification_FWD_DEFINED__
#define __ISideShowNotification_FWD_DEFINED__
typedef interface ISideShowNotification ISideShowNotification;
#endif

#ifndef __ISideShowContentManager_FWD_DEFINED__
#define __ISideShowContentManager_FWD_DEFINED__
typedef interface ISideShowContentManager ISideShowContentManager;
#endif

#ifndef __ISideShowContent_FWD_DEFINED__
#define __ISideShowContent_FWD_DEFINED__
typedef interface ISideShowContent ISideShowContent;
#endif

#ifndef __ISideShowEvents_FWD_DEFINED__
#define __ISideShowEvents_FWD_DEFINED__
typedef interface ISideShowEvents ISideShowEvents;
#endif

#ifndef __ISideShowCapabilities_FWD_DEFINED__
#define __ISideShowCapabilities_FWD_DEFINED__
typedef interface ISideShowCapabilities ISideShowCapabilities;
#endif

#ifndef __ISideShowCapabilitiesCollection_FWD_DEFINED__
#define __ISideShowCapabilitiesCollection_FWD_DEFINED__
typedef interface ISideShowCapabilitiesCollection ISideShowCapabilitiesCollection;
#endif

#ifndef __SideShowSession_FWD_DEFINED__
#define __SideShowSession_FWD_DEFINED__

#ifdef __cplusplus
typedef class SideShowSession SideShowSession;
#else
typedef struct SideShowSession SideShowSession;
#endif

#endif

#ifndef __SideShowNotification_FWD_DEFINED__
#define __SideShowNotification_FWD_DEFINED__

#ifdef __cplusplus
typedef class SideShowNotification SideShowNotification;
#else
typedef struct SideShowNotification SideShowNotification;
#endif

#endif

#include "oaidl.h"
#include "propsys.h"

#ifdef __cplusplus
extern "C"{
#endif 

#if (_WIN32_WINNT >= 0x0600)
typedef GUID APPLICATION_ID;
typedef GUID ENDPOINT_ID;
typedef LPWSTR DEVICE_ID;
typedef REFGUID REFAPPLICATION_ID;
typedef REFGUID REFENDPOINT_ID;
typedef ENDPOINT_ID *PENDPOINT_ID;
typedef APPLICATION_ID *PAPPLICATION_ID;
typedef DEVICE_ID *PDEVICE_ID;
typedef unsigned long CONTENT_ID;
typedef CONTENT_ID *PCONTENT_ID;
typedef unsigned long NOTIFICATION_ID;
typedef NOTIFICATION_ID *PNOTIFICATION_ID;

extern RPC_IF_HANDLE __MIDL_itf_windowssideshowapi_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_windowssideshowapi_0000_0000_v0_0_s_ifspec;

#ifndef __ISideShowSession_INTERFACE_DEFINED__
#define __ISideShowSession_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISideShowSession;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e22331ee-9e7d-4922-9fc2-ab7aa41ce491") ISideShowSession : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE RegisterContent(REFAPPLICATION_ID,REFENDPOINT_ID,ISideShowContentManager**) = 0;
    virtual HRESULT STDMETHODCALLTYPE RegisterNotifications(REFAPPLICATION_ID,ISideShowNotificationManager**) = 0;
};
#else
typedef struct ISideShowSessionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISideShowSession*,REFIID,void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISideShowSession*);
    ULONG (STDMETHODCALLTYPE *Release)(ISideShowSession*);
    HRESULT (STDMETHODCALLTYPE *RegisterContent)(ISideShowSession*,REFAPPLICATION_ID,REFENDPOINT_ID,ISideShowContentManager**);
    HRESULT (STDMETHODCALLTYPE *RegisterNotifications)(ISideShowSession*,REFAPPLICATION_ID,ISideShowNotificationManager**);
    END_INTERFACE
} ISideShowSessionVtbl;

interface ISideShowSession {
    CONST_VTBL struct ISideShowSessionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISideShowSession_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISideShowSession_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISideShowSession_Release(This)  ((This)->lpVtbl->Release(This))
#define ISideShowSession_RegisterContent(This,in_applicationId,in_endpointId,out_ppIContent)  ((This)->lpVtbl->RegisterContent(This,in_applicationId,in_endpointId,out_ppIContent))
#define ISideShowSession_RegisterNotifications(This,in_applicationId,out_ppINotification)  ((This)->lpVtbl->RegisterNotifications(This,in_applicationId,out_ppINotification))
#endif

#endif

#endif

#ifndef __ISideShowNotificationManager_INTERFACE_DEFINED__
#define __ISideShowNotificationManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISideShowNotificationManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("63cea909-f2b9-4302-b5e1-c68e6d9ab833") ISideShowNotificationManager : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Show(ISideShowNotification*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Revoke(const NOTIFICATION_ID) = 0;
    virtual HRESULT STDMETHODCALLTYPE RevokeAll(void) = 0;
};
#else
typedef struct ISideShowNotificationManagerVtbl
{
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISideShowNotificationManager*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISideShowNotificationManager*);
    ULONG (STDMETHODCALLTYPE *Release)(ISideShowNotificationManager*);
    HRESULT (STDMETHODCALLTYPE *Show)(ISideShowNotificationManager*,ISideShowNotification*);
    HRESULT (STDMETHODCALLTYPE *Revoke)(ISideShowNotificationManager*,const NOTIFICATION_ID);
    HRESULT (STDMETHODCALLTYPE *RevokeAll)(ISideShowNotificationManager*);
    END_INTERFACE
} ISideShowNotificationManagerVtbl;

interface ISideShowNotificationManager {
    CONST_VTBL struct ISideShowNotificationManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISideShowNotificationManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISideShowNotificationManager_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISideShowNotificationManager_Release(This)  ((This)->lpVtbl->Release(This))
#define ISideShowNotificationManager_Show(This,in_pINotification)  ((This)->lpVtbl->Show(This,in_pINotification))
#define ISideShowNotificationManager_Revoke(This,in_notificationId)  ((This)->lpVtbl->Revoke(This,in_notificationId))
#define ISideShowNotificationManager_RevokeAll(This)  ((This)->lpVtbl->RevokeAll(This))
#endif

#endif

#endif

#ifndef __ISideShowNotification_INTERFACE_DEFINED__
#define __ISideShowNotification_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISideShowNotification;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("03c93300-8ab2-41c5-9b79-46127a30e148") ISideShowNotification : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_NotificationId(PNOTIFICATION_ID) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NotificationId(NOTIFICATION_ID) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Title(LPWSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Title(LPWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Message(LPWSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Message(LPWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Image(HICON*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Image(HICON) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ExpirationTime(SYSTEMTIME*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ExpirationTime(SYSTEMTIME*) = 0;
};
#else
typedef struct ISideShowNotificationVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISideShowNotification*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISideShowNotification*);
    ULONG (STDMETHODCALLTYPE *Release)(ISideShowNotification*);
    HRESULT (STDMETHODCALLTYPE *get_NotificationId)(ISideShowNotification*,PNOTIFICATION_ID);
    HRESULT (STDMETHODCALLTYPE *put_NotificationId)(ISideShowNotification*,NOTIFICATION_ID);
    HRESULT (STDMETHODCALLTYPE *get_Title)(ISideShowNotification*,LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *put_Title)(ISideShowNotification*,LPWSTR);
    HRESULT (STDMETHODCALLTYPE *get_Message)(ISideShowNotification*,LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *put_Message)(ISideShowNotification*,LPWSTR);
    HRESULT (STDMETHODCALLTYPE *get_Image)(ISideShowNotification*,HICON*);
    HRESULT (STDMETHODCALLTYPE *put_Image)(ISideShowNotification*,HICON);
    HRESULT (STDMETHODCALLTYPE *get_ExpirationTime)(ISideShowNotification*,SYSTEMTIME*);
    HRESULT (STDMETHODCALLTYPE *put_ExpirationTime)(ISideShowNotification*,SYSTEMTIME*);
    END_INTERFACE
} ISideShowNotificationVtbl;

interface ISideShowNotification {
    CONST_VTBL struct ISideShowNotificationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISideShowNotification_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISideShowNotification_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISideShowNotification_Release(This)  ((This)->lpVtbl->Release(This))
#define ISideShowNotification_get_NotificationId(This,out_pNotificationId)  ((This)->lpVtbl->get_NotificationId(This,out_pNotificationId))
#define ISideShowNotification_put_NotificationId(This,in_notificationId)  ((This)->lpVtbl->put_NotificationId(This,in_notificationId))
#define ISideShowNotification_get_Title(This,out_ppwszTitle)  ((This)->lpVtbl->get_Title(This,out_ppwszTitle))
#define ISideShowNotification_put_Title(This,in_pwszTitle)  ((This)->lpVtbl->put_Title(This,in_pwszTitle))
#define ISideShowNotification_get_Message(This,out_ppwszMessage)  ((This)->lpVtbl->get_Message(This,out_ppwszMessage))
#define ISideShowNotification_put_Message(This,in_pwszMessage)  ((This)->lpVtbl->put_Message(This,in_pwszMessage))
#define ISideShowNotification_get_Image(This,out_phIcon)  ((This)->lpVtbl->get_Image(This,out_phIcon))
#define ISideShowNotification_put_Image(This,in_hIcon)  ((This)->lpVtbl->put_Image(This,in_hIcon))
#define ISideShowNotification_get_ExpirationTime(This,out_pTime)  ((This)->lpVtbl->get_ExpirationTime(This,out_pTime))
#define ISideShowNotification_put_ExpirationTime(This,in_pTime)  ((This)->lpVtbl->put_ExpirationTime(This,in_pTime))
#endif

#endif

#endif

#ifndef __ISideShowContentManager_INTERFACE_DEFINED__
#define __ISideShowContentManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISideShowContentManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a5d5b66b-eef9-41db-8d7e-e17c33ab10b0") ISideShowContentManager : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Add(ISideShowContent*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(const CONTENT_ID) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAll(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEventSink(ISideShowEvents*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceCapabilities(ISideShowCapabilitiesCollection**) = 0;
};
#else
typedef struct ISideShowContentManagerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISideShowContentManager*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISideShowContentManager*);
    ULONG (STDMETHODCALLTYPE *Release)(ISideShowContentManager*);
    HRESULT (STDMETHODCALLTYPE *Add)(ISideShowContentManager*,ISideShowContent*);
    HRESULT (STDMETHODCALLTYPE *Remove)(ISideShowContentManager*,const CONTENT_ID);
    HRESULT (STDMETHODCALLTYPE *RemoveAll)(ISideShowContentManager*);
    HRESULT (STDMETHODCALLTYPE *SetEventSink)(ISideShowContentManager*,ISideShowEvents*);
    HRESULT (STDMETHODCALLTYPE *GetDeviceCapabilities)(ISideShowContentManager*,ISideShowCapabilitiesCollection**);
    END_INTERFACE
} ISideShowContentManagerVtbl;

interface ISideShowContentManager {
    CONST_VTBL struct ISideShowContentManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISideShowContentManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISideShowContentManager_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISideShowContentManager_Release(This)  ((This)->lpVtbl->Release(This))
#define ISideShowContentManager_Add(This,in_pIContent)  ((This)->lpVtbl->Add(This,in_pIContent))
#define ISideShowContentManager_Remove(This,in_contentId)  ((This)->lpVtbl->Remove(This,in_contentId))
#define ISideShowContentManager_RemoveAll(This)  ((This)->lpVtbl->RemoveAll(This))
#define ISideShowContentManager_SetEventSink(This,in_pIEvents)  ((This)->lpVtbl->SetEventSink(This,in_pIEvents))
#define ISideShowContentManager_GetDeviceCapabilities(This,out_ppCollection)  ((This)->lpVtbl->GetDeviceCapabilities(This,out_ppCollection))
#endif

#endif

#endif

#ifndef __ISideShowContent_INTERFACE_DEFINED__
#define __ISideShowContent_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISideShowContent;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("c18552ed-74ff-4fec-be07-4cfed29d4887") ISideShowContent : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetContent(ISideShowCapabilities*,DWORD*,BYTE**) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentId(PCONTENT_ID) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DifferentiateContent(BOOL*) = 0;
};
#else
typedef struct ISideShowContentVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISideShowContent*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISideShowContent*);
    ULONG (STDMETHODCALLTYPE *Release)(ISideShowContent*);
    HRESULT (STDMETHODCALLTYPE *GetContent)(ISideShowContent*,ISideShowCapabilities*,DWORD*,BYTE**);
    HRESULT (STDMETHODCALLTYPE *get_ContentId)(ISideShowContent*,PCONTENT_ID);
    HRESULT (STDMETHODCALLTYPE *get_DifferentiateContent)(ISideShowContent*,BOOL*);
    END_INTERFACE
} ISideShowContentVtbl;

interface ISideShowContent {
    CONST_VTBL struct ISideShowContentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISideShowContent_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISideShowContent_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISideShowContent_Release(This)  ((This)->lpVtbl->Release(This))
#define ISideShowContent_GetContent(This,in_pICapabilities,out_pdwSize,out_ppbData)  ((This)->lpVtbl->GetContent(This,in_pICapabilities,out_pdwSize,out_ppbData))
#define ISideShowContent_get_ContentId(This,out_pcontentId)  ((This)->lpVtbl->get_ContentId(This,out_pcontentId))
#define ISideShowContent_get_DifferentiateContent(This,out_pfDifferentiateContent)  ((This)->lpVtbl->get_DifferentiateContent(This,out_pfDifferentiateContent))
#endif

#endif

#endif

#ifndef __ISideShowEvents_INTERFACE_DEFINED__
#define __ISideShowEvents_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISideShowEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("61feca4c-deb4-4a7e-8d75-51f1132d615b") ISideShowEvents : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ContentMissing(const CONTENT_ID,ISideShowContent**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplicationEvent(ISideShowCapabilities*,const DWORD,const DWORD,const BYTE*) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeviceAdded(ISideShowCapabilities*) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeviceRemoved(ISideShowCapabilities*) = 0;
};
#else
typedef struct ISideShowEventsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISideShowEvents*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISideShowEvents*);
    ULONG (STDMETHODCALLTYPE *Release)(ISideShowEvents*);
    HRESULT (STDMETHODCALLTYPE *ContentMissing)(ISideShowEvents*,const CONTENT_ID,ISideShowContent**);
    HRESULT (STDMETHODCALLTYPE *ApplicationEvent)(ISideShowEvents*,ISideShowCapabilities*,const DWORD,const DWORD,const BYTE*);
    HRESULT (STDMETHODCALLTYPE *DeviceAdded)(ISideShowEvents*,ISideShowCapabilities*);
    HRESULT (STDMETHODCALLTYPE *DeviceRemoved)(ISideShowEvents*,ISideShowCapabilities*);
    END_INTERFACE
} ISideShowEventsVtbl;

interface ISideShowEvents {
    CONST_VTBL struct ISideShowEventsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISideShowEvents_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISideShowEvents_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISideShowEvents_Release(This)  ((This)->lpVtbl->Release(This))
#define ISideShowEvents_ContentMissing(This,in_contentId,out_ppIContent)  ((This)->lpVtbl->ContentMissing(This,in_contentId,out_ppIContent))
#define ISideShowEvents_ApplicationEvent(This,in_pICapabilities,in_dwEventId,in_dwEventSize,in_pbEventData)  ((This)->lpVtbl->ApplicationEvent(This,in_pICapabilities,in_dwEventId,in_dwEventSize,in_pbEventData))
#define ISideShowEvents_DeviceAdded(This,in_pIDevice)  ((This)->lpVtbl->DeviceAdded(This,in_pIDevice))
#define ISideShowEvents_DeviceRemoved(This,in_pIDevice)  ((This)->lpVtbl->DeviceRemoved(This,in_pIDevice))
#endif

#endif

#endif

#ifndef __ISideShowCapabilities_INTERFACE_DEFINED__
#define __ISideShowCapabilities_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISideShowCapabilities;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("535e1379-c09e-4a54-a511-597bab3a72b8") ISideShowCapabilities : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCapability(REFPROPERTYKEY,PROPVARIANT*) = 0;
};
#else
typedef struct ISideShowCapabilitiesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISideShowCapabilities*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISideShowCapabilities*);
    ULONG (STDMETHODCALLTYPE *Release)(ISideShowCapabilities*);
    HRESULT (STDMETHODCALLTYPE *GetCapability)(ISideShowCapabilities*,REFPROPERTYKEY,PROPVARIANT*);
    END_INTERFACE
} ISideShowCapabilitiesVtbl;

interface ISideShowCapabilities {
    CONST_VTBL struct ISideShowCapabilitiesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISideShowCapabilities_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISideShowCapabilities_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISideShowCapabilities_Release(This)  ((This)->lpVtbl->Release(This))
#define ISideShowCapabilities_GetCapability(This,in_keyCapability,inout_pValue)  ((This)->lpVtbl->GetCapability(This,in_keyCapability,inout_pValue))
#endif

#endif

#endif

#ifndef __ISideShowCapabilitiesCollection_INTERFACE_DEFINED__
#define __ISideShowCapabilitiesCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISideShowCapabilitiesCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("50305597-5e0d-4ff7-b3af-33d0d9bd52dd") ISideShowCapabilitiesCollection : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCount(DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAt(DWORD,ISideShowCapabilities**) = 0;
};
#else
typedef struct ISideShowCapabilitiesCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISideShowCapabilitiesCollection*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISideShowCapabilitiesCollection*);
    ULONG (STDMETHODCALLTYPE *Release)(ISideShowCapabilitiesCollection*);
    HRESULT (STDMETHODCALLTYPE *GetCount)(ISideShowCapabilitiesCollection*,DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetAt)(ISideShowCapabilitiesCollection*,DWORD,ISideShowCapabilities**);
    END_INTERFACE
} ISideShowCapabilitiesCollectionVtbl;

interface ISideShowCapabilitiesCollection {
    CONST_VTBL struct ISideShowCapabilitiesCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISideShowCapabilitiesCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISideShowCapabilitiesCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISideShowCapabilitiesCollection_Release(This)  ((This)->lpVtbl->Release(This))
#define ISideShowCapabilitiesCollection_GetCount(This,out_pdwCount)  ((This)->lpVtbl->GetCount(This,out_pdwCount))
#define ISideShowCapabilitiesCollection_GetAt(This,in_dwIndex,out_ppCapabilities)  ((This)->lpVtbl->GetAt(This,in_dwIndex,out_ppCapabilities))
#endif

#endif

#endif

#ifndef __WindowsSideShowApiLibrary_LIBRARY_DEFINED__
#define __WindowsSideShowApiLibrary_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_WindowsSideShowApiLibrary;
EXTERN_C const CLSID CLSID_SideShowSession;

#ifdef __cplusplus
class DECLSPEC_UUID("e20543b9-f785-4ea2-981e-c4ffa76bbc7c") SideShowSession;
#endif

EXTERN_C const CLSID CLSID_SideShowNotification;

#ifdef __cplusplus
class DECLSPEC_UUID("0ce3e86f-d5cd-4525-a766-1abab1a752f5") SideShowNotification;
#endif

#endif

#endif

extern RPC_IF_HANDLE __MIDL_itf_windowssideshowapi_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_windowssideshowapi_0000_0008_v0_0_s_ifspec;

unsigned long __RPC_USER BSTR_UserSize(unsigned long *,unsigned long,BSTR*);
unsigned char * __RPC_USER BSTR_UserMarshal(unsigned long*,unsigned char*,BSTR*);
unsigned char * __RPC_USER BSTR_UserUnmarshal(unsigned long *,unsigned char*,BSTR*);
void __RPC_USER BSTR_UserFree(unsigned long*,BSTR*);

unsigned long __RPC_USER HICON_UserSize(unsigned long*,unsigned long,HICON*);
unsigned char * __RPC_USER HICON_UserMarshal(unsigned long *,unsigned char*,HICON*);
unsigned char * __RPC_USER HICON_UserUnmarshal(unsigned long *,unsigned char*,HICON*);
void __RPC_USER HICON_UserFree(unsigned long*,HICON*);

unsigned long __RPC_USER LPSAFEARRAY_UserSize(unsigned long*,unsigned long,LPSAFEARRAY*);
unsigned char * __RPC_USER LPSAFEARRAY_UserMarshal(unsigned long *,unsigned char*,LPSAFEARRAY*);
unsigned char * __RPC_USER LPSAFEARRAY_UserUnmarshal(unsigned long *,unsigned char*,LPSAFEARRAY*);
void __RPC_USER LPSAFEARRAY_UserFree(unsigned long*,LPSAFEARRAY*);

unsigned long __RPC_USER BSTR_UserSize64(unsigned long*, unsigned long,BSTR*);
unsigned char * __RPC_USER BSTR_UserMarshal64(unsigned long*,unsigned char*,BSTR*);
unsigned char * __RPC_USER BSTR_UserUnmarshal64(unsigned long*,unsigned char*,BSTR*);
void __RPC_USER BSTR_UserFree64(unsigned long*,BSTR*);

unsigned long __RPC_USER HICON_UserSize64(unsigned long*,unsigned long,HICON*);
unsigned char * __RPC_USER HICON_UserMarshal64(unsigned long*,unsigned char*,HICON*);
unsigned char * __RPC_USER HICON_UserUnmarshal64(unsigned long*,unsigned char*,HICON*);
void __RPC_USER HICON_UserFree64(unsigned long*,HICON*);

unsigned long __RPC_USER LPSAFEARRAY_UserSize64(unsigned long*, unsigned long,LPSAFEARRAY*);
unsigned char * __RPC_USER LPSAFEARRAY_UserMarshal64(unsigned long*,unsigned char*,LPSAFEARRAY*);
unsigned char * __RPC_USER LPSAFEARRAY_UserUnmarshal64(unsigned long*,unsigned char*,LPSAFEARRAY*);
void __RPC_USER LPSAFEARRAY_UserFree64(unsigned long*,LPSAFEARRAY*);

#ifdef __cplusplus
}
#endif

#endif
