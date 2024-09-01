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

#ifndef _CAMERAUICONTROL_H
#define _CAMERAUICONTROL_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward declarations */ 

#ifndef __ICameraUIControlEventCallback_FWD_DEFINED__
#define __ICameraUIControlEventCallback_FWD_DEFINED__
typedef interface ICameraUIControlEventCallback ICameraUIControlEventCallback;
#endif /* __ICameraUIControlEventCallback_FWD_DEFINED__ */

#ifndef __ICameraUIControl_FWD_DEFINED__
#define __ICameraUIControl_FWD_DEFINED__
typedef interface ICameraUIControl ICameraUIControl;
#endif /* __ICameraUIControl_FWD_DEFINED__ */

#ifndef __CameraUIControl_FWD_DEFINED__
#define __CameraUIControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class CameraUIControl CameraUIControl;
#else
typedef struct CameraUIControl CameraUIControl;
#endif /* __cplusplus */

#endif /* __CameraUIControl_FWD_DEFINED__ */

#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

#if (NTDDI_VERSION >= NTDDI_WIN8)
typedef enum CameraUIControlMode {
    Browse = 0,
    Linear = (Browse + 1)
} CameraUIControlMode;

typedef enum CameraUIControlLinearSelectionMode {
    Single = 0,
    Multiple = (Single + 1)
} CameraUIControlLinearSelectionMode;

typedef enum CameraUIControlCaptureMode {
    PhotoOrVideo = 0,
    Photo = (PhotoOrVideo + 1),
    Video = (Photo + 1)
} CameraUIControlCaptureMode;

typedef enum CameraUIControlPhotoFormat {
    Jpeg = 0,
    Png = (Jpeg + 1),
    JpegXR = (Png + 1)
} CameraUIControlPhotoFormat;

typedef enum CameraUIControlVideoFormat {
    Mp4 = 0,
    Wmv = (Mp4 + 1)
} CameraUIControlVideoFormat;

typedef enum CameraUIControlViewType {
    SingleItem = 0,
    ItemList = (SingleItem + 1)
} CameraUIControlViewType;

extern RPC_IF_HANDLE __MIDL_itf_camerauicontrol_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_camerauicontrol_0000_0000_v0_0_s_ifspec;

#ifndef __ICameraUIControlEventCallback_INTERFACE_DEFINED__
#define __ICameraUIControlEventCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICameraUIControlEventCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1BFA0C2C-FBCD-4776-BDA4-88BF974E74F4")ICameraUIControlEventCallback:public IUnknown
{
    public:
    virtual void STDMETHODCALLTYPE OnStartupComplete(void) = 0;
    virtual void STDMETHODCALLTYPE OnSuspendComplete(void) = 0;
    virtual void STDMETHODCALLTYPE OnItemCaptured(  LPCWSTR pszPath) = 0;
    virtual void STDMETHODCALLTYPE OnItemDeleted(  LPCWSTR pszPath) = 0;
    virtual void STDMETHODCALLTYPE OnClosed(void) = 0;
};
#else /* C style interface */

typedef struct ICameraUIControlEventCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ICameraUIControlEventCallback *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(ICameraUIControlEventCallback *);
    ULONG (STDMETHODCALLTYPE *Release)(ICameraUIControlEventCallback *);
    void (STDMETHODCALLTYPE *OnStartupComplete)(ICameraUIControlEventCallback *);
    void (STDMETHODCALLTYPE *OnSuspendComplete)(ICameraUIControlEventCallback *);
    void (STDMETHODCALLTYPE *OnItemCaptured)(ICameraUIControlEventCallback *, LPCWSTR);
    void (STDMETHODCALLTYPE *OnItemDeleted)(ICameraUIControlEventCallback *, LPCWSTR);
    void (STDMETHODCALLTYPE *OnClosed)(ICameraUIControlEventCallback *);
    END_INTERFACE
} ICameraUIControlEventCallbackVtbl;

interface ICameraUIControlEventCallback {
    CONST_VTBL struct ICameraUIControlEventCallbackVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define ICameraUIControlEventCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define ICameraUIControlEventCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ICameraUIControlEventCallback_Release(This)  ((This)->lpVtbl->Release(This)) 
#define ICameraUIControlEventCallback_OnStartupComplete(This)  ((This)->lpVtbl->OnStartupComplete(This)) 
#define ICameraUIControlEventCallback_OnSuspendComplete(This)  ((This)->lpVtbl->OnSuspendComplete(This)) 
#define ICameraUIControlEventCallback_OnItemCaptured(This,pszPath)  ((This)->lpVtbl->OnItemCaptured(This,pszPath)) 
#define ICameraUIControlEventCallback_OnItemDeleted(This,pszPath)  ((This)->lpVtbl->OnItemDeleted(This,pszPath)) 
#define ICameraUIControlEventCallback_OnClosed(This)  ((This)->lpVtbl->OnClosed(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ICameraUIControlEventCallback_INTERFACE_DEFINED__ */


#ifndef __ICameraUIControl_INTERFACE_DEFINED__
#define __ICameraUIControl_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICameraUIControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("B8733ADF-3D68-4b8f-BB08-E28A0BED0376")ICameraUIControl:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Show(IUnknown *, CameraUIControlMode, CameraUIControlLinearSelectionMode, CameraUIControlCaptureMode, CameraUIControlPhotoFormat, CameraUIControlVideoFormat, BOOL, ICameraUIControlEventCallback *) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Suspend(BOOL *) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resume(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentViewType(CameraUIControlViewType *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetActiveItem(BSTR *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(SAFEARRAY **) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveCapturedItem(LPCWSTR) = 0;
};
#else /* C style interface */

typedef struct ICameraUIControlVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ICameraUIControl *, REFIID, void **);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICameraUIControl *);
    ULONG(STDMETHODCALLTYPE *Release)(ICameraUIControl *);
    HRESULT(STDMETHODCALLTYPE *Show)(ICameraUIControl *, IUnknown *, CameraUIControlMode, CameraUIControlLinearSelectionMode, CameraUIControlCaptureMode, CameraUIControlPhotoFormat, CameraUIControlVideoFormat, BOOL, ICameraUIControlEventCallback *);
    HRESULT(STDMETHODCALLTYPE *Close)(ICameraUIControl *);
    HRESULT(STDMETHODCALLTYPE *Suspend)(ICameraUIControl *, BOOL *);
    HRESULT(STDMETHODCALLTYPE *Resume)(ICameraUIControl *);
    HRESULT(STDMETHODCALLTYPE *GetCurrentViewType)(ICameraUIControl *, CameraUIControlViewType *);
    HRESULT(STDMETHODCALLTYPE *GetActiveItem)(ICameraUIControl *, BSTR *);
    HRESULT(STDMETHODCALLTYPE *GetSelectedItems)(ICameraUIControl *, SAFEARRAY **);
    HRESULT(STDMETHODCALLTYPE *RemoveCapturedItem)(ICameraUIControl *, LPCWSTR);
    END_INTERFACE
} ICameraUIControlVtbl;

interface ICameraUIControl {
    CONST_VTBL struct ICameraUIControlVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICameraUIControl_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define ICameraUIControl_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ICameraUIControl_Release(This)  ((This)->lpVtbl->Release(This))
#define ICameraUIControl_Show(This,pWindow,mode,selectionMode,captureMode,photoFormat,videoFormat,bHasCloseButton,pEventCallback)  ((This)->lpVtbl->Show(This,pWindow,mode,selectionMode,captureMode,photoFormat,videoFormat,bHasCloseButton,pEventCallback)) 
#define ICameraUIControl_Close(This)  ((This)->lpVtbl->Close(This))
#define ICameraUIControl_Suspend(This,pbDeferralRequired)  ((This)->lpVtbl->Suspend(This,pbDeferralRequired))
#define ICameraUIControl_Resume(This)  ((This)->lpVtbl->Resume(This))
#define ICameraUIControl_GetCurrentViewType(This,pViewType)  ((This)->lpVtbl->GetCurrentViewType(This,pViewType)) 
#define ICameraUIControl_GetActiveItem(This,pbstrActiveItemPath)  ((This)->lpVtbl->GetActiveItem(This,pbstrActiveItemPath)) 
#define ICameraUIControl_GetSelectedItems(This,ppSelectedItemPaths)  ((This)->lpVtbl->GetSelectedItems(This,ppSelectedItemPaths)) 
#define ICameraUIControl_RemoveCapturedItem(This,pszPath)  ((This)->lpVtbl->RemoveCapturedItem(This,pszPath)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ICameraUIControl_INTERFACE_DEFINED__ */


#ifndef __CameraUIControlTypesLib_LIBRARY_DEFINED__
#define __CameraUIControlTypesLib_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_CameraUIControlTypesLib;
EXTERN_C const CLSID CLSID_CameraUIControl;

#ifdef __cplusplus
class DECLSPEC_UUID("16D5A2BE-B1C5-47b3-8EAE-CCBCF452C7E8") CameraUIControl;
#endif
#endif /* __CameraUIControlTypesLib_LIBRARY_DEFINED__ */

#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

extern RPC_IF_HANDLE __MIDL_itf_camerauicontrol_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_camerauicontrol_0000_0003_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _CAMERAUICONTROL_H */
