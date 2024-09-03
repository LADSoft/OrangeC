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

#ifndef _VSADMIN_H
#define _VSADMIN_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IVssAdmin_FWD_DEFINED__
#define __IVssAdmin_FWD_DEFINED__
typedef interface IVssAdmin IVssAdmin;
#endif /* __IVssAdmin_FWD_DEFINED__ */

#ifndef __IVssAdminEx_FWD_DEFINED__
#define __IVssAdminEx_FWD_DEFINED__
typedef interface IVssAdminEx IVssAdminEx;
#endif /* __IVssAdminEx_FWD_DEFINED__ */

#ifndef __VSSCoordinator_FWD_DEFINED__
#define __VSSCoordinator_FWD_DEFINED__

#ifdef __cplusplus
typedef class VSSCoordinator VSSCoordinator;
#else
typedef struct VSSCoordinator VSSCoordinator;
#endif /* __cplusplus */

#endif /* __VSSCoordinator_FWD_DEFINED__ */

#include "oaidl.h"
#include "ocidl.h"
#include "vss.h"

#ifdef __cplusplus
extern "C"{
#endif 

#ifndef __IVssAdmin_INTERFACE_DEFINED__
#define __IVssAdmin_INTERFACE_DEFINED__

EXTERN_C const IID IID_IVssAdmin;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("77ED5996-2F63-11d3-8A39-00C04F72D8E3") IVssAdmin:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE RegisterProvider(VSS_ID, CLSID, VSS_PWSZ, VSS_PROVIDER_TYPE, VSS_PWSZ, VSS_ID) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnregisterProvider(VSS_ID) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryProviders(IVssEnumObject **) = 0;
    virtual HRESULT STDMETHODCALLTYPE AbortAllSnapshotsInProgress(void) = 0;
};
#else 	/* C style interface */

typedef struct IVssAdminVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVssAdmin *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IVssAdmin *);
    ULONG (STDMETHODCALLTYPE *Release)(IVssAdmin *);
    HRESULT (STDMETHODCALLTYPE *RegisterProvider)(IVssAdmin *, VSS_ID, CLSID, VSS_PWSZ, VSS_PROVIDER_TYPE, VSS_PWSZ, VSS_ID);
    HRESULT (STDMETHODCALLTYPE *UnregisterProvider)(IVssAdmin *, VSS_ID);
    HRESULT (STDMETHODCALLTYPE *QueryProviders)(IVssAdmin *, IVssEnumObject **);
    HRESULT (STDMETHODCALLTYPE *AbortAllSnapshotsInProgress)(IVssAdmin *);
    END_INTERFACE
} IVssAdminVtbl;

interface IVssAdmin {
    CONST_VTBL struct IVssAdminVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IVssAdmin_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IVssAdmin_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IVssAdmin_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IVssAdmin_RegisterProvider(This,pProviderId,ClassId,pwszProviderName,eProviderType,pwszProviderVersion,ProviderVersionId)  ((This)->lpVtbl->RegisterProvider(This,pProviderId,ClassId,pwszProviderName,eProviderType,pwszProviderVersion,ProviderVersionId)) 
#define IVssAdmin_UnregisterProvider(This,ProviderId)  ((This)->lpVtbl->UnregisterProvider(This,ProviderId)) 
#define IVssAdmin_QueryProviders(This,ppEnum)  ((This)->lpVtbl->QueryProviders(This,ppEnum)) 
#define IVssAdmin_AbortAllSnapshotsInProgress(This)  ((This)->lpVtbl->AbortAllSnapshotsInProgress(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IVssAdmin_INTERFACE_DEFINED__ */


#ifndef __IVssAdminEx_INTERFACE_DEFINED__
#define __IVssAdminEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IVssAdminEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7858A9F8-B1FA-41a6-964F-B9B36B8CD8D8")IVssAdminEx:public IVssAdmin
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetProviderCapability(VSS_ID, ULONGLONG *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProviderContext(VSS_ID, LONG *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProviderContext(VSS_ID, LONG) = 0;
};
#else /* C style interface */

typedef struct IVssAdminExVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVssAdminEx *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IVssAdminEx *);
    ULONG (STDMETHODCALLTYPE *Release)(IVssAdminEx *);
    HRESULT (STDMETHODCALLTYPE *RegisterProvider)(IVssAdminEx *, VSS_ID, CLSID, VSS_PWSZ, VSS_PROVIDER_TYPE, VSS_PWSZ, VSS_ID);
    HRESULT (STDMETHODCALLTYPE *UnregisterProvider)(IVssAdminEx *, VSS_ID);
    HRESULT (STDMETHODCALLTYPE *QueryProviders)(IVssAdminEx *, IVssEnumObject **);
    HRESULT (STDMETHODCALLTYPE *AbortAllSnapshotsInProgress)(IVssAdminEx *);
    HRESULT (STDMETHODCALLTYPE *GetProviderCapability)(IVssAdminEx *, VSS_ID, ULONGLONG *);
    HRESULT (STDMETHODCALLTYPE *GetProviderContext)(IVssAdminEx *, VSS_ID, LONG *);
    HRESULT (STDMETHODCALLTYPE *SetProviderContext)(IVssAdminEx *, VSS_ID, LONG);
    END_INTERFACE
} IVssAdminExVtbl;

interface IVssAdminEx {
    CONST_VTBL struct IVssAdminExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IVssAdminEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IVssAdminEx_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IVssAdminEx_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IVssAdminEx_RegisterProvider(This,pProviderId,ClassId,pwszProviderName,eProviderType,pwszProviderVersion,ProviderVersionId)  ((This)->lpVtbl->RegisterProvider(This,pProviderId,ClassId,pwszProviderName,eProviderType,pwszProviderVersion,ProviderVersionId)) 
#define IVssAdminEx_UnregisterProvider(This,ProviderId)  ((This)->lpVtbl->UnregisterProvider(This,ProviderId)) 
#define IVssAdminEx_QueryProviders(This,ppEnum)  ((This)->lpVtbl->QueryProviders(This,ppEnum)) 
#define IVssAdminEx_AbortAllSnapshotsInProgress(This)  ((This)->lpVtbl->AbortAllSnapshotsInProgress(This)) 
#define IVssAdminEx_GetProviderCapability(This,pProviderId,pllOriginalCapabilityMask)  ((This)->lpVtbl->GetProviderCapability(This,pProviderId,pllOriginalCapabilityMask)) 
#define IVssAdminEx_GetProviderContext(This,ProviderId,plContext)  ((This)->lpVtbl->GetProviderContext(This,ProviderId,plContext)) 
#define IVssAdminEx_SetProviderContext(This,ProviderId,lContext)  ((This)->lpVtbl->SetProviderContext(This,ProviderId,lContext)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IVssAdminEx_INTERFACE_DEFINED__ */


#ifndef __VSSAdmin_LIBRARY_DEFINED__
#define __VSSAdmin_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_VSSAdmin;

EXTERN_C const CLSID CLSID_VSSCoordinator;

#ifdef __cplusplus
class DECLSPEC_UUID("E579AB5F-1CC4-44b4-BED9-DE0991FF0623") VSSCoordinator;
#endif
#endif /* __VSSAdmin_LIBRARY_DEFINED__ */

#ifdef __cplusplus
}
#endif

#endif /* _VSADMIN_H */
