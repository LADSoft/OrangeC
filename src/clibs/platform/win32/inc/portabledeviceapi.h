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
#endif /* COM_NO_WINDOWS_H */

#ifndef _PORTABLEDEVICEAPI_H
#define _PORTABLEDEVICEAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IPortableDeviceManager_FWD_DEFINED__
#define __IPortableDeviceManager_FWD_DEFINED__
typedef interface IPortableDeviceManager IPortableDeviceManager;
#endif /* __IPortableDeviceManager_FWD_DEFINED__ */

#ifndef __IPortableDevice_FWD_DEFINED__
#define __IPortableDevice_FWD_DEFINED__
typedef interface IPortableDevice IPortableDevice;
#endif /* __IPortableDevice_FWD_DEFINED__ */

#ifndef __IPortableDeviceContent_FWD_DEFINED__
#define __IPortableDeviceContent_FWD_DEFINED__
typedef interface IPortableDeviceContent IPortableDeviceContent;
#endif /* __IPortableDeviceContent_FWD_DEFINED__ */

#ifndef __IPortableDeviceContent2_FWD_DEFINED__
#define __IPortableDeviceContent2_FWD_DEFINED__
typedef interface IPortableDeviceContent2 IPortableDeviceContent2;
#endif /* __IPortableDeviceContent2_FWD_DEFINED__ */

#ifndef __IEnumPortableDeviceObjectIDs_FWD_DEFINED__
#define __IEnumPortableDeviceObjectIDs_FWD_DEFINED__
typedef interface IEnumPortableDeviceObjectIDs IEnumPortableDeviceObjectIDs;
#endif /* __IEnumPortableDeviceObjectIDs_FWD_DEFINED__ */

#ifndef __IPortableDeviceProperties_FWD_DEFINED__
#define __IPortableDeviceProperties_FWD_DEFINED__
typedef interface IPortableDeviceProperties IPortableDeviceProperties;
#endif /* __IPortableDeviceProperties_FWD_DEFINED__ */

#ifndef __IPortableDeviceResources_FWD_DEFINED__
#define __IPortableDeviceResources_FWD_DEFINED__
typedef interface IPortableDeviceResources IPortableDeviceResources;
#endif /* __IPortableDeviceResources_FWD_DEFINED__ */

#ifndef __IPortableDeviceCapabilities_FWD_DEFINED__
#define __IPortableDeviceCapabilities_FWD_DEFINED__
typedef interface IPortableDeviceCapabilities IPortableDeviceCapabilities;
#endif /* __IPortableDeviceCapabilities_FWD_DEFINED__ */

#ifndef __IPortableDeviceEventCallback_FWD_DEFINED__
#define __IPortableDeviceEventCallback_FWD_DEFINED__
typedef interface IPortableDeviceEventCallback IPortableDeviceEventCallback;
#endif /* __IPortableDeviceEventCallback_FWD_DEFINED__ */

#ifndef __IPortableDeviceDataStream_FWD_DEFINED__
#define __IPortableDeviceDataStream_FWD_DEFINED__
typedef interface IPortableDeviceDataStream IPortableDeviceDataStream;
#endif /* __IPortableDeviceDataStream_FWD_DEFINED__ */

#ifndef __IPortableDevicePropertiesBulk_FWD_DEFINED__
#define __IPortableDevicePropertiesBulk_FWD_DEFINED__
typedef interface IPortableDevicePropertiesBulk IPortableDevicePropertiesBulk;
#endif /* __IPortableDevicePropertiesBulk_FWD_DEFINED__ */

#ifndef __IPortableDevicePropertiesBulkCallback_FWD_DEFINED__
#define __IPortableDevicePropertiesBulkCallback_FWD_DEFINED__
typedef interface IPortableDevicePropertiesBulkCallback IPortableDevicePropertiesBulkCallback;
#endif /* __IPortableDevicePropertiesBulkCallback_FWD_DEFINED__ */

#ifndef __IPortableDeviceServiceManager_FWD_DEFINED__
#define __IPortableDeviceServiceManager_FWD_DEFINED__
typedef interface IPortableDeviceServiceManager IPortableDeviceServiceManager;
#endif /* __IPortableDeviceServiceManager_FWD_DEFINED__ */

#ifndef __IPortableDeviceService_FWD_DEFINED__
#define __IPortableDeviceService_FWD_DEFINED__
typedef interface IPortableDeviceService IPortableDeviceService;
#endif /* __IPortableDeviceService_FWD_DEFINED__ */

#ifndef __IPortableDeviceServiceCapabilities_FWD_DEFINED__
#define __IPortableDeviceServiceCapabilities_FWD_DEFINED__
typedef interface IPortableDeviceServiceCapabilities IPortableDeviceServiceCapabilities;
#endif /* __IPortableDeviceServiceCapabilities_FWD_DEFINED__ */

#ifndef __IPortableDeviceServiceMethods_FWD_DEFINED__
#define __IPortableDeviceServiceMethods_FWD_DEFINED__
typedef interface IPortableDeviceServiceMethods IPortableDeviceServiceMethods;
#endif /* __IPortableDeviceServiceMethods_FWD_DEFINED__ */

#ifndef __IPortableDeviceServiceMethodCallback_FWD_DEFINED__
#define __IPortableDeviceServiceMethodCallback_FWD_DEFINED__
typedef interface IPortableDeviceServiceMethodCallback IPortableDeviceServiceMethodCallback;
#endif /* __IPortableDeviceServiceMethodCallback_FWD_DEFINED__ */

#ifndef __IPortableDeviceDispatchFactory_FWD_DEFINED__
#define __IPortableDeviceDispatchFactory_FWD_DEFINED__
typedef interface IPortableDeviceDispatchFactory IPortableDeviceDispatchFactory;
#endif /* __IPortableDeviceDispatchFactory_FWD_DEFINED__ */

#ifndef __PortableDevice_FWD_DEFINED__
#define __PortableDevice_FWD_DEFINED__

#ifdef __cplusplus
typedef class PortableDevice PortableDevice;
#else
typedef struct PortableDevice PortableDevice;
#endif /* __cplusplus */

#endif /* __PortableDevice_FWD_DEFINED__ */

#ifndef __PortableDeviceManager_FWD_DEFINED__
#define __PortableDeviceManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class PortableDeviceManager PortableDeviceManager;
#else
typedef struct PortableDeviceManager PortableDeviceManager;
#endif /* __cplusplus */

#endif /* __PortableDeviceManager_FWD_DEFINED__ */

#ifndef __PortableDeviceService_FWD_DEFINED__
#define __PortableDeviceService_FWD_DEFINED__

#ifdef __cplusplus
typedef class PortableDeviceService PortableDeviceService;
#else
typedef struct PortableDeviceService PortableDeviceService;
#endif /* __cplusplus */

#endif /* __PortableDeviceService_FWD_DEFINED__ */

#ifndef __PortableDeviceDispatchFactory_FWD_DEFINED__
#define __PortableDeviceDispatchFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class PortableDeviceDispatchFactory PortableDeviceDispatchFactory;
#else
typedef struct PortableDeviceDispatchFactory PortableDeviceDispatchFactory;
#endif /* __cplusplus */

#endif /* __PortableDeviceDispatchFactory_FWD_DEFINED__ */

#ifndef __PortableDeviceFTM_FWD_DEFINED__
#define __PortableDeviceFTM_FWD_DEFINED__

#ifdef __cplusplus
typedef class PortableDeviceFTM PortableDeviceFTM;
#else
typedef struct PortableDeviceFTM PortableDeviceFTM;
#endif /* __cplusplus */

#endif /* __PortableDeviceFTM_FWD_DEFINED__ */

#ifndef __PortableDeviceServiceFTM_FWD_DEFINED__
#define __PortableDeviceServiceFTM_FWD_DEFINED__

#ifdef __cplusplus
typedef class PortableDeviceServiceFTM PortableDeviceServiceFTM;
#else
typedef struct PortableDeviceServiceFTM PortableDeviceServiceFTM;
#endif /* __cplusplus */

#endif /* __PortableDeviceServiceFTM_FWD_DEFINED__ */

#include "propidl.h"
#include "portabledevicetypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

#if (_WIN32_WINNT >= 0x0501)

extern RPC_IF_HANDLE __MIDL_itf_PortableDeviceApi_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_PortableDeviceApi_0000_0000_v0_0_s_ifspec;

#ifndef __IPortableDeviceManager_INTERFACE_DEFINED__
#define __IPortableDeviceManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceManager;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("a1567595-4c2f-4574-a6fa-ecef917b9a40")IPortableDeviceManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetDevices(LPWSTR * pPnPDeviceIDs, DWORD * pcPnPDeviceIDs) = 0;
    virtual HRESULT STDMETHODCALLTYPE RefreshDeviceList(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFriendlyName(LPCWSTR pszPnPDeviceID, WCHAR *pDeviceFriendlyName, DWORD *pcchDeviceFriendlyName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceDescription(LPCWSTR pszPnPDeviceID, WCHAR *pDeviceDescription, DWORD *pcchDeviceDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceManufacturer(LPCWSTR pszPnPDeviceID, WCHAR *pDeviceManufacturer, DWORD *pcchDeviceManufacturer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceProperty(LPCWSTR pszPnPDeviceID, LPCWSTR pszDevicePropertyName, BYTE *pData, DWORD *pcbData, DWORD *pdwType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPrivateDevices(LPWSTR *pPnPDeviceIDs, DWORD *pcPnPDeviceIDs) = 0;
};
#else 	/* C style interface */

typedef struct IPortableDeviceManagerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceManager * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceManager * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceManager * This);
    HRESULT (STDMETHODCALLTYPE *GetDevices) (IPortableDeviceManager * This, LPWSTR * pPnPDeviceIDs, DWORD * pcPnPDeviceIDs);
    HRESULT (STDMETHODCALLTYPE *RefreshDeviceList) (IPortableDeviceManager * This);
    HRESULT (STDMETHODCALLTYPE *GetDeviceFriendlyName) (IPortableDeviceManager * This, LPCWSTR pszPnPDeviceID, WCHAR * pDeviceFriendlyName, DWORD * pcchDeviceFriendlyName);
    HRESULT (STDMETHODCALLTYPE *GetDeviceDescription) (IPortableDeviceManager * This, LPCWSTR pszPnPDeviceID, WCHAR * pDeviceDescription, DWORD * pcchDeviceDescription);
    HRESULT (STDMETHODCALLTYPE *GetDeviceManufacturer) (IPortableDeviceManager * This, LPCWSTR pszPnPDeviceID, WCHAR * pDeviceManufacturer, DWORD * pcchDeviceManufacturer);
    HRESULT (STDMETHODCALLTYPE *GetDeviceProperty) (IPortableDeviceManager * This, LPCWSTR pszPnPDeviceID, LPCWSTR pszDevicePropertyName, BYTE * pData, DWORD * pcbData, DWORD * pdwType);
    HRESULT (STDMETHODCALLTYPE *GetPrivateDevices) (IPortableDeviceManager * This, LPWSTR * pPnPDeviceIDs, DWORD * pcPnPDeviceIDs);
    END_INTERFACE
} IPortableDeviceManagerVtbl;

interface IPortableDeviceManager {
    CONST_VTBL struct IPortableDeviceManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceManager_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceManager_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceManager_GetDevices(This,pPnPDeviceIDs,pcPnPDeviceIDs)  ((This)->lpVtbl->GetDevices(This,pPnPDeviceIDs,pcPnPDeviceIDs)) 
#define IPortableDeviceManager_RefreshDeviceList(This)  ((This)->lpVtbl->RefreshDeviceList(This)) 
#define IPortableDeviceManager_GetDeviceFriendlyName(This,pszPnPDeviceID,pDeviceFriendlyName,pcchDeviceFriendlyName)  ((This)->lpVtbl->GetDeviceFriendlyName(This,pszPnPDeviceID,pDeviceFriendlyName,pcchDeviceFriendlyName)) 
#define IPortableDeviceManager_GetDeviceDescription(This,pszPnPDeviceID,pDeviceDescription,pcchDeviceDescription)  ((This)->lpVtbl->GetDeviceDescription(This,pszPnPDeviceID,pDeviceDescription,pcchDeviceDescription)) 
#define IPortableDeviceManager_GetDeviceManufacturer(This,pszPnPDeviceID,pDeviceManufacturer,pcchDeviceManufacturer)  ((This)->lpVtbl->GetDeviceManufacturer(This,pszPnPDeviceID,pDeviceManufacturer,pcchDeviceManufacturer)) 
#define IPortableDeviceManager_GetDeviceProperty(This,pszPnPDeviceID,pszDevicePropertyName,pData,pcbData,pdwType)  ((This)->lpVtbl->GetDeviceProperty(This,pszPnPDeviceID,pszDevicePropertyName,pData,pcbData,pdwType)) 
#define IPortableDeviceManager_GetPrivateDevices(This,pPnPDeviceIDs,pcPnPDeviceIDs)  ((This)->lpVtbl->GetPrivateDevices(This,pPnPDeviceIDs,pcPnPDeviceIDs)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceManager_INTERFACE_DEFINED__ */


#ifndef __IPortableDevice_INTERFACE_DEFINED__
#define __IPortableDevice_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("625e2df8-6392-4cf0-9ad1-3cfa5f17775c") IPortableDevice:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Open(LPCWSTR pszPnPDeviceID, IPortableDeviceValues * pClientInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendCommand(const DWORD dwFlags, IPortableDeviceValues *pParameters, IPortableDeviceValues **ppResults) = 0;
    virtual HRESULT STDMETHODCALLTYPE Content(IPortableDeviceContent **ppContent) = 0;
    virtual HRESULT STDMETHODCALLTYPE Capabilities(IPortableDeviceCapabilities **ppCapabilities) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Advise(const DWORD dwFlags, IPortableDeviceEventCallback *pCallback, IPortableDeviceValues *pParameters, LPWSTR *ppszCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unadvise(LPCWSTR pszCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPnPDeviceID(LPWSTR *ppszPnPDeviceID) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDevice * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDevice * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDevice * This);
    HRESULT (STDMETHODCALLTYPE *Open) (IPortableDevice * This, LPCWSTR pszPnPDeviceID, IPortableDeviceValues * pClientInfo);
    HRESULT (STDMETHODCALLTYPE *SendCommand) (IPortableDevice * This, const DWORD dwFlags, IPortableDeviceValues * pParameters, IPortableDeviceValues ** ppResults);
    HRESULT (STDMETHODCALLTYPE *Content) (IPortableDevice * This, IPortableDeviceContent ** ppContent);
    HRESULT (STDMETHODCALLTYPE *Capabilities) (IPortableDevice * This, IPortableDeviceCapabilities ** ppCapabilities);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDevice * This);
    HRESULT (STDMETHODCALLTYPE *Close) (IPortableDevice * This);
    HRESULT (STDMETHODCALLTYPE *Advise) (IPortableDevice * This, const DWORD dwFlags, IPortableDeviceEventCallback * pCallback, IPortableDeviceValues * pParameters, LPWSTR * ppszCookie);
    HRESULT (STDMETHODCALLTYPE *Unadvise) (IPortableDevice * This, LPCWSTR pszCookie);
    HRESULT (STDMETHODCALLTYPE *GetPnPDeviceID) (IPortableDevice * This, LPWSTR * ppszPnPDeviceID);
    END_INTERFACE
} IPortableDeviceVtbl;

interface IPortableDevice {
    CONST_VTBL struct IPortableDeviceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDevice_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDevice_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDevice_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDevice_Open(This,pszPnPDeviceID,pClientInfo)  ((This)->lpVtbl->Open(This,pszPnPDeviceID,pClientInfo)) 
#define IPortableDevice_SendCommand(This,dwFlags,pParameters,ppResults)  ((This)->lpVtbl->SendCommand(This,dwFlags,pParameters,ppResults)) 
#define IPortableDevice_Content(This,ppContent)  ((This)->lpVtbl->Content(This,ppContent)) 
#define IPortableDevice_Capabilities(This,ppCapabilities)  ((This)->lpVtbl->Capabilities(This,ppCapabilities)) 
#define IPortableDevice_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IPortableDevice_Close(This)  ((This)->lpVtbl->Close(This)) 
#define IPortableDevice_Advise(This,dwFlags,pCallback,pParameters,ppszCookie)  ((This)->lpVtbl->Advise(This,dwFlags,pCallback,pParameters,ppszCookie)) 
#define IPortableDevice_Unadvise(This,pszCookie)  ((This)->lpVtbl->Unadvise(This,pszCookie)) 
#define IPortableDevice_GetPnPDeviceID(This,ppszPnPDeviceID)  ((This)->lpVtbl->GetPnPDeviceID(This,ppszPnPDeviceID)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDevice_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceContent_INTERFACE_DEFINED__
#define __IPortableDeviceContent_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceContent;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6a96ed84-7c73-4480-9938-bf5af477d426")IPortableDeviceContent:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE EnumObjects(const DWORD dwFlags, LPCWSTR pszParentObjectID, IPortableDeviceValues *pFilter, IEnumPortableDeviceObjectIDs **ppEnum) = 0;
    virtual HRESULT STDMETHODCALLTYPE Properties(IPortableDeviceProperties **ppProperties) = 0;
    virtual HRESULT STDMETHODCALLTYPE Transfer(IPortableDeviceResources **ppResources) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateObjectWithPropertiesOnly(IPortableDeviceValues *pValues, LPWSTR *ppszObjectID) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateObjectWithPropertiesAndData(IPortableDeviceValues *pValues, IStream **ppData, DWORD *pdwOptimalWriteBufferSize, LPWSTR *ppszCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE Delete(const DWORD dwOptions, IPortableDevicePropVariantCollection *pObjectIDs, IPortableDevicePropVariantCollection **ppResults) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetObjectIDsFromPersistentUniqueIDs(IPortableDevicePropVariantCollection *pPersistentUniqueIDs, IPortableDevicePropVariantCollection **ppObjectIDs) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Move(IPortableDevicePropVariantCollection *pObjectIDs, LPCWSTR pszDestinationFolderObjectID, IPortableDevicePropVariantCollection **ppResults) = 0;
    virtual HRESULT STDMETHODCALLTYPE Copy(IPortableDevicePropVariantCollection *pObjectIDs, LPCWSTR pszDestinationFolderObjectID, IPortableDevicePropVariantCollection **ppResults) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceContentVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceContent * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceContent * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceContent * This);
    HRESULT (STDMETHODCALLTYPE *EnumObjects) (IPortableDeviceContent * This, const DWORD dwFlags, LPCWSTR pszParentObjectID, IPortableDeviceValues * pFilter, IEnumPortableDeviceObjectIDs ** ppEnum);
    HRESULT (STDMETHODCALLTYPE *Properties) (IPortableDeviceContent * This, IPortableDeviceProperties ** ppProperties);
    HRESULT (STDMETHODCALLTYPE *Transfer) (IPortableDeviceContent * This, IPortableDeviceResources ** ppResources);
    HRESULT (STDMETHODCALLTYPE *CreateObjectWithPropertiesOnly) (IPortableDeviceContent * This, IPortableDeviceValues * pValues, LPWSTR * ppszObjectID);
    HRESULT (STDMETHODCALLTYPE *CreateObjectWithPropertiesAndData) (IPortableDeviceContent * This, IPortableDeviceValues * pValues, IStream ** ppData, DWORD * pdwOptimalWriteBufferSize, LPWSTR * ppszCookie);
    HRESULT (STDMETHODCALLTYPE *Delete) (IPortableDeviceContent * This, const DWORD dwOptions, IPortableDevicePropVariantCollection * pObjectIDs, IPortableDevicePropVariantCollection ** ppResults);
    HRESULT (STDMETHODCALLTYPE *GetObjectIDsFromPersistentUniqueIDs) (IPortableDeviceContent * This, IPortableDevicePropVariantCollection * pPersistentUniqueIDs, IPortableDevicePropVariantCollection ** ppObjectIDs);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceContent * This);
    HRESULT (STDMETHODCALLTYPE *Move) (IPortableDeviceContent * This, IPortableDevicePropVariantCollection * pObjectIDs, LPCWSTR pszDestinationFolderObjectID, IPortableDevicePropVariantCollection ** ppResults);
    HRESULT (STDMETHODCALLTYPE *Copy) (IPortableDeviceContent * This, IPortableDevicePropVariantCollection * pObjectIDs, LPCWSTR pszDestinationFolderObjectID, IPortableDevicePropVariantCollection ** ppResults);
    END_INTERFACE
} IPortableDeviceContentVtbl;

interface IPortableDeviceContent {
    CONST_VTBL struct IPortableDeviceContentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceContent_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceContent_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceContent_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceContent_EnumObjects(This,dwFlags,pszParentObjectID,pFilter,ppEnum)  ((This)->lpVtbl->EnumObjects(This,dwFlags,pszParentObjectID,pFilter,ppEnum)) 
#define IPortableDeviceContent_Properties(This,ppProperties)  ((This)->lpVtbl->Properties(This,ppProperties)) 
#define IPortableDeviceContent_Transfer(This,ppResources)  ((This)->lpVtbl->Transfer(This,ppResources)) 
#define IPortableDeviceContent_CreateObjectWithPropertiesOnly(This,pValues,ppszObjectID)  ((This)->lpVtbl->CreateObjectWithPropertiesOnly(This,pValues,ppszObjectID)) 
#define IPortableDeviceContent_CreateObjectWithPropertiesAndData(This,pValues,ppData,pdwOptimalWriteBufferSize,ppszCookie)  ((This)->lpVtbl->CreateObjectWithPropertiesAndData(This,pValues,ppData,pdwOptimalWriteBufferSize,ppszCookie)) 
#define IPortableDeviceContent_Delete(This,dwOptions,pObjectIDs,ppResults)  ((This)->lpVtbl->Delete(This,dwOptions,pObjectIDs,ppResults)) 
#define IPortableDeviceContent_GetObjectIDsFromPersistentUniqueIDs(This,pPersistentUniqueIDs,ppObjectIDs)  ((This)->lpVtbl->GetObjectIDsFromPersistentUniqueIDs(This,pPersistentUniqueIDs,ppObjectIDs)) 
#define IPortableDeviceContent_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IPortableDeviceContent_Move(This,pObjectIDs,pszDestinationFolderObjectID,ppResults)  ((This)->lpVtbl->Move(This,pObjectIDs,pszDestinationFolderObjectID,ppResults)) 
#define IPortableDeviceContent_Copy(This,pObjectIDs,pszDestinationFolderObjectID,ppResults)  ((This)->lpVtbl->Copy(This,pObjectIDs,pszDestinationFolderObjectID,ppResults)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceContent_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceContent2_INTERFACE_DEFINED__
#define __IPortableDeviceContent2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceContent2;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("9b4add96-f6bf-4034-8708-eca72bf10554") IPortableDeviceContent2:public IPortableDeviceContent
{
    public:
    virtual HRESULT STDMETHODCALLTYPE UpdateObjectWithPropertiesAndData(LPCWSTR pszObjectID, IPortableDeviceValues * pProperties, IStream ** ppData, DWORD * pdwOptimalWriteBufferSize) = 0;
};
#else 	/* C style interface */

typedef struct IPortableDeviceContent2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceContent2 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceContent2 * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceContent2 * This);
    HRESULT (STDMETHODCALLTYPE *EnumObjects) (IPortableDeviceContent2 * This, const DWORD dwFlags, LPCWSTR pszParentObjectID, IPortableDeviceValues * pFilter, IEnumPortableDeviceObjectIDs ** ppEnum);
    HRESULT (STDMETHODCALLTYPE *Properties) (IPortableDeviceContent2 * This, IPortableDeviceProperties ** ppProperties);
    HRESULT (STDMETHODCALLTYPE *Transfer) (IPortableDeviceContent2 * This, IPortableDeviceResources ** ppResources);
    HRESULT (STDMETHODCALLTYPE *CreateObjectWithPropertiesOnly) (IPortableDeviceContent2 * This, IPortableDeviceValues * pValues, LPWSTR * ppszObjectID);
    HRESULT (STDMETHODCALLTYPE *CreateObjectWithPropertiesAndData) (IPortableDeviceContent2 * This, IPortableDeviceValues * pValues, IStream ** ppData, DWORD * pdwOptimalWriteBufferSize, LPWSTR * ppszCookie);
    HRESULT (STDMETHODCALLTYPE *Delete) (IPortableDeviceContent2 * This, const DWORD dwOptions, IPortableDevicePropVariantCollection * pObjectIDs, IPortableDevicePropVariantCollection ** ppResults);
    HRESULT (STDMETHODCALLTYPE *GetObjectIDsFromPersistentUniqueIDs) (IPortableDeviceContent2 * This, IPortableDevicePropVariantCollection * pPersistentUniqueIDs, IPortableDevicePropVariantCollection ** ppObjectIDs);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceContent2 * This);
    HRESULT (STDMETHODCALLTYPE *Move) (IPortableDeviceContent2 * This, IPortableDevicePropVariantCollection * pObjectIDs, LPCWSTR pszDestinationFolderObjectID, IPortableDevicePropVariantCollection ** ppResults);
    HRESULT (STDMETHODCALLTYPE *Copy) (IPortableDeviceContent2 * This, IPortableDevicePropVariantCollection * pObjectIDs, LPCWSTR pszDestinationFolderObjectID, IPortableDevicePropVariantCollection ** ppResults);
    HRESULT (STDMETHODCALLTYPE *UpdateObjectWithPropertiesAndData) (IPortableDeviceContent2 * This, LPCWSTR pszObjectID, IPortableDeviceValues * pProperties, IStream ** ppData, DWORD * pdwOptimalWriteBufferSize);
    END_INTERFACE
} IPortableDeviceContent2Vtbl;

interface IPortableDeviceContent2 {
    CONST_VTBL struct IPortableDeviceContent2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceContent2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceContent2_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceContent2_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceContent2_EnumObjects(This,dwFlags,pszParentObjectID,pFilter,ppEnum)  ((This)->lpVtbl->EnumObjects(This,dwFlags,pszParentObjectID,pFilter,ppEnum)) 
#define IPortableDeviceContent2_Properties(This,ppProperties)  ((This)->lpVtbl->Properties(This,ppProperties)) 
#define IPortableDeviceContent2_Transfer(This,ppResources)  ((This)->lpVtbl->Transfer(This,ppResources)) 
#define IPortableDeviceContent2_CreateObjectWithPropertiesOnly(This,pValues,ppszObjectID)  ((This)->lpVtbl->CreateObjectWithPropertiesOnly(This,pValues,ppszObjectID)) 
#define IPortableDeviceContent2_CreateObjectWithPropertiesAndData(This,pValues,ppData,pdwOptimalWriteBufferSize,ppszCookie)  ((This)->lpVtbl->CreateObjectWithPropertiesAndData(This,pValues,ppData,pdwOptimalWriteBufferSize,ppszCookie)) 
#define IPortableDeviceContent2_Delete(This,dwOptions,pObjectIDs,ppResults)  ((This)->lpVtbl->Delete(This,dwOptions,pObjectIDs,ppResults)) 
#define IPortableDeviceContent2_GetObjectIDsFromPersistentUniqueIDs(This,pPersistentUniqueIDs,ppObjectIDs)  ((This)->lpVtbl->GetObjectIDsFromPersistentUniqueIDs(This,pPersistentUniqueIDs,ppObjectIDs)) 
#define IPortableDeviceContent2_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IPortableDeviceContent2_Move(This,pObjectIDs,pszDestinationFolderObjectID,ppResults)  ((This)->lpVtbl->Move(This,pObjectIDs,pszDestinationFolderObjectID,ppResults)) 
#define IPortableDeviceContent2_Copy(This,pObjectIDs,pszDestinationFolderObjectID,ppResults)  ((This)->lpVtbl->Copy(This,pObjectIDs,pszDestinationFolderObjectID,ppResults)) 
#define IPortableDeviceContent2_UpdateObjectWithPropertiesAndData(This,pszObjectID,pProperties,ppData,pdwOptimalWriteBufferSize)  ((This)->lpVtbl->UpdateObjectWithPropertiesAndData(This,pszObjectID,pProperties,ppData,pdwOptimalWriteBufferSize)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceContent2_INTERFACE_DEFINED__ */


#ifndef __IEnumPortableDeviceObjectIDs_INTERFACE_DEFINED__
#define __IEnumPortableDeviceObjectIDs_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumPortableDeviceObjectIDs;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("10ece955-cf41-4728-bfa0-41eedf1bbf19") IEnumPortableDeviceObjectIDs:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG cObjects, LPWSTR * pObjIDs, ULONG * pcFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG cObjects) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumPortableDeviceObjectIDs **ppEnum) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
};
#else /* C style interface */

typedef struct IEnumPortableDeviceObjectIDsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IEnumPortableDeviceObjectIDs * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IEnumPortableDeviceObjectIDs * This);
    ULONG (STDMETHODCALLTYPE *Release) (IEnumPortableDeviceObjectIDs * This);
    HRESULT (STDMETHODCALLTYPE *Next) (IEnumPortableDeviceObjectIDs * This, ULONG cObjects, LPWSTR * pObjIDs, ULONG * pcFetched);
    HRESULT (STDMETHODCALLTYPE *Skip) (IEnumPortableDeviceObjectIDs * This, ULONG cObjects);
    HRESULT (STDMETHODCALLTYPE *Reset) (IEnumPortableDeviceObjectIDs * This);
    HRESULT (STDMETHODCALLTYPE *Clone) (IEnumPortableDeviceObjectIDs * This, IEnumPortableDeviceObjectIDs ** ppEnum);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IEnumPortableDeviceObjectIDs * This);
    END_INTERFACE
} IEnumPortableDeviceObjectIDsVtbl;

interface IEnumPortableDeviceObjectIDs {
    CONST_VTBL struct IEnumPortableDeviceObjectIDsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumPortableDeviceObjectIDs_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IEnumPortableDeviceObjectIDs_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IEnumPortableDeviceObjectIDs_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IEnumPortableDeviceObjectIDs_Next(This,cObjects,pObjIDs,pcFetched)  ((This)->lpVtbl->Next(This,cObjects,pObjIDs,pcFetched)) 
#define IEnumPortableDeviceObjectIDs_Skip(This,cObjects)  ((This)->lpVtbl->Skip(This,cObjects)) 
#define IEnumPortableDeviceObjectIDs_Reset(This)  ((This)->lpVtbl->Reset(This)) 
#define IEnumPortableDeviceObjectIDs_Clone(This,ppEnum)  ((This)->lpVtbl->Clone(This,ppEnum)) 
#define IEnumPortableDeviceObjectIDs_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IEnumPortableDeviceObjectIDs_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceProperties_INTERFACE_DEFINED__
#define __IPortableDeviceProperties_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("7f6d695c-03df-4439-a809-59266beee3a6") IPortableDeviceProperties:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSupportedProperties(LPCWSTR pszObjectID, IPortableDeviceKeyCollection ** ppKeys) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyAttributes(LPCWSTR pszObjectID, REFPROPERTYKEY Key, IPortableDeviceValues **ppAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValues(LPCWSTR pszObjectID, IPortableDeviceKeyCollection *pKeys, IPortableDeviceValues **ppValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetValues(LPCWSTR pszObjectID, IPortableDeviceValues *pValues, IPortableDeviceValues **ppResults) = 0;
    virtual HRESULT STDMETHODCALLTYPE Delete(LPCWSTR pszObjectID, IPortableDeviceKeyCollection *pKeys) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
};
#else /* C style interface */

typedef struct IPortableDevicePropertiesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceProperties * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceProperties * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceProperties * This);
    HRESULT (STDMETHODCALLTYPE *GetSupportedProperties) (IPortableDeviceProperties * This, LPCWSTR pszObjectID, IPortableDeviceKeyCollection ** ppKeys);
    HRESULT (STDMETHODCALLTYPE *GetPropertyAttributes) (IPortableDeviceProperties * This, LPCWSTR pszObjectID, REFPROPERTYKEY Key, IPortableDeviceValues ** ppAttributes);
    HRESULT (STDMETHODCALLTYPE *GetValues) (IPortableDeviceProperties * This, LPCWSTR pszObjectID, IPortableDeviceKeyCollection * pKeys, IPortableDeviceValues ** ppValues);
    HRESULT (STDMETHODCALLTYPE *SetValues) (IPortableDeviceProperties * This, LPCWSTR pszObjectID, IPortableDeviceValues * pValues, IPortableDeviceValues ** ppResults);
    HRESULT (STDMETHODCALLTYPE *Delete) (IPortableDeviceProperties * This, LPCWSTR pszObjectID, IPortableDeviceKeyCollection * pKeys);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceProperties * This);
    END_INTERFACE
} IPortableDevicePropertiesVtbl;

interface IPortableDeviceProperties {
    CONST_VTBL struct IPortableDevicePropertiesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceProperties_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceProperties_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceProperties_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceProperties_GetSupportedProperties(This,pszObjectID,ppKeys)  ((This)->lpVtbl->GetSupportedProperties(This,pszObjectID,ppKeys)) 
#define IPortableDeviceProperties_GetPropertyAttributes(This,pszObjectID,Key,ppAttributes)  ((This)->lpVtbl->GetPropertyAttributes(This,pszObjectID,Key,ppAttributes)) 
#define IPortableDeviceProperties_GetValues(This,pszObjectID,pKeys,ppValues)  ((This)->lpVtbl->GetValues(This,pszObjectID,pKeys,ppValues)) 
#define IPortableDeviceProperties_SetValues(This,pszObjectID,pValues,ppResults)  ((This)->lpVtbl->SetValues(This,pszObjectID,pValues,ppResults)) 
#define IPortableDeviceProperties_Delete(This,pszObjectID,pKeys)  ((This)->lpVtbl->Delete(This,pszObjectID,pKeys)) 
#define IPortableDeviceProperties_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceProperties_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceResources_INTERFACE_DEFINED__
#define __IPortableDeviceResources_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceResources;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("fd8878ac-d841-4d17-891c-e6829cdb6934")IPortableDeviceResources:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSupportedResources(LPCWSTR pszObjectID, IPortableDeviceKeyCollection ** ppKeys) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResourceAttributes(LPCWSTR pszObjectID, REFPROPERTYKEY Key, IPortableDeviceValues **ppResourceAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStream(LPCWSTR pszObjectID, REFPROPERTYKEY Key, const DWORD dwMode, DWORD *pdwOptimalBufferSize, IStream **ppStream) = 0;
    virtual HRESULT STDMETHODCALLTYPE Delete(LPCWSTR pszObjectID, IPortableDeviceKeyCollection *pKeys) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateResource(IPortableDeviceValues *pResourceAttributes, IStream **ppData, DWORD *pdwOptimalWriteBufferSize, LPWSTR *ppszCookie) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceResourcesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceResources * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceResources * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceResources * This);
    HRESULT (STDMETHODCALLTYPE *GetSupportedResources) (IPortableDeviceResources * This, LPCWSTR pszObjectID, IPortableDeviceKeyCollection ** ppKeys);
    HRESULT (STDMETHODCALLTYPE *GetResourceAttributes) (IPortableDeviceResources * This, LPCWSTR pszObjectID, REFPROPERTYKEY Key, IPortableDeviceValues ** ppResourceAttributes);
    HRESULT (STDMETHODCALLTYPE *GetStream) (IPortableDeviceResources * This, LPCWSTR pszObjectID, REFPROPERTYKEY Key, const DWORD dwMode, DWORD * pdwOptimalBufferSize, IStream ** ppStream);
    HRESULT (STDMETHODCALLTYPE *Delete) (IPortableDeviceResources * This, LPCWSTR pszObjectID, IPortableDeviceKeyCollection * pKeys);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceResources * This);
    HRESULT (STDMETHODCALLTYPE *CreateResource) (IPortableDeviceResources * This, IPortableDeviceValues * pResourceAttributes, IStream ** ppData, DWORD * pdwOptimalWriteBufferSize, LPWSTR * ppszCookie);
    END_INTERFACE
} IPortableDeviceResourcesVtbl;

interface IPortableDeviceResources {
    CONST_VTBL struct IPortableDeviceResourcesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceResources_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceResources_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceResources_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceResources_GetSupportedResources(This,pszObjectID,ppKeys)  ((This)->lpVtbl->GetSupportedResources(This,pszObjectID,ppKeys)) 
#define IPortableDeviceResources_GetResourceAttributes(This,pszObjectID,Key,ppResourceAttributes)  ((This)->lpVtbl->GetResourceAttributes(This,pszObjectID,Key,ppResourceAttributes)) 
#define IPortableDeviceResources_GetStream(This,pszObjectID,Key,dwMode,pdwOptimalBufferSize,ppStream)  ((This)->lpVtbl->GetStream(This,pszObjectID,Key,dwMode,pdwOptimalBufferSize,ppStream)) 
#define IPortableDeviceResources_Delete(This,pszObjectID,pKeys)  ((This)->lpVtbl->Delete(This,pszObjectID,pKeys)) 
#define IPortableDeviceResources_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IPortableDeviceResources_CreateResource(This,pResourceAttributes,ppData,pdwOptimalWriteBufferSize,ppszCookie)  ((This)->lpVtbl->CreateResource(This,pResourceAttributes,ppData,pdwOptimalWriteBufferSize,ppszCookie)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceResources_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceCapabilities_INTERFACE_DEFINED__
#define __IPortableDeviceCapabilities_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceCapabilities;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("2c8c6dbf-e3dc-4061-becc-8542e810d126")IPortableDeviceCapabilities:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSupportedCommands(IPortableDeviceKeyCollection ** ppCommands) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCommandOptions(REFPROPERTYKEY Command, IPortableDeviceValues **ppOptions) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFunctionalCategories(IPortableDevicePropVariantCollection **ppCategories) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFunctionalObjects(REFGUID Category, IPortableDevicePropVariantCollection **ppObjectIDs) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedContentTypes(REFGUID Category, IPortableDevicePropVariantCollection **ppContentTypes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedFormats(REFGUID ContentType, IPortableDevicePropVariantCollection **ppFormats) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedFormatProperties(REFGUID Format, IPortableDeviceKeyCollection **ppKeys) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFixedPropertyAttributes(REFGUID Format, REFPROPERTYKEY Key, IPortableDeviceValues **ppAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedEvents(IPortableDevicePropVariantCollection **ppEvents) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEventOptions(REFGUID Event, IPortableDeviceValues **ppOptions) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceCapabilitiesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceCapabilities * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceCapabilities * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceCapabilities * This);
    HRESULT (STDMETHODCALLTYPE *GetSupportedCommands) (IPortableDeviceCapabilities * This, IPortableDeviceKeyCollection ** ppCommands);
    HRESULT (STDMETHODCALLTYPE *GetCommandOptions) (IPortableDeviceCapabilities * This, REFPROPERTYKEY Command, IPortableDeviceValues ** ppOptions);
    HRESULT (STDMETHODCALLTYPE *GetFunctionalCategories) (IPortableDeviceCapabilities * This, IPortableDevicePropVariantCollection ** ppCategories);
    HRESULT (STDMETHODCALLTYPE *GetFunctionalObjects) (IPortableDeviceCapabilities * This, REFGUID Category, IPortableDevicePropVariantCollection ** ppObjectIDs);
    HRESULT (STDMETHODCALLTYPE *GetSupportedContentTypes) (IPortableDeviceCapabilities * This, REFGUID Category, IPortableDevicePropVariantCollection ** ppContentTypes);
    HRESULT (STDMETHODCALLTYPE *GetSupportedFormats) (IPortableDeviceCapabilities * This, REFGUID ContentType, IPortableDevicePropVariantCollection ** ppFormats);
    HRESULT (STDMETHODCALLTYPE *GetSupportedFormatProperties) (IPortableDeviceCapabilities * This, REFGUID Format, IPortableDeviceKeyCollection ** ppKeys);
    HRESULT (STDMETHODCALLTYPE *GetFixedPropertyAttributes) (IPortableDeviceCapabilities * This, REFGUID Format, REFPROPERTYKEY Key, IPortableDeviceValues ** ppAttributes);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceCapabilities * This);
    HRESULT (STDMETHODCALLTYPE *GetSupportedEvents) (IPortableDeviceCapabilities * This, IPortableDevicePropVariantCollection ** ppEvents);
    HRESULT (STDMETHODCALLTYPE *GetEventOptions) (IPortableDeviceCapabilities * This, REFGUID Event, IPortableDeviceValues ** ppOptions);
    END_INTERFACE
} IPortableDeviceCapabilitiesVtbl;

interface IPortableDeviceCapabilities {
    CONST_VTBL struct IPortableDeviceCapabilitiesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceCapabilities_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceCapabilities_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceCapabilities_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceCapabilities_GetSupportedCommands(This,ppCommands)  ((This)->lpVtbl->GetSupportedCommands(This,ppCommands)) 
#define IPortableDeviceCapabilities_GetCommandOptions(This,Command,ppOptions)  ((This)->lpVtbl->GetCommandOptions(This,Command,ppOptions)) 
#define IPortableDeviceCapabilities_GetFunctionalCategories(This,ppCategories)  ((This)->lpVtbl->GetFunctionalCategories(This,ppCategories)) 
#define IPortableDeviceCapabilities_GetFunctionalObjects(This,Category,ppObjectIDs)  ((This)->lpVtbl->GetFunctionalObjects(This,Category,ppObjectIDs)) 
#define IPortableDeviceCapabilities_GetSupportedContentTypes(This,Category,ppContentTypes)  ((This)->lpVtbl->GetSupportedContentTypes(This,Category,ppContentTypes)) 
#define IPortableDeviceCapabilities_GetSupportedFormats(This,ContentType,ppFormats)  ((This)->lpVtbl->GetSupportedFormats(This,ContentType,ppFormats)) 
#define IPortableDeviceCapabilities_GetSupportedFormatProperties(This,Format,ppKeys)  ((This)->lpVtbl->GetSupportedFormatProperties(This,Format,ppKeys)) 
#define IPortableDeviceCapabilities_GetFixedPropertyAttributes(This,Format,Key,ppAttributes)  ((This)->lpVtbl->GetFixedPropertyAttributes(This,Format,Key,ppAttributes)) 
#define IPortableDeviceCapabilities_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IPortableDeviceCapabilities_GetSupportedEvents(This,ppEvents)  ((This)->lpVtbl->GetSupportedEvents(This,ppEvents)) 
#define IPortableDeviceCapabilities_GetEventOptions(This,Event,ppOptions)  ((This)->lpVtbl->GetEventOptions(This,Event,ppOptions)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceCapabilities_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceEventCallback_INTERFACE_DEFINED__
#define __IPortableDeviceEventCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceEventCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("a8792a31-f385-493c-a893-40f64eb45f6e")IPortableDeviceEventCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnEvent(IPortableDeviceValues * pEventParameters) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceEventCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceEventCallback * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceEventCallback * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceEventCallback * This);
    HRESULT (STDMETHODCALLTYPE *OnEvent) (IPortableDeviceEventCallback * This, IPortableDeviceValues * pEventParameters);
    END_INTERFACE
} IPortableDeviceEventCallbackVtbl;

interface IPortableDeviceEventCallback {
    CONST_VTBL struct IPortableDeviceEventCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceEventCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceEventCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceEventCallback_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceEventCallback_OnEvent(This,pEventParameters)  ((This)->lpVtbl->OnEvent(This,pEventParameters)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceEventCallback_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceDataStream_INTERFACE_DEFINED__
#define __IPortableDeviceDataStream_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceDataStream;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("88e04db3-1012-4d64-9996-f703a950d3f4")IPortableDeviceDataStream:public IStream
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetObjectID(LPWSTR * ppszObjectID) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceDataStreamVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceDataStream * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceDataStream * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceDataStream * This);
    HRESULT (STDMETHODCALLTYPE *Read) (IPortableDeviceDataStream * This, void *pv, ULONG cb, ULONG * pcbRead);
    HRESULT (STDMETHODCALLTYPE *Write) (IPortableDeviceDataStream * This, const void *pv, ULONG cb, ULONG * pcbWritten);
    HRESULT (STDMETHODCALLTYPE *Seek) (IPortableDeviceDataStream * This, LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * plibNewPosition);
    HRESULT (STDMETHODCALLTYPE *SetSize) (IPortableDeviceDataStream * This, ULARGE_INTEGER libNewSize);
    HRESULT (STDMETHODCALLTYPE *CopyTo) (IPortableDeviceDataStream * This, IStream * pstm, ULARGE_INTEGER cb, ULARGE_INTEGER * pcbRead, ULARGE_INTEGER * pcbWritten);
    HRESULT (STDMETHODCALLTYPE *Commit) (IPortableDeviceDataStream * This, DWORD grfCommitFlags);
    HRESULT (STDMETHODCALLTYPE *Revert) (IPortableDeviceDataStream * This);
    HRESULT (STDMETHODCALLTYPE *LockRegion) (IPortableDeviceDataStream * This, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    HRESULT (STDMETHODCALLTYPE *UnlockRegion) (IPortableDeviceDataStream * This, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    HRESULT (STDMETHODCALLTYPE *Stat) (IPortableDeviceDataStream * This, STATSTG * pstatstg, DWORD grfStatFlag);
    HRESULT (STDMETHODCALLTYPE *Clone) (IPortableDeviceDataStream * This, IStream ** ppstm);
    HRESULT (STDMETHODCALLTYPE *GetObjectID) (IPortableDeviceDataStream * This, LPWSTR * ppszObjectID);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceDataStream * This);
    END_INTERFACE
} IPortableDeviceDataStreamVtbl;

interface IPortableDeviceDataStream {
    CONST_VTBL struct IPortableDeviceDataStreamVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceDataStream_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceDataStream_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceDataStream_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceDataStream_Read(This,pv,cb,pcbRead)  ((This)->lpVtbl->Read(This,pv,cb,pcbRead)) 
#define IPortableDeviceDataStream_Write(This,pv,cb,pcbWritten)  ((This)->lpVtbl->Write(This,pv,cb,pcbWritten)) 
#define IPortableDeviceDataStream_Seek(This,dlibMove,dwOrigin,plibNewPosition)  ((This)->lpVtbl->Seek(This,dlibMove,dwOrigin,plibNewPosition)) 
#define IPortableDeviceDataStream_SetSize(This,libNewSize)  ((This)->lpVtbl->SetSize(This,libNewSize)) 
#define IPortableDeviceDataStream_CopyTo(This,pstm,cb,pcbRead,pcbWritten)  ((This)->lpVtbl->CopyTo(This,pstm,cb,pcbRead,pcbWritten)) 
#define IPortableDeviceDataStream_Commit(This,grfCommitFlags)  ((This)->lpVtbl->Commit(This,grfCommitFlags)) 
#define IPortableDeviceDataStream_Revert(This)  ((This)->lpVtbl->Revert(This)) 
#define IPortableDeviceDataStream_LockRegion(This,libOffset,cb,dwLockType)  ((This)->lpVtbl->LockRegion(This,libOffset,cb,dwLockType)) 
#define IPortableDeviceDataStream_UnlockRegion(This,libOffset,cb,dwLockType)  ((This)->lpVtbl->UnlockRegion(This,libOffset,cb,dwLockType)) 
#define IPortableDeviceDataStream_Stat(This,pstatstg,grfStatFlag)  ((This)->lpVtbl->Stat(This,pstatstg,grfStatFlag)) 
#define IPortableDeviceDataStream_Clone(This,ppstm)  ((This)->lpVtbl->Clone(This,ppstm)) 
#define IPortableDeviceDataStream_GetObjectID(This,ppszObjectID)  ((This)->lpVtbl->GetObjectID(This,ppszObjectID)) 
#define IPortableDeviceDataStream_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceDataStream_INTERFACE_DEFINED__ */


#ifndef __IPortableDevicePropertiesBulk_INTERFACE_DEFINED__
#define __IPortableDevicePropertiesBulk_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDevicePropertiesBulk;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("482b05c0-4056-44ed-9e0f-5e23b009da93")IPortableDevicePropertiesBulk:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE QueueGetValuesByObjectList(IPortableDevicePropVariantCollection * pObjectIDs, IPortableDeviceKeyCollection * pKeys, IPortableDevicePropertiesBulkCallback * pCallback, GUID * pContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueueGetValuesByObjectFormat(REFGUID pguidObjectFormat, LPCWSTR pszParentObjectID, const DWORD dwDepth, IPortableDeviceKeyCollection *pKeys, IPortableDevicePropertiesBulkCallback *pCallback, GUID *pContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueueSetValuesByObjectList(IPortableDeviceValuesCollection *pObjectValues, IPortableDevicePropertiesBulkCallback *pCallback, GUID *pContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE Start(REFGUID pContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(REFGUID pContext) = 0;
};
#else /* C style interface */

typedef struct IPortableDevicePropertiesBulkVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDevicePropertiesBulk * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDevicePropertiesBulk * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDevicePropertiesBulk * This);
    HRESULT (STDMETHODCALLTYPE *QueueGetValuesByObjectList) (IPortableDevicePropertiesBulk * This, IPortableDevicePropVariantCollection * pObjectIDs, IPortableDeviceKeyCollection * pKeys, IPortableDevicePropertiesBulkCallback * pCallback, GUID * pContext);
    HRESULT (STDMETHODCALLTYPE *QueueGetValuesByObjectFormat) (IPortableDevicePropertiesBulk * This, REFGUID pguidObjectFormat, LPCWSTR pszParentObjectID, const DWORD dwDepth, IPortableDeviceKeyCollection * pKeys, IPortableDevicePropertiesBulkCallback * pCallback, GUID * pContext);
    HRESULT (STDMETHODCALLTYPE *QueueSetValuesByObjectList) (IPortableDevicePropertiesBulk * This, IPortableDeviceValuesCollection * pObjectValues, IPortableDevicePropertiesBulkCallback * pCallback, GUID * pContext);
    HRESULT (STDMETHODCALLTYPE *Start) (IPortableDevicePropertiesBulk * This, REFGUID pContext);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDevicePropertiesBulk * This, REFGUID pContext);
    END_INTERFACE
} IPortableDevicePropertiesBulkVtbl;

interface IPortableDevicePropertiesBulk {
    CONST_VTBL struct IPortableDevicePropertiesBulkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDevicePropertiesBulk_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDevicePropertiesBulk_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDevicePropertiesBulk_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDevicePropertiesBulk_QueueGetValuesByObjectList(This,pObjectIDs,pKeys,pCallback,pContext)  ((This)->lpVtbl->QueueGetValuesByObjectList(This,pObjectIDs,pKeys,pCallback,pContext)) 
#define IPortableDevicePropertiesBulk_QueueGetValuesByObjectFormat(This,pguidObjectFormat,pszParentObjectID,dwDepth,pKeys,pCallback,pContext)  ((This)->lpVtbl->QueueGetValuesByObjectFormat(This,pguidObjectFormat,pszParentObjectID,dwDepth,pKeys,pCallback,pContext)) 
#define IPortableDevicePropertiesBulk_QueueSetValuesByObjectList(This,pObjectValues,pCallback,pContext)  ((This)->lpVtbl->QueueSetValuesByObjectList(This,pObjectValues,pCallback,pContext)) 
#define IPortableDevicePropertiesBulk_Start(This,pContext)  ((This)->lpVtbl->Start(This,pContext)) 
#define IPortableDevicePropertiesBulk_Cancel(This,pContext)  ((This)->lpVtbl->Cancel(This,pContext)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDevicePropertiesBulk_INTERFACE_DEFINED__ */


#ifndef __IPortableDevicePropertiesBulkCallback_INTERFACE_DEFINED__
#define __IPortableDevicePropertiesBulkCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDevicePropertiesBulkCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("9deacb80-11e8-40e3-a9f3-f557986a7845")IPortableDevicePropertiesBulkCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnStart(REFGUID pContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnProgress(REFGUID pContext, IPortableDeviceValuesCollection *pResults) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnEnd(REFGUID pContext, HRESULT hrStatus) = 0;
};
#else /* C style interface */

typedef struct IPortableDevicePropertiesBulkCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDevicePropertiesBulkCallback * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDevicePropertiesBulkCallback * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDevicePropertiesBulkCallback * This);
    HRESULT (STDMETHODCALLTYPE *OnStart) (IPortableDevicePropertiesBulkCallback * This, REFGUID pContext);
    HRESULT (STDMETHODCALLTYPE *OnProgress) (IPortableDevicePropertiesBulkCallback * This, REFGUID pContext, IPortableDeviceValuesCollection * pResults);
    HRESULT (STDMETHODCALLTYPE *OnEnd) (IPortableDevicePropertiesBulkCallback * This, REFGUID pContext, HRESULT hrStatus);
    END_INTERFACE
} IPortableDevicePropertiesBulkCallbackVtbl;

interface IPortableDevicePropertiesBulkCallback {
    CONST_VTBL struct IPortableDevicePropertiesBulkCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDevicePropertiesBulkCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDevicePropertiesBulkCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDevicePropertiesBulkCallback_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDevicePropertiesBulkCallback_OnStart(This,pContext)  ((This)->lpVtbl->OnStart(This,pContext)) 
#define IPortableDevicePropertiesBulkCallback_OnProgress(This,pContext,pResults)  ((This)->lpVtbl->OnProgress(This,pContext,pResults)) 
#define IPortableDevicePropertiesBulkCallback_OnEnd(This,pContext,hrStatus)  ((This)->lpVtbl->OnEnd(This,pContext,hrStatus)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDevicePropertiesBulkCallback_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceServiceManager_INTERFACE_DEFINED__
#define __IPortableDeviceServiceManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceServiceManager;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("a8abc4e9-a84a-47a9-80b3-c5d9b172a961")IPortableDeviceServiceManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetDeviceServices(LPCWSTR pszPnPDeviceID, REFGUID guidServiceCategory, LPWSTR * pServices, DWORD * pcServices) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceForService(LPCWSTR pszPnPServiceID, LPWSTR *ppszPnPDeviceID) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceServiceManagerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceServiceManager * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceServiceManager * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceServiceManager * This);
    HRESULT (STDMETHODCALLTYPE *GetDeviceServices) (IPortableDeviceServiceManager * This, LPCWSTR pszPnPDeviceID, REFGUID guidServiceCategory, LPWSTR * pServices, DWORD * pcServices);
    HRESULT (STDMETHODCALLTYPE *GetDeviceForService) (IPortableDeviceServiceManager * This, LPCWSTR pszPnPServiceID, LPWSTR * ppszPnPDeviceID);
    END_INTERFACE
} IPortableDeviceServiceManagerVtbl;

interface IPortableDeviceServiceManager {
    CONST_VTBL struct IPortableDeviceServiceManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceServiceManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceServiceManager_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceServiceManager_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceServiceManager_GetDeviceServices(This,pszPnPDeviceID,guidServiceCategory,pServices,pcServices)  ((This)->lpVtbl->GetDeviceServices(This,pszPnPDeviceID,guidServiceCategory,pServices,pcServices)) 
#define IPortableDeviceServiceManager_GetDeviceForService(This,pszPnPServiceID,ppszPnPDeviceID)  ((This)->lpVtbl->GetDeviceForService(This,pszPnPServiceID,ppszPnPDeviceID)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceServiceManager_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceService_INTERFACE_DEFINED__
#define __IPortableDeviceService_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceService;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("d3bd3a44-d7b5-40a9-98b7-2fa4d01dec08")IPortableDeviceService:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Open(LPCWSTR pszPnPServiceID, IPortableDeviceValues * pClientInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE Capabilities(IPortableDeviceServiceCapabilities **ppCapabilities) = 0;
    virtual HRESULT STDMETHODCALLTYPE Content(IPortableDeviceContent2 **ppContent) = 0;
    virtual HRESULT STDMETHODCALLTYPE Methods(IPortableDeviceServiceMethods **ppMethods) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetServiceObjectID(LPWSTR *ppszServiceObjectID) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPnPServiceID(LPWSTR *ppszPnPServiceID) = 0;
    virtual HRESULT STDMETHODCALLTYPE Advise(const DWORD dwFlags, IPortableDeviceEventCallback *pCallback, IPortableDeviceValues *pParameters, LPWSTR *ppszCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unadvise(LPCWSTR pszCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendCommand(const DWORD dwFlags, IPortableDeviceValues *pParameters, IPortableDeviceValues **ppResults) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceServiceVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceService * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceService * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceService * This);
    HRESULT (STDMETHODCALLTYPE *Open) (IPortableDeviceService * This, LPCWSTR pszPnPServiceID, IPortableDeviceValues * pClientInfo);
    HRESULT (STDMETHODCALLTYPE *Capabilities) (IPortableDeviceService * This, IPortableDeviceServiceCapabilities ** ppCapabilities);
    HRESULT (STDMETHODCALLTYPE *Content) (IPortableDeviceService * This, IPortableDeviceContent2 ** ppContent);
    HRESULT (STDMETHODCALLTYPE *Methods) (IPortableDeviceService * This, IPortableDeviceServiceMethods ** ppMethods);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceService * This);
    HRESULT (STDMETHODCALLTYPE *Close) (IPortableDeviceService * This);
    HRESULT (STDMETHODCALLTYPE *GetServiceObjectID) (IPortableDeviceService * This, LPWSTR * ppszServiceObjectID);
    HRESULT (STDMETHODCALLTYPE *GetPnPServiceID) (IPortableDeviceService * This, LPWSTR * ppszPnPServiceID);
    HRESULT (STDMETHODCALLTYPE *Advise) (IPortableDeviceService * This, const DWORD dwFlags, IPortableDeviceEventCallback * pCallback, IPortableDeviceValues * pParameters, LPWSTR * ppszCookie);
    HRESULT (STDMETHODCALLTYPE *Unadvise) (IPortableDeviceService * This, LPCWSTR pszCookie);
    HRESULT (STDMETHODCALLTYPE *SendCommand) (IPortableDeviceService * This, const DWORD dwFlags, IPortableDeviceValues * pParameters, IPortableDeviceValues ** ppResults);
    END_INTERFACE
} IPortableDeviceServiceVtbl;

interface IPortableDeviceService {
    CONST_VTBL struct IPortableDeviceServiceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceService_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceService_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceService_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceService_Open(This,pszPnPServiceID,pClientInfo)  ((This)->lpVtbl->Open(This,pszPnPServiceID,pClientInfo)) 
#define IPortableDeviceService_Capabilities(This,ppCapabilities)  ((This)->lpVtbl->Capabilities(This,ppCapabilities)) 
#define IPortableDeviceService_Content(This,ppContent)  ((This)->lpVtbl->Content(This,ppContent)) 
#define IPortableDeviceService_Methods(This,ppMethods)  ((This)->lpVtbl->Methods(This,ppMethods)) 
#define IPortableDeviceService_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IPortableDeviceService_Close(This)  ((This)->lpVtbl->Close(This)) 
#define IPortableDeviceService_GetServiceObjectID(This,ppszServiceObjectID)  ((This)->lpVtbl->GetServiceObjectID(This,ppszServiceObjectID)) 
#define IPortableDeviceService_GetPnPServiceID(This,ppszPnPServiceID)  ((This)->lpVtbl->GetPnPServiceID(This,ppszPnPServiceID)) 
#define IPortableDeviceService_Advise(This,dwFlags,pCallback,pParameters,ppszCookie)  ((This)->lpVtbl->Advise(This,dwFlags,pCallback,pParameters,ppszCookie)) 
#define IPortableDeviceService_Unadvise(This,pszCookie)  ((This)->lpVtbl->Unadvise(This,pszCookie)) 
#define IPortableDeviceService_SendCommand(This,dwFlags,pParameters,ppResults)  ((This)->lpVtbl->SendCommand(This,dwFlags,pParameters,ppResults)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceService_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceServiceCapabilities_INTERFACE_DEFINED__
#define __IPortableDeviceServiceCapabilities_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceServiceCapabilities;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("24dbd89d-413e-43e0-bd5b-197f3c56c886")IPortableDeviceServiceCapabilities:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSupportedMethods(IPortableDevicePropVariantCollection ** ppMethods) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedMethodsByFormat(REFGUID Format, IPortableDevicePropVariantCollection **ppMethods) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMethodAttributes(REFGUID Method, IPortableDeviceValues **ppAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMethodParameterAttributes(REFGUID Method, REFPROPERTYKEY Parameter, IPortableDeviceValues **ppAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedFormats(IPortableDevicePropVariantCollection **ppFormats) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFormatAttributes(REFGUID Format, IPortableDeviceValues **ppAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedFormatProperties(REFGUID Format, IPortableDeviceKeyCollection **ppKeys) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFormatPropertyAttributes(REFGUID Format, REFPROPERTYKEY Property, IPortableDeviceValues **ppAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedEvents(IPortableDevicePropVariantCollection **ppEvents) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEventAttributes(REFGUID Event, IPortableDeviceValues **ppAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEventParameterAttributes(REFGUID Event, REFPROPERTYKEY Parameter, IPortableDeviceValues **ppAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInheritedServices(const DWORD dwInheritanceType, IPortableDevicePropVariantCollection **ppServices) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFormatRenderingProfiles(REFGUID Format, IPortableDeviceValuesCollection **ppRenderingProfiles) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedCommands(IPortableDeviceKeyCollection **ppCommands) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCommandOptions(REFPROPERTYKEY Command, IPortableDeviceValues **ppOptions) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceServiceCapabilitiesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceServiceCapabilities * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceServiceCapabilities * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceServiceCapabilities * This);
    HRESULT (STDMETHODCALLTYPE *GetSupportedMethods) (IPortableDeviceServiceCapabilities * This, IPortableDevicePropVariantCollection ** ppMethods);
    HRESULT (STDMETHODCALLTYPE *GetSupportedMethodsByFormat) (IPortableDeviceServiceCapabilities * This, REFGUID Format, IPortableDevicePropVariantCollection ** ppMethods);
    HRESULT (STDMETHODCALLTYPE *GetMethodAttributes) (IPortableDeviceServiceCapabilities * This, REFGUID Method, IPortableDeviceValues ** ppAttributes);
    HRESULT (STDMETHODCALLTYPE *GetMethodParameterAttributes) (IPortableDeviceServiceCapabilities * This, REFGUID Method, REFPROPERTYKEY Parameter, IPortableDeviceValues ** ppAttributes);
    HRESULT (STDMETHODCALLTYPE *GetSupportedFormats) (IPortableDeviceServiceCapabilities * This, IPortableDevicePropVariantCollection ** ppFormats);
    HRESULT (STDMETHODCALLTYPE *GetFormatAttributes) (IPortableDeviceServiceCapabilities * This, REFGUID Format, IPortableDeviceValues ** ppAttributes);
    HRESULT (STDMETHODCALLTYPE *GetSupportedFormatProperties) (IPortableDeviceServiceCapabilities * This, REFGUID Format, IPortableDeviceKeyCollection ** ppKeys);
    HRESULT (STDMETHODCALLTYPE *GetFormatPropertyAttributes) (IPortableDeviceServiceCapabilities * This, REFGUID Format, REFPROPERTYKEY Property, IPortableDeviceValues ** ppAttributes);
    HRESULT (STDMETHODCALLTYPE *GetSupportedEvents) (IPortableDeviceServiceCapabilities * This, IPortableDevicePropVariantCollection ** ppEvents);
    HRESULT (STDMETHODCALLTYPE *GetEventAttributes) (IPortableDeviceServiceCapabilities * This, REFGUID Event, IPortableDeviceValues ** ppAttributes);
    HRESULT (STDMETHODCALLTYPE *GetEventParameterAttributes) (IPortableDeviceServiceCapabilities * This, REFGUID Event, REFPROPERTYKEY Parameter, IPortableDeviceValues ** ppAttributes);
    HRESULT (STDMETHODCALLTYPE *GetInheritedServices) (IPortableDeviceServiceCapabilities * This, const DWORD dwInheritanceType, IPortableDevicePropVariantCollection ** ppServices);
    HRESULT (STDMETHODCALLTYPE *GetFormatRenderingProfiles) (IPortableDeviceServiceCapabilities * This, REFGUID Format, IPortableDeviceValuesCollection ** ppRenderingProfiles);
    HRESULT (STDMETHODCALLTYPE *GetSupportedCommands) (IPortableDeviceServiceCapabilities * This, IPortableDeviceKeyCollection ** ppCommands);
    HRESULT (STDMETHODCALLTYPE *GetCommandOptions) (IPortableDeviceServiceCapabilities * This, REFPROPERTYKEY Command, IPortableDeviceValues ** ppOptions);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceServiceCapabilities * This);
    END_INTERFACE
} IPortableDeviceServiceCapabilitiesVtbl;

interface IPortableDeviceServiceCapabilities {
    CONST_VTBL struct IPortableDeviceServiceCapabilitiesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceServiceCapabilities_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceServiceCapabilities_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceServiceCapabilities_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceServiceCapabilities_GetSupportedMethods(This,ppMethods)  ((This)->lpVtbl->GetSupportedMethods(This,ppMethods)) 
#define IPortableDeviceServiceCapabilities_GetSupportedMethodsByFormat(This,Format,ppMethods)  ((This)->lpVtbl->GetSupportedMethodsByFormat(This,Format,ppMethods)) 
#define IPortableDeviceServiceCapabilities_GetMethodAttributes(This,Method,ppAttributes)  ((This)->lpVtbl->GetMethodAttributes(This,Method,ppAttributes)) 
#define IPortableDeviceServiceCapabilities_GetMethodParameterAttributes(This,Method,Parameter,ppAttributes)  ((This)->lpVtbl->GetMethodParameterAttributes(This,Method,Parameter,ppAttributes)) 
#define IPortableDeviceServiceCapabilities_GetSupportedFormats(This,ppFormats)  ((This)->lpVtbl->GetSupportedFormats(This,ppFormats)) 
#define IPortableDeviceServiceCapabilities_GetFormatAttributes(This,Format,ppAttributes)  ((This)->lpVtbl->GetFormatAttributes(This,Format,ppAttributes)) 
#define IPortableDeviceServiceCapabilities_GetSupportedFormatProperties(This,Format,ppKeys)  ((This)->lpVtbl->GetSupportedFormatProperties(This,Format,ppKeys)) 
#define IPortableDeviceServiceCapabilities_GetFormatPropertyAttributes(This,Format,Property,ppAttributes)  ((This)->lpVtbl->GetFormatPropertyAttributes(This,Format,Property,ppAttributes)) 
#define IPortableDeviceServiceCapabilities_GetSupportedEvents(This,ppEvents)  ((This)->lpVtbl->GetSupportedEvents(This,ppEvents)) 
#define IPortableDeviceServiceCapabilities_GetEventAttributes(This,Event,ppAttributes)  ((This)->lpVtbl->GetEventAttributes(This,Event,ppAttributes)) 
#define IPortableDeviceServiceCapabilities_GetEventParameterAttributes(This,Event,Parameter,ppAttributes)  ((This)->lpVtbl->GetEventParameterAttributes(This,Event,Parameter,ppAttributes)) 
#define IPortableDeviceServiceCapabilities_GetInheritedServices(This,dwInheritanceType,ppServices)  ((This)->lpVtbl->GetInheritedServices(This,dwInheritanceType,ppServices)) 
#define IPortableDeviceServiceCapabilities_GetFormatRenderingProfiles(This,Format,ppRenderingProfiles)  ((This)->lpVtbl->GetFormatRenderingProfiles(This,Format,ppRenderingProfiles)) 
#define IPortableDeviceServiceCapabilities_GetSupportedCommands(This,ppCommands)  ((This)->lpVtbl->GetSupportedCommands(This,ppCommands)) 
#define IPortableDeviceServiceCapabilities_GetCommandOptions(This,Command,ppOptions)  ((This)->lpVtbl->GetCommandOptions(This,Command,ppOptions)) 
#define IPortableDeviceServiceCapabilities_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceServiceCapabilities_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceServiceMethods_INTERFACE_DEFINED__
#define __IPortableDeviceServiceMethods_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceServiceMethods;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("e20333c9-fd34-412d-a381-cc6f2d820df7")IPortableDeviceServiceMethods:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Invoke(REFGUID Method, IPortableDeviceValues * pParameters, IPortableDeviceValues ** ppResults) = 0;
    virtual HRESULT STDMETHODCALLTYPE InvokeAsync(REFGUID Method, IPortableDeviceValues *pParameters, IPortableDeviceServiceMethodCallback *pCallback) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(IPortableDeviceServiceMethodCallback *pCallback) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceServiceMethodsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceServiceMethods * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceServiceMethods * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceServiceMethods * This);
    HRESULT (STDMETHODCALLTYPE *Invoke) (IPortableDeviceServiceMethods * This, REFGUID Method, IPortableDeviceValues * pParameters, IPortableDeviceValues ** ppResults);
    HRESULT (STDMETHODCALLTYPE *InvokeAsync) (IPortableDeviceServiceMethods * This, REFGUID Method, IPortableDeviceValues * pParameters, IPortableDeviceServiceMethodCallback * pCallback);
    HRESULT (STDMETHODCALLTYPE *Cancel) (IPortableDeviceServiceMethods * This, IPortableDeviceServiceMethodCallback * pCallback);
    END_INTERFACE
} IPortableDeviceServiceMethodsVtbl;

interface IPortableDeviceServiceMethods {
    CONST_VTBL struct IPortableDeviceServiceMethodsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceServiceMethods_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceServiceMethods_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceServiceMethods_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceServiceMethods_Invoke(This,Method,pParameters,ppResults)  ((This)->lpVtbl->Invoke(This,Method,pParameters,ppResults)) 
#define IPortableDeviceServiceMethods_InvokeAsync(This,Method,pParameters,pCallback)  ((This)->lpVtbl->InvokeAsync(This,Method,pParameters,pCallback)) 
#define IPortableDeviceServiceMethods_Cancel(This,pCallback)  ((This)->lpVtbl->Cancel(This,pCallback)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceServiceMethods_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceServiceMethodCallback_INTERFACE_DEFINED__
#define __IPortableDeviceServiceMethodCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceServiceMethodCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("c424233c-afce-4828-a756-7ed7a2350083")IPortableDeviceServiceMethodCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnComplete(HRESULT hrStatus, IPortableDeviceValues * pResults) = 0;
};
#else /* C style interface */

typedef struct IPortableDeviceServiceMethodCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceServiceMethodCallback * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceServiceMethodCallback * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceServiceMethodCallback * This);
    HRESULT (STDMETHODCALLTYPE *OnComplete) (IPortableDeviceServiceMethodCallback * This, HRESULT hrStatus, IPortableDeviceValues * pResults);
    END_INTERFACE
} IPortableDeviceServiceMethodCallbackVtbl;

interface IPortableDeviceServiceMethodCallback {
    CONST_VTBL struct IPortableDeviceServiceMethodCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceServiceMethodCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceServiceMethodCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceServiceMethodCallback_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceServiceMethodCallback_OnComplete(This,hrStatus,pResults)  ((This)->lpVtbl->OnComplete(This,hrStatus,pResults)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceServiceMethodCallback_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceDispatchFactory_INTERFACE_DEFINED__
#define __IPortableDeviceDispatchFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceDispatchFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5e1eafc3-e3d7-4132-96fa-759c0f9d1e0f")IPortableDeviceDispatchFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetDeviceDispatch(LPCWSTR pszPnPDeviceID, IDispatch ** ppDeviceDispatch) = 0;

};
#else /* C style interface */

typedef struct IPortableDeviceDispatchFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IPortableDeviceDispatchFactory * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef) (IPortableDeviceDispatchFactory * This);
    ULONG (STDMETHODCALLTYPE *Release) (IPortableDeviceDispatchFactory * This);
    HRESULT (STDMETHODCALLTYPE *GetDeviceDispatch) (IPortableDeviceDispatchFactory * This, LPCWSTR pszPnPDeviceID, IDispatch ** ppDeviceDispatch);
    END_INTERFACE
} IPortableDeviceDispatchFactoryVtbl;

interface IPortableDeviceDispatchFactory {
    CONST_VTBL struct IPortableDeviceDispatchFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceDispatchFactory_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceDispatchFactory_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceDispatchFactory_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceDispatchFactory_GetDeviceDispatch(This,pszPnPDeviceID,ppDeviceDispatch)  ((This)->lpVtbl->GetDeviceDispatch(This,pszPnPDeviceID,ppDeviceDispatch)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceDispatchFactory_INTERFACE_DEFINED__ */

#ifndef __PortableDeviceApiLib_LIBRARY_DEFINED__
#define __PortableDeviceApiLib_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_PortableDeviceApiLib;

EXTERN_C const CLSID CLSID_PortableDevice;

#ifdef __cplusplus
class DECLSPEC_UUID("728a21c5-3d9e-48d7-9810-864848f0f404") PortableDevice;
#endif

EXTERN_C const CLSID CLSID_PortableDeviceManager;

#ifdef __cplusplus
class DECLSPEC_UUID("0af10cec-2ecd-4b92-9581-34f6ae0637f3") PortableDeviceManager;
#endif

EXTERN_C const CLSID CLSID_PortableDeviceService;

#ifdef __cplusplus
class DECLSPEC_UUID("ef5db4c2-9312-422c-9152-411cd9c4dd84") PortableDeviceService;
#endif

EXTERN_C const CLSID CLSID_PortableDeviceDispatchFactory;

#ifdef __cplusplus
class DECLSPEC_UUID("43232233-8338-4658-ae01-0b4ae830b6b0") PortableDeviceDispatchFactory;
#endif

EXTERN_C const CLSID CLSID_PortableDeviceFTM;

#ifdef __cplusplus
class DECLSPEC_UUID("f7c0039a-4762-488a-b4b3-760ef9a1ba9b") PortableDeviceFTM;
#endif

EXTERN_C const CLSID CLSID_PortableDeviceServiceFTM;

#ifdef __cplusplus
class DECLSPEC_UUID("1649b154-c794-497a-9b03-f3f0121302f3") PortableDeviceServiceFTM;
#endif
#endif /* __PortableDeviceApiLib_LIBRARY_DEFINED__ */

#endif /* _WIN32_WINNT >= 0x0501 */

extern RPC_IF_HANDLE __MIDL_itf_PortableDeviceApi_0000_0018_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_PortableDeviceApi_0000_0018_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _PORTABLEDEVICEAPI_H */
